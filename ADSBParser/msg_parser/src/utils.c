
#include "defs.h"
#include "msgStruct.h"

/* 
 * Given the Downlink Format (DF) of the message, return the message length
 * in bits. */

int modesMessageLenByType(int type) {
    if (type == 16 || type == 17 ||
        type == 19 || type == 20 ||
        type == 21)
        return MODES_LONG_MSG_BITS;
    else
        return MODES_SHORT_MSG_BITS;
}

/*
 * function getMsgType()
 * it extracts Message type from msg bit stream. 
 * TC,5 means Type code and 5 bits information from bit stream
 * 
 * @Param msg: input message bit stream
 *
 * @return : return the Message type eg. 17, 11 etc..
 *
 */
int getMsgType(UCHAR *msg) {
	return msg[0]>>3;
}



uint32_t getCrcFromMsg(UCHAR *msg, int msgbits) {
	return ((uint32_t)msg[(msgbits/8)-3] << 16) |
            ((uint32_t)msg[(msgbits/8)-2] << 8) |
            (uint32_t)msg[(msgbits/8)-1];
}


void readICAOAddress(struct modesMessage *mm, UCHAR *msg) {
	/* ICAO address */
	mm->aa1 = msg[1];
	mm->aa2 = msg[2];
	mm->aa3 = msg[3];
}





