#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "msgStruct.h"

#define MESSAGE_LENGTH 64
typedef struct Message {
	long mtype;
	union msgData{
		char buffer[MESSAGE_LENGTH];
		struct modesMessage modeSMsg;
	}data;

	int err;
} Message;

void clearMessage(Message* msg);
void setMessage(Message* msg, char* buffer, int len, int mtype);


#endif