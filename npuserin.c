// Neel Patel
// 110031678
#include "smallsh.h"
#include<unistd.h>
#include<string.h>
#include<signal.h>


//Signal handler for handling alarm signal
void signalHandler() {
	printf("\nPress Enter key to continue now! and type command now\n");
}


// command arg1 arg2 arg3
static char inpbuf[MAXBUF], tokbuf[2 * MAXBUF],
*ptr = inpbuf, *tok = tokbuf;






int inarg(char c);

int userin(char *p)
{
	

	
	
	//Set the handler for alarm
	signal(SIGALRM,signalHandler);
	int c, count;
	ptr = inpbuf;
	tok = tokbuf;

	
	
	
	//Alarm called once
	alarm(10);

    //Code for checking enter key pressed and alarm rescheduled. ASCII Code for enter key  is '10'.
	if(c == 10) {
		alarm(10);
	}

	
	
	//Code for getting current working directory whenever prompt is printed on the screen.
	char buffer[256];
	p = strcat(getcwd(buffer,sizeof(buffer)), "$");
	printf("%s", p);

	count = 0;
	while (1)
	{
		if ((c = getchar()) == EOF)
			return(EOF);
		if (count < MAXBUF)
			inpbuf[count++] = c;
		if (c == '\n' && count < MAXBUF)
		{
			inpbuf[count] = '\0';
			return count;
		}
		
		if (c == '\n') 
		{
			printf("smallsh: input line too long\n");
			count = 0;
			printf("%s ", p);
		}
	}

	// printf("inpbuf=%s\n", inpbuf);
}

int gettok(char **outptr)
{
	int type;
	
	*outptr = tok;
	
	while (*ptr == ' ' || *ptr == '\t')
		ptr++;
	
	*tok++ = *ptr;
	
	switch (*ptr++) {
	case '\n':
		type = EOL;
		break;
	case '&':
		type = AMPERSAND;
		break;
	case ';':
		type = SEMICOLON;
		break;
	default:
		type = ARG;
		
		while (inarg(*ptr))
			*tok++ = *ptr++;
	}
	*tok++ = '\0';
	return type;
}

//****************************************************************************************
static char special[] = { ' ', '\t', '&', ';', '\n', '\0' };
int inarg(char c)
{
	char *wrk;
	for (wrk = special; *wrk; wrk++)
	{
		if (c == *wrk)
			return (0);
	}
	
	return (1);
}




