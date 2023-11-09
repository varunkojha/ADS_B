#ifndef _ADS_MESSAGE_DECODER_H
#define _ADS_MESSAGE_DECODER_H

#include "msgStruct.h"
#include "errorcodes.h"
#include "parser_status.h"


/*
 * function decodeModeSMessage()
 * function decodes the received Message in HEX
 * Message format as below, 
 * |    | ICAO24 |      DATA      |  CRC   |
 * |----|--------|----------------|--------|
 *
 * Below is DATA Field 
 * +------+------+------+------+------+------+------+------+------+------+
 * | TC,5 | EC,3 | C1,6 | C2,6 | C3,6 | C4,6 | C5,6 | C6,6 | C7,6 | C8,6 |
 * +------+------+------+------+------+------+------+------+------+------+
 *
 * @Param msgbytes Message data in Hex format
 * @return void :TODO Need to decide the Return structure.
 *
 */

int decodeModeSMessage(ParserStatus *self, struct modesMessage *mm, UCHAR *msg, int length);

#endif

