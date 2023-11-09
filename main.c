#include "headers.h"
#include "declarations.h"

int main(void)
{
	msgG = generateMessage;
	msgP = parseMessage;

	Aircraft * message;
	
	#ifdef DEBUG
	printf("%s: BEGIN \n",__func__);
        #endif

	#ifdef DEBUG
	printf("Starting...\n");
	message = (*msgG)(message);
	message = (*msgP)(message);
        #endif

	#ifdef DEBUG
	printf("%s: END \n",__func__);
        #endif

	return 0;
}
