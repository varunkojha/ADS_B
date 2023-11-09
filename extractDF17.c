#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() 
{
	int i = 0, j = 0, k = 0;
	size_t count;
	char buff[30];
	char *line = malloc(30);
	char search_string[] = "*8D";

	FILE *file, *fout;  /* FILE pointers  */
	file = fopen("Recording_2-28-2019.txt", "r");  /* open a text file for reading */
	fout = fopen("DF_17_messages.txt", "w");

	while(getline(&line, &count, file) != -1) 
	{
		if(strstr(line, search_string))
		{
			k = 0, j = 1;
			while(line[j] != ';')
			{
				buff[k++] = line[j++];
			}
			buff[k] = 10;
			buff[k + 1] = '\0';
			fputs(buff, fout);
			i++;
		}
	}
	printf("Total no. of messages = %d\n", i);

	fclose(file);
	fclose(fout);

	return 0;
}
