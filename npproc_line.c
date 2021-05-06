// Neel Patel
// 110031678
#include "smallsh.h"
#include <string.h>
#include <stdlib.h>

int gettok(char **outptr);
int runcommand(char **cline, int where);
void buffer_count(int c);


void procline(void) 
{
	char *arg[MAXARG + 1]; 
	int toktype; 
	int narg; 
	int type; 

	narg = 0;

	for (;;) 
	{
		
	   switch (toktype = gettok(&arg[narg])) {
		case ARG:
			if (narg < MAXARG) {
				narg++;
			}
			buffer_count(narg);
			break;
		case EOL:
		case SEMICOLON:
		case AMPERSAND:
			if (toktype == AMPERSAND)
				type = BACKGROUND;
			else
				type = FOREGROUND;
			if (narg != 0)
			{
				arg[narg] = NULL;
				runcommand(arg, type);
			}
			if (toktype == EOL)
				return;
			narg = 0;
			break;
	  }
	  
	}
}
