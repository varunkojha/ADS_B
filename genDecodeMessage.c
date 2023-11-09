#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define BUFLEN 64

int main() 
{
	//char message[BUFLEN] = "*8DA9B276990D05A6400483F2EDAF;*E1C0674F004F0E19B3DF38F00CFE;";
	int i = 0;
	size_t count;
	char *line = malloc(30);
	FILE *file, *fout;  /* FILE pointers  */
	char* token;
	
	file = fopen("Recording_2-28-2019.txt", "r");  /* open a text file for reading */

	while(getline(&line, &count, file) != -1)
	{
		++i;
		// strtok() will work similar to .split() or class StringTokenizer
		token = strtok(line, "*;");
		while(token != NULL)
		{
			// token is overwritten everytime
			printf("%d: [%p] --- %s\n", i, token, token);
			token = strtok(NULL, "*;");
			if (*(token + 1) == 10)
				break;
		}
	}

	printf("Total no. of messages = %d\n", i);

	fclose(file);

	return 0;
}
