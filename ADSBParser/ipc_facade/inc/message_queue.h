#ifndef MESSAGE_QUEUE_FACEDE_H
#define MESSAGE_QUEUE_FACEDE_H

#include <sys/msg.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "message.h"

int messageQueueCreate(key_t key);
int messageQueueDelete(int qid);

int messageQueueSend(int qid, Message *msg);
int messageQueueReceive(int qid, Message *msg, long type);

#endif