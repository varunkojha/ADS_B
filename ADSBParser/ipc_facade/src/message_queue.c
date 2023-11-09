#include "message_queue.h"


/***************************************************
 * Function: messageQueueCreate(key_t key)
 * @Param key: A key (from ftok) used to create a queue.
 * Purpose: Creates a queue based on key.
 * @Return: The qid on success, -1 on failure.
 **************************************************/

int messageQueueCreate(key_t key)
{
	int qid;
	if ((qid = msgget(key, 0666)) == -1)
		return -1;

	return qid;
}


/*********************************************************
 * Function: messageQueueDelete(int qid)
 * Param qid: The queue ID of the message queue to be deleted.
 * Purpose: Deletes a message queue to release system resources.
 * Returns: -1 on failure, 0 on success.
 *********************************************************/
int messageQueueDelete(int qid)
{
	return msgctl(qid, IPC_RMID, NULL);
}

/****************************************************
 * Function: messageQueueSend(int qid, BMsg *msg)
 * @Param qid: The queue ID of the message queue.
 * @Param msg: A BMsg struct that has been initialized
 * 		  and will be sent over the message queue
 * Purpose: Sends a message over a queue denoted by qid
 * @Return: -1 on failure, 0 on success.
 ****************************************************/
int messageQueueSend(int qid, Message* msg)
{
	int ret;
	if ((ret = msgsnd(qid, msg, sizeof(Message) - sizeof(long), 0)) == -1)
	{
		msg->err=errno;
		return -1;
	}
	return ret;
}


/*******************************************************
 * Function: messageQueueReceive(int qid, long type, MMsg *msg)
 * @Param qid: The queue ID of the message queue.
 * @Param type: The specific type of message you want to get.
 *                 Can be any integer.
 * @Param msg: A MMsg struct that will store the return data.
 * Purpose: Receives a message from the message queue.
 * @Return: -1 on failure, 0 on success.
 *********************************************************/
int messageQueueReceive(int qid, Message* msg, long type)
{
	int ret;
	if ((ret = msgrcv(qid, msg, sizeof(Message) - sizeof(long), type, 0)) == -1)
	{
		msg->err=errno;
		return -1;
	}
	return ret;
}