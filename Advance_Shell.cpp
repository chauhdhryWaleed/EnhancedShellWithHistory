#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

void runCommand(char command[]){


	char* token;
	char* argv[40][40]; //2d array of c strings
	int i=0;
	int j=0;
	char *ptr=strstr(command,"cd");
	bool found=false;
	if(ptr)
        {


        char*path=command;
        int ret=chdir(command);
        printf("%s \n" ,command);
       if(ret!=0)
       {
         cout<<"directory not changed";
         cout<<"\n";
           exit(0);
        }
       if(ret==0)
       {

         cout<<"directory changed";
         cout<<"\n";
         exit(0);
        }
       }
       else
      {
	        
        
	//tokenizing command if space is detected
	token = strtok(command, " \n\0\t"); 
	while(token != NULL){
		if(strcmp(token, "|")==0){ //store next sub-command if pipe is detected
			argv[i][j]=NULL;
			i++;
			j=0;
			token = strtok(NULL, " \n\0\t");
		}
		else {
			argv[i][j]=token;
			token = strtok(NULL, " \n\0\t");
			j++;
		}
	}
	argv[i][j] = NULL; //storing NULL at the end
	
	int noOfCommnd = i + 1; //total number of sub-commands
	int fd[2];
	char** subCommand;
    	int oldFd = 0;
    	int fdIn, fdOut;
    	
    	for (int k = 0; k < noOfCommnd ; k++) {
       	pipe(fd);
       	subCommand = argv[k];
       	int pid = fork();	
       	wait(NULL);
       	bool flagIn = false;
    	bool flagOut = false;
    		
    		//handling input-output redirection
       	for(int l=0; subCommand[l]!= NULL ;l++){
			if(strcmp(subCommand[l], ">")==0){ //write into file if '>' is in the command
				subCommand[l]=NULL;
	  			char *inFname =  subCommand[l+1];
	  			fdOut = open(inFname,O_CREAT|O_WRONLY, 0777);
	  			flagOut = true;
	  			break;
	  		}
	  		if(strcmp(subCommand[l], ">>")==0){ //append the file if '>>' is in the command
				subCommand[l]=NULL;
	  			char *inFname =  subCommand[l+1];
	  			fdOut = open(inFname,O_APPEND|O_WRONLY, 0777);
	  			flagOut = true;
	  			break;
	  		}	
	  		if(strcmp(subCommand[l], "<")==0){ //take input from the file
				subCommand[l]=NULL;
	  			char *inFname =  subCommand[l+1];
	  			fdIn = open(inFname,O_RDONLY, 0777);
	  			flagIn = true;
	  			break;
	  		}	
		}
       	
        	if (pid == 0) {
        		//child
		   	if (oldFd != 0) {
				dup2(oldFd, 0); //STD input into old file descriptor
				close(oldFd);
		    	}

		    	dup2(fd[1], 1); //writing into the pipe
		   	close(fd[1]);
		   	
		   	//handling file descriptors if input-output redirection detected
		   	if(flagOut){
		   		dup2(fdOut,1);
	  			close(fdOut);
		   	}
       		       if(flagIn){
       			        dup2(fdIn,0);
	  			close(fdIn);
       		}
		   	
		   	
		   	if(execvp(*subCommand, subCommand) < 0){ //executing command
		   		cout<<"ERROR: Command failed!\n";
		   		exit(1);
		   	}
		   	else
		   	{
		   	exit(0);
		   	}
		   
		   	
        	}
        	close(oldFd);
       	        close(fd[1]);
     
        	oldFd = fd[0]; //storing the value of old file descriptor
    	}

   	 if (oldFd != 0) {
       	 dup2(oldFd, 0);
       	 close(oldFd);
    	}
    	if(execvp(*subCommand, subCommand) < 0){
		cout<<"ERROR: Command failed!\n";
		exit(1);
   	
    	
    	}
    	else
    	exit(0);
    	}
}


int main() {

char commands [100][100];
int commandIndex=0;
    char command[100];
    bool run=true;
  
    while (run) {
        cout << "\n\nEnter command: ";
        cin.getline(command, 100);
        if (strcmp(command, "exit") == 0) {
            cout << "Exiting...\n";
            run=false;
            exit(0);
        } else if (strcmp(command, "history") == 0) {
            cout << "Command history:\n";
            int count = commandIndex-1;
            int i;
            hist=true;
            
            if(count>10)
            {
             i=10;
            
            }
            else
            i=count+1;
            for (; i >= 0&& count>=0; i--) {
                cout << i << ". " << commands[count] << endl;
                count--;
            }
           continue;
        } else if (strcmp(command, "!!") == 0) {
            if (commandIndex == 0) {
                cout << "No previous commands in history.\n";
            } else {
                strcpy(command, commands[(commandIndex - 1) ]);
                cout << "Executing previous command: " << command << endl;
                 strcpy(commands[commandIndex], command);
                commandIndex++;
            }
        } else if (command[0] == '!') {
            int index = atoi(command + 1);
            if (index <= 0 || index >commandIndex) {
                cout << "Invalid command index.\n";
            } else {
                strcpy(command, commands[(index - 1)]);
                cout << "Executing command " << index << ": " << command << endl;
                 strcpy(commands[commandIndex], command);
                 commandIndex++;
            }
        } else {
            // store the command in history
            
            strcpy(commands[commandIndex], command);
           
            commandIndex++;
            // execute the command
            
        }
        if(fork()==0)
	runCommand(command);
	else 
	wait(NULL);
        
    }
    return 0;
}

