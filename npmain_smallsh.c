// Neel Patel
// 110031678
#include "smallsh.h"
#include<unistd.h>
#include<string.h>



char buffer[256];
char *prompt;



int userin(char *p);
void procline(void);

int main()
{
	prompt = strcat(getcwd(buffer,sizeof(buffer)), "$");
	while (userin(prompt) != EOF)
		procline();
}

