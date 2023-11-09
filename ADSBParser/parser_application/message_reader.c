#include "adsBMessageDecoder.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Remove * and ; from the string
void extract_message(char *msg, char **returned){
	int i=0;
	// Get Size of String 
	int size = strlen(msg);
	// Check if * and ; exists in string
	if(msg[0] == '*') {
		i++;
	}
	if(msg[size-1] == ';') {
		*returned = malloc(sizeof(char)*(size - i));
		memcpy(*returned, (msg+i), (size - i -1));
		returned[size-i-1] = '\0';
	}
	else {
		// If end of char is not ";" 
		*returned = malloc(sizeof(char)*(size - i));
		memcpy(*returned, (msg+i), (size - i));
		returned[size-i] = '\0';
	}
}

int convert_to_hex(char* msg, unsigned char **ret) {
	int size = strlen(msg);
	int i = 0;
	int j = 0;
	char high_nibble=0x00, low_nibble=0x00;
	unsigned char temp[sizeof(char) * ((size/2)+1)];

	*ret = malloc(sizeof(char) * ((size/2)+1));

	for (;i<size;i=i+2) {
		high_nibble = (msg[i] < 58) ? msg[i] - '0' : msg[i] - 55;
		low_nibble = (msg[i+1] < 58) ? msg[i+1] - '0' : msg[i+1] - 55;
		temp[j] = high_nibble << 4 | low_nibble;
		j++;
	}
	temp[size/2] = '\0';
	memcpy(*ret, temp, size/2);
	return size/2;
}

int main1(int argc, char *argv[]) {
	//char *pMsg = "*02E19338D496AE;";
	char *pMsg = "*8DABC4BE58C901616A021ACC932A;";
	char *msg = NULL;
	extract_message(pMsg, &msg);
	if(msg) {
		unsigned char* ret = NULL;
		printf("%s\n", msg);
		int sz = convert_to_hex(msg, &ret);

		if(ret)
		{
			int i=0;
			printf("HEX Message %d\n", sz);
			for(; i< sz; i++) {
				printf("%02X\t", ret[i]);
			}
			printf("\n");
			struct modesMessage mm;
			decodeModeSMessage(&mm , ret, sz);
			
			free(ret);
		}
		free(msg);
	}

	return 0;
}
