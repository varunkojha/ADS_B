#ifndef PARSER_STATUS_H
#define PARSER_STATUS_H

#define MODE_S_ICAO_CACHE_LEN 1024 // Power of two required

typedef struct {
	// Internal state
  uint32_t icao_cache[sizeof(uint32_t)*MODE_S_ICAO_CACHE_LEN*2]; // Recently seen ICAO addresses cache

  // Configuration
  int fix_errors; // Single bit error correction if true
  int aggressive; // Aggressive detection algorithm
  int check_crc;  // Only display messages with good CRC
}ParserStatus ;

#endif