#ifndef UTILS_H_
#define UTILS_H_

int modesMessageLenByType(int type);
int getMsgType(UCHAR *msg);
uint32_t getCrcFromMsg(UCHAR *msg, int msgbits);
uint32_t computeChecksum(unsigned char *msg, int bits);
void readICAOAddress(struct modesMessage *mm, UCHAR *msg);

#endif