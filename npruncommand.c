// Neel Patel
// 110031678
#include "smallsh.h"
#include <unistd.h>
#include <stdlib.h>
#include<string.h>
#include<errno.h>
#include <stdbool.h>
#include<fcntl.h>


// Method for changing directory
void change_directory(const char *argv);
// Method for terminating shell program
void terminate_shell();
//Method for getting total count of command along with arguments entered by the user 
void buffer_count(int c);
// Calculator method
void calculator();
// Method to be used by calculator method
void childFunction(char *line);

// For storing count of total elements in buffer
int count;

// Boolean flag indicating wthether command contains pipe "|" symbol or not.
bool contain_pipe_symbol = false;
// For storing two commands in two variables after being splitted by pipe "|" symbol.
char **cmd1=NULL,**cmd2=NULL;
// For storing two commands in two variables after being splitted by redirect ">" symbol.
char **cmd3=NULL,**cmd4=NULL;
// Boolean flag indicating wthether command contains std output redirect '>' symbol or not.
bool contain_std_output_redirect_symbol = false;
// Boolean flag indicating wthether command entered is calculator or false.
bool is_command_calculator = false;

int p[2];






int runcommand(char **cline, int where)
{
	pid_t pid;
	int status;
	char buffer[MAXBUF];
	int file_descriptor;


	// Code for checking if command entered is cd or not
	// If command entered is cd then check if any argument is passed or not
	if(strcmp(cline[0],"cd") == 0) {
		if(cline[1] == 0){
			change_directory(NULL);
		}
		else { 
			change_directory(cline[1]);
		}
	}

	else {
		// Code for creating a one continous string from words in cline variable.
		for(int i=0;i<count;i++) {
			strcat(buffer, " ");
			strcat(buffer,cline[i]);
		}
		// Code for checking command contains pipe "|" symbol or not.
		if(strstr(buffer,"|") != NULL ) {
			contain_pipe_symbol = true;
			// Code for splitting commands into two parts i.e. before pipe '|' and after pipe '|'
			char *arg1 = strtok(buffer,"|");
			char *arg2 = strtok(NULL,"|");		
			//Split arg1 into token based on white space
			char *temp1   = strtok (arg1, " ");
			// Here cmd1 is created dynamically based on tokens generated.
			int p = 0;
			while (temp1) {
				cmd1 = realloc(cmd1, sizeof (char*) * ++p);
				if (cmd1 == NULL)
					exit (-1); /* memory allocation failed */
				cmd1[p-1] = temp1;
				temp1 = strtok (NULL, " ");
			}
			//One more element NULL in cmd1 added i.e. which is required by execvp function
			cmd1 = realloc(cmd1, sizeof (char*) * (p+1));
			cmd1[p] = 0;
			//Split arg2 into token based on white space
			char *temp2 = strtok(arg2, " ");
			// Here cmd2 is created dynamically based on tokens generated.
			int j = 0;
			while (temp2) {
				cmd2 = realloc (cmd2, sizeof (char*) * ++j);
				if (cmd2 == NULL)
					exit (-1); /* memory allocation failed */
				cmd2[j-1] = temp2;
				temp2 = strtok (NULL, " ");
			}
			//One more element NULL in cmd2 added i.e. which is required by execvp function
			cmd2 = realloc (cmd2, sizeof (char*) * (j+1));
			cmd2[j] = 0;
		}


		// Code for checking if command entered is exit or quit
		if(strcmp(cline[0],"exit") == 0 || strcmp(cline[0],"quit") == 0) {
			terminate_shell();
		}


		//Code for checking if command contains std output redirect '>' symbol or not
		if(strstr(buffer,">") != NULL) {
			contain_std_output_redirect_symbol = true;
			// Code for splitting commands into two parts i.e. before redirect '>' and after redirect '>'
			char *arg1 = strtok(buffer,">");
			char *arg2 = strtok(NULL,">");		
			//Split arg1 into token based on white space
			char *temp1   = strtok (arg1, " ");
			// Here cmd3 is created dynamically based on tokens generated.
			int l = 0;
			while (temp1) {
				cmd3 = realloc (cmd3, sizeof (char*) * ++l);
				if (cmd3 == NULL)
					exit (-1); /* memory allocation failed */
				cmd3[l-1] = temp1;
				temp1 = strtok (NULL, " ");
			}
			//One more element NULL in cmd3 added i.e. which is required by execvp function
			cmd3 = realloc (cmd3, sizeof (char*) * (l+1));
			cmd3[l] = 0;
			//Split arg2 into token based on white space
			char *temp2   = strtok(arg2, " ");
			// Here cmd4 is created dynamically based on tokens generated.
			int k = 0;
			while (temp2) {
				cmd4 = realloc (cmd4, sizeof (char*) * ++k);
				if (cmd4 == NULL)
					exit (-1); /* memory allocation failed */
				cmd4[k-1] = temp2;
				temp2 = strtok (NULL, " ");
			}
		}

		if(strcmp(cline[0],"calculator") == 0) {
			is_command_calculator = true;
		}
	
	
		//Execute command entered by the user
		switch (pid = fork()) {
			case -1:
				perror("smallsh");
				return (-1);
			case 0:
				if(contain_pipe_symbol == true) {
					int pid1,status;
					if(pipe(p) == -1) {
						perror("Unable to create pipe \n");
					}
					pid1 = fork();
					if(pid1 == -1) {
						perror("Error while creating process \n");
					}
					// One new process created for executing one part of command before pipe
					// "|" symbol
					if(pid1 == 0){
						// standard output redirected to pipe, so another process can read from it.
						dup2(p[1], 1);
						close(p[0]);
						close(p[1]);
						execvp(cmd1[0],cmd1);
						perror(*cmd1);
						exit(3);
					}
					// other process will be executing another part of command after pipe
					// "|" symbol
					if(pid1 > 0) {
						waitpid(pid1,&status,0);
						dup2(p[0], 0);
						close(p[0]);
						close(p[1]);
						execvp(cmd2[0],cmd2);
						perror(*cmd2);
						free(cmd1);
						free(cmd2);
						memset(buffer,0,MAXBUF);
						exit(2);
					}
				}
				else if(contain_pipe_symbol == false && contain_std_output_redirect_symbol == false && is_command_calculator == false) { 
					execvp(cline[0], cline);
					perror(*cline);
					exit(1);
				}
				else if(contain_std_output_redirect_symbol == true) {
					int file_descriptor = open(cmd4[0],O_WRONLY|O_CREAT|O_TRUNC,0777);
					if(file_descriptor == -1) {
						perror("Error while opening file");
					}
					dup2(file_descriptor,1);
					execvp(cmd3[0],cmd3);
					close(file_descriptor);
					free(cmd3);
					free(cmd4);
					memset(buffer,0,MAXBUF);
					exit(3);
				} 
				else if(is_command_calculator == true) {
					calculator();
					exit(4);
				}
		}
	}
	

	
	
	
		
	
	


	//code for parents
	if (where == BACKGROUND)
	{
		printf("[process id %d]\n", pid);
		return (0);
	}
	
	if (waitpid(pid, &status, 0) == -1)
		return (-1);
	else{
		contain_pipe_symbol = false;
		contain_std_output_redirect_symbol = false;
		is_command_calculator = false;
		memset(buffer,0,MAXBUF);
		return (status);
	}
}

void change_directory(const char *argv) {
	int status;
	if(argv == NULL){
		status = chdir(getenv("HOME"));
	} else {
		status = chdir(argv);
	}
	
	if(status == -1) {
		switch (errno)
		{
			case EACCES:
				perror("Permission denied");
				break;
			case EIO:
				perror("Input error");
				break;
			case ENAMETOOLONG:
				perror("Specified path is too long");
				break;
			case ENOTDIR:
				perror("The directory specified in path does not exist");
				break;
			case ENOENT: 
				perror("No such file or directory");
				break; 
		}
	}
	
	
}

void terminate_shell() {
	exit(0);
}

void buffer_count(int c) {
	count = c;
}

void childFunction(char *line)
{
    int n1,n2,operand1, operand2,sum,difference,product;
    char operator;
    float quotient;
    char buffer[500];
     
    write(1,"I am a child working for my parent\n",36); 
    n1 = sscanf(line, "%d %c %d", &operand1, &operator, &operand2);  
     
    sleep(1);
     
    if (n1 < 3)    
        exit(50);
     
    if (operator == '/' && operand2 == 0)   
        exit(100);
     
    switch (operator) 
    {
        
             
        case '-':
            difference = operand1-operand2;
            n2=sprintf(buffer,"\n>%d %c %d = %d\n", operand1, operator, operand2,difference);
            write(1,buffer,n2);
            break;
        case '+':
            sum = operand1+operand2;
            n2=sprintf(buffer,"\n>%d %c %d = %d\n", operand1, operator, operand2, sum);
            write(1,buffer,n2);
            break;
             
        case '/':
            quotient = operand1 / operand2;
            n2=sprintf(buffer,"\n>%d %c %d = %f\n", operand1, operator, operand2, quotient);
            write(1,buffer,n2);
            break;
             
        case '*':
            product = operand1 * operand2;
            n2=sprintf(buffer,"\n>%d %c %d = %d\n", operand1, operator, operand2, product);
            write(1,buffer,n2);
            break;
             
        default:
            exit(200);
            break;
    }
    exit(0);
     
}

void calculator() {
	    char str[500];
    int status;
    pid_t pid;
    write(1,"This program makes simple arithmetics\n",39);
     
    while(1)    
    {
        write(1,"Enter an arithmetic statement(eg 34+132)>\n",43);
        
        
       read(0,str,15);
         
        pid = fork();  
         
        if (pid == -1)  
        {
            perror("Fork error\n");
            exit(0);
        }
         
        else if (pid == 0)    
            childFunction(str);
         
        else
        {
            if (pid > 0)  
            {
                if(strstr(str,"exit") != NULL) {
                    exit(1);
                }
                write(1,"\nCreated a child to make your operation, waiting\n",50); 
                wait(&status);  
                 
                if (WEXITSTATUS(status) == 50)  
                    write(1,"Wrong Statement\n",17);
                 
                else if (WEXITSTATUS(status) == 100)    
                    write(1,"Division by zero\n",18);
                 
                else
                    if (WEXITSTATUS(status) == 200) 
                        write(1,"Wrong operator\n",16);
            }
        }
         
    }
}


