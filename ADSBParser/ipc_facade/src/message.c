#include "message.h"

void clearMessage(Message* msg)
{
	memset(msg->data.buffer, 0, sizeof(msg->data.buffer));
	msg->err = 0;
	msg->mtype = 0;
}

void setMessage(Message* msg, char* buffer, int len, int mtype)
{	
	assert(len <= MESSAGE_LENGTH);
	memcpy(msg->data.buffer, buffer, len);
	msg->err = 0;
	msg->mtype = mtype;
}