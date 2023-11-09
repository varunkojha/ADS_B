#include "adsBMessageDecoder.h"
#include "utils.h"
#include "stdio.h"
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <time.h>


// =============================== Initialization ===========================


static uint16_t maglut[129*129*2];
static int maglut_initialized = 0;
static const char *ais_charset = "?ABCDEFGHIJKLMNOPQRSTUVWXYZ????? ???????????????0123456789??????";


void mode_s_init(ParserStatus *self) {
  int i, q;

  self->fix_errors = 1;
  self->check_crc = 1;
  self->aggressive = 0;

  // Allocate the ICAO address cache. We use two uint32_t for every entry
  // because it's a addr / timestamp pair for every entry
  memset(&self->icao_cache, 0, sizeof(self->icao_cache));

  // Populate the I/Q -> Magnitude lookup table. It is used because sqrt or
  // round may be expensive and may vary a lot depending on the libc used.
  //
  // We scale to 0-255 range multiplying by 1.4 in order to ensure that every
  // different I/Q pair will result in a different magnitude value, not losing
  // any resolution.
  if (!maglut_initialized) {
    for (i = 0; i <= 128; i++) {
      for (q = 0; q <= 128; q++) {
        maglut[i*129+q] = round(sqrt(i*i+q*q)*360);
      }
    }
    maglut_initialized = 1;
  }
}


// Hash the ICAO address to index our cache of MODE_S_ICAO_CACHE_LEN elements,
// that is assumed to be a power of two.
uint32_t icao_cache_has_addr(uint32_t a) {
  // The following three rounds wil make sure that every bit affects every
  // output bit with ~ 50% of probability.
  a = ((a >> 16) ^ a) * 0x45d9f3b;
  a = ((a >> 16) ^ a) * 0x45d9f3b;
  a = ((a >> 16) ^ a);
  return a & (MODE_S_ICAO_CACHE_LEN-1);
}

// Add the specified entry to the cache of recently seen ICAO addresses. Note
// that we also add a timestamp so that we can make sure that the entry is only
// valid for MODE_S_ICAO_CACHE_TTL seconds.
void add_recently_seen_icao_addr(ParserStatus *self, uint32_t addr) {
  uint32_t h = icao_cache_has_addr(addr);
  self->icao_cache[h*2] = addr;
  self->icao_cache[h*2+1] = (uint32_t) time(NULL);
}

// Returns 1 if the specified ICAO address was seen in a DF format with proper
// checksum (not xored with address) no more than * MODE_S_ICAO_CACHE_TTL
// seconds ago. Otherwise returns 0.
int icao_addr_was_recently_seen(ParserStatus *self, uint32_t addr) {
  uint32_t h = icao_cache_has_addr(addr);
  uint32_t a = self->icao_cache[h*2];
  int32_t t = self->icao_cache[h*2+1];

  return a && a == addr && time(NULL)-t <= MODE_S_ICAO_CACHE_TTL;
}



// If the message type has the checksum xored with the ICAO address, try to
// brute force it using a list of recently seen ICAO addresses.
//
// Do this in a brute-force fashion by xoring the predicted CRC with the
// address XOR checksum field in the message. This will recover the address: if
// we found it in our cache, we can assume the message is ok.
//
// This function expects mm->msgtype and mm->msgbits to be correctly populated
// by the caller.
//
// On success the correct ICAO address is stored in the modesMessage structure in
// the aa3, aa2, and aa1 fiedls.
//
// If the function successfully recovers a message with a correct checksum it
// returns 1. Otherwise 0 is returned.
int brute_force_ap(ParserStatus *self, unsigned char *msg, struct modesMessage *mm) {
  unsigned char aux[MODE_S_LONG_MSG_BYTES];
  int msgtype = mm->msgtype;
  int msgbits = mm->msgbits;

  if (msgtype == 0 ||         // Short air surveillance
      msgtype == 4 ||         // Surveillance, altitude reply
      msgtype == 5 ||         // Surveillance, identity reply
      msgtype == 16 ||        // Long Air-Air survillance
      msgtype == 20 ||        // Comm-A, altitude request
      msgtype == 21 ||        // Comm-A, identity request
      msgtype == 24)          // Comm-C ELM
  {
    uint32_t addr;
    uint32_t crc;
    int lastbyte = (msgbits/8)-1;

    // Work on a copy.
    memcpy(aux, msg, msgbits/8);

    // Compute the CRC of the message and XOR it with the AP field so that
    // we recover the address, because:
    //
    // (ADDR xor CRC) xor CRC = ADDR.
    crc = computeChecksum(aux, msgbits);
    aux[lastbyte] ^= crc & 0xff;
    aux[lastbyte-1] ^= (crc >> 8) & 0xff;
    aux[lastbyte-2] ^= (crc >> 16) & 0xff;
    
    // If the obtained address exists in our cache we consider the message
    // valid.
    addr = aux[lastbyte] | (aux[lastbyte-1] << 8) | (aux[lastbyte-2] << 16);
    if (icao_addr_was_recently_seen(self, addr)) {
      mm->aa1 = aux[lastbyte-2];
      mm->aa2 = aux[lastbyte-1];
      mm->aa3 = aux[lastbyte];
      return 1;
    }
  }
  return 0;
}


// Decode the 13 bit AC altitude field (in DF 20 and others). Returns the
// altitude, and set 'unit' to either MODE_S_UNIT_METERS or MDOES_UNIT_FEETS.
int decode_ac13_field(unsigned char *msg, int *unit) {
  int m_bit = msg[3] & (1<<6);
  int q_bit = msg[3] & (1<<4);

  if (!m_bit) {
    *unit = MODE_S_UNIT_FEET;
    if (q_bit) {
      // N is the 11 bit integer resulting from the removal of bit Q and M
      int n = ((msg[2]&31)<<6) |
              ((msg[3]&0x80)>>2) |
              ((msg[3]&0x20)>>1) |
               (msg[3]&15);
      // The final altitude is due to the resulting number multiplied by
      // 25, minus 1000.
      return n*25-1000;
    } else {
      // TODO: Implement altitude where Q=0 and M=0
    }
  } else {
    *unit = MODE_S_UNIT_METERS;
    // TODO: Implement altitude when meter unit is selected.
  }
  return 0;
}

// Decode the 12 bit AC altitude field (in DF 17 and others). Returns the
// altitude or 0 if it can't be decoded.
int decode_ac12_field(unsigned char *msg, int *unit) {
  int q_bit = msg[5] & 1;

  if (q_bit) {
    // N is the 11 bit integer resulting from the removal of bit Q
    *unit = MODE_S_UNIT_FEET;
    int n = ((msg[5]>>1)<<4) | ((msg[6]&0xF0) >> 4);
    // The final altitude is due to the resulting number multiplied by 25,
    // minus 1000.
    return n*25-1000;
  } else {
    return 0;
  }
}

int decodeModeSMessage(ParserStatus *self, struct modesMessage *mm, UCHAR *msg, int length) {
	uint32_t crc2;   /* Computed CRC, used to verify the message CRC. */

	if(!mm || !msg) {

		// Error should be returned
		return ADSB_MSG_PARAM_ERR;
	}

	//memcpy(mm->msg,msg,length);

	mm->msgtype = getMsgType(msg); /**/
	mm->msgbits = modesMessageLenByType(mm->msgtype); /**/
	mm->crc = getCrcFromMsg(msg, mm->msgbits); /**/
	crc2 = computeChecksum(msg,mm->msgbits); /**/

	printf("msg %X  compute   %X\n", mm->crc, crc2);

	mm->errorbit = -1;  /* No error */
	mm->crcok = (mm->crc == crc2);

	if(!mm->crcok) {
		/* CRC Note Matching 
		 * Error correction needs to be added.
		 * TODO: Single and Two bits Error correction to be supported
		 */
	}

	mm->ca = msg[0] & 7;        /* Responder capabilities. */
	readICAOAddress(mm, msg);

	 // DF 17 type (assuming this is a DF17, otherwise not used)
  mm->metype = msg[4] >> 3;   // Extended squitter message type.
  mm->mesub = msg[4] & 7;     // Extended squitter message subtype.

  // Fields for DF4,5,20,21
  mm->fs = msg[0] & 7;        // Flight status for DF4,5,20,21
  mm->dr = msg[1] >> 3 & 31;  // Request extraction of downlink request.
  mm->um = ((msg[1] & 7)<<3)| // Request extraction of downlink request.
            msg[2]>>5;

  // In the squawk (identity) field bits are interleaved like that (message
  // bit 20 to bit 32):
  //
  // C1-A1-C2-A2-C4-A4-ZERO-B1-D1-B2-D2-B4-D4
  //
  // So every group of three bits A, B, C, D represent an integer from 0 to
  // 7.
  //
  // The actual meaning is just 4 octal numbers, but we convert it into a
  // base ten number tha happens to represent the four octal numbers.
  //
  // For more info: http://en.wikipedia.org/wiki/Gillham_code
  {
    int a, b, c, d;

    a = ((msg[3] & 0x80) >> 5) |
        ((msg[2] & 0x02) >> 0) |
        ((msg[2] & 0x08) >> 3);
    b = ((msg[3] & 0x02) << 1) |
        ((msg[3] & 0x08) >> 2) |
        ((msg[3] & 0x20) >> 5);
    c = ((msg[2] & 0x01) << 2) |
        ((msg[2] & 0x04) >> 1) |
        ((msg[2] & 0x10) >> 4);
    d = ((msg[3] & 0x01) << 2) |
        ((msg[3] & 0x04) >> 1) |
        ((msg[3] & 0x10) >> 4);
    mm->identity = a*1000 + b*100 + c*10 + d;
  }

  // DF 11 & 17: try to populate our ICAO addresses whitelist. DFs with an AP
  // field (xored addr and crc), try to decode it.
  if (mm->msgtype != 11 && mm->msgtype != 17) {
    // Check if we can check the checksum for the Downlink Formats where
    // the checksum is xored with the aircraft ICAO address. We try to
    // brute force it using a list of recently seen aircraft addresses.
    if (brute_force_ap(self, msg, mm)) {
      // We recovered the message, mark the checksum as valid.
      mm->crcok = 1;
    } else {
      mm->crcok = 0;
    }
  } else {
    // If this is DF 11 or DF 17 and the checksum was ok, we can add this
    // address to the list of recently seen addresses.
    if (mm->crcok && mm->errorbit == -1) {
      uint32_t addr = (mm->aa1 << 16) | (mm->aa2 << 8) | mm->aa3;
      add_recently_seen_icao_addr(self, addr);
    }
  }

  // Decode 13 bit altitude for DF0, DF4, DF16, DF20
  if (mm->msgtype == 0 || mm->msgtype == 4 ||
      mm->msgtype == 16 || mm->msgtype == 20) {
    mm->altitude = decode_ac13_field(msg, &mm->unit);
  }

  // Decode extended squitter specific stuff.
  if (mm->msgtype == 17) {
    // Decode the extended squitter message.

    if (mm->metype >= 1 && mm->metype <= 4) {
      // Aircraft Identification and Category
      mm->aircraft_type = mm->metype-1;
      mm->flight[0] = (ais_charset)[msg[5]>>2];
      mm->flight[1] = ais_charset[((msg[5]&3)<<4)|(msg[6]>>4)];
      mm->flight[2] = ais_charset[((msg[6]&15)<<2)|(msg[7]>>6)];
      mm->flight[3] = ais_charset[msg[7]&63];
      mm->flight[4] = ais_charset[msg[8]>>2];
      mm->flight[5] = ais_charset[((msg[8]&3)<<4)|(msg[9]>>4)];
      mm->flight[6] = ais_charset[((msg[9]&15)<<2)|(msg[10]>>6)];
      mm->flight[7] = ais_charset[msg[10]&63];
      mm->flight[8] = '\0';
    } else if (mm->metype >= 9 && mm->metype <= 18) {
      // Airborne position Message
      mm->fflag = msg[6] & (1<<2);
      mm->tflag = msg[6] & (1<<3);
      mm->altitude = decode_ac12_field(msg, &mm->unit);
      mm->raw_latitude = ((msg[6] & 3) << 15) |
                          (msg[7] << 7) |
                          (msg[8] >> 1);
      mm->raw_longitude = ((msg[8]&1) << 16) |
                           (msg[9] << 8) |
                           msg[10];
    } else if (mm->metype == 19 && mm->mesub >= 1 && mm->mesub <= 4) {
      // Airborne Velocity Message
      if (mm->mesub == 1 || mm->mesub == 2) {
        mm->ew_dir = (msg[5]&4) >> 2;
        mm->ew_velocity = ((msg[5]&3) << 8) | msg[6];
        mm->ns_dir = (msg[7]&0x80) >> 7;
        mm->ns_velocity = ((msg[7]&0x7f) << 3) | ((msg[8]&0xe0) >> 5);
        mm->vert_rate_source = (msg[8]&0x10) >> 4;
        mm->vert_rate_sign = (msg[8]&0x8) >> 3;
        mm->vert_rate = ((msg[8]&7) << 6) | ((msg[9]&0xfc) >> 2);
        // Compute velocity and angle from the two speed components
        mm->velocity = sqrt(mm->ns_velocity*mm->ns_velocity+
                            mm->ew_velocity*mm->ew_velocity);
        if (mm->velocity) {
          int ewv = mm->ew_velocity;
          int nsv = mm->ns_velocity;
          double heading;

          if (mm->ew_dir) ewv *= -1;
          if (mm->ns_dir) nsv *= -1;
          heading = atan2(ewv, nsv);

          // Convert to degrees.
          mm->heading = heading * 360 / (M_PI*2);
          // We don't want negative values but a 0-360 scale.
          if (mm->heading < 0) mm->heading += 360;
        } else {
          mm->heading = 0;
        }
      } else if (mm->mesub == 3 || mm->mesub == 4) {
        mm->heading_is_valid = msg[5] & (1<<2);
        mm->heading = (360.0/128) * (((msg[5] & 3) << 5) |
                                      (msg[6] >> 3));
      }
    }
  }
  mm->phase_corrected = 0; // Set to 1 by the caller if needed.
}
