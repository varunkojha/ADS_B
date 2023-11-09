#include <stdio.h>
#include <pthread.h>

extern void *messageReceiver();
extern void *messageParser();

pthread_t startMessageReceiverThread() {
	pthread_t thread;
	int ret;
	pthread_create( &thread, NULL, messageReceiver, NULL);
	return thread;
}


pthread_t startMessageParserThread() {
	pthread_t thread;
	int ret;
	pthread_create( &thread, NULL, messageParser, NULL);
	return thread;
}


void startApplication() {
	pthread_t thread1, thread2;

	thread1 = startMessageReceiverThread();
	thread2 = startMessageParserThread();

	pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);
}


int main(int argc, char const *argv[])
{
	/* code */
	startApplication();
	return 0;
}