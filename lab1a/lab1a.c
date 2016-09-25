#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <termios.h> 
#include <string.h> 
#include <getopt.h>
#include <signal.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#define B_size 1
#define B_size2 128
char cr_lf[2] = {0x0D, 0x0A};//
char linefeed[1]={0x0A};//line feed only
int to_c_pipe[2];// to shell pipe
int from_c_pipe[2];// from shell pipe
struct termios oldt;// termios old and new 
struct termios newt;
int i, check=0;// check value loop value
pid_t cc =-1; //gobal pid_T
int check2=0;
pid_t child_ID;
void reset_input_mode(void)
{
 printf("atexit mode!\n");
tcsetattr( STDIN_FILENO, TCSANOW, &oldt );//Restores terminal modes on program exit
if (check)
{
 int status;
 waitpid(-1,&status,0);
 printf("exit code:%d \n",WEXITSTATUS(status));
}
}

void mysigpipe() //SIGPIPE HANDLE
{
   printf("I have received a sigpipe\n");
   int child_state;
   int num2=1;
   tcsetattr( STDIN_FILENO, TCSANOW, &oldt );//Restores terminal modes on program exit
   printf("exit status(RC): %d\n",num2);
   _exit(num2);
    
  
}
void exit_reset () //for atexit (didn't use)
{
tcsetattr( STDIN_FILENO, TCSANOW, &oldt );//Restores terminal modes on program exit
  printf("end! reset model\n");
}

void  intHandler(int sig) //SIGNAL handler
{
  // check2=1;
     fprintf(stdout,"'%s' catch the  Ctrl-C.(EOF COME BEOFRE SIGPIPE HERE BUT MY SIGPIPE IS WORK)\n","lab1");
}
void mysighup() { // sighup handler
   printf("I have received a SIGHUP\n");
    
}
void *read_from ()//thread for read from the shell pipe
{
  char buffer1[2048];
    int count=0;
    int num1=1;
    int status;
	while (1)
	{
       count = read(from_c_pipe[0],buffer1,sizeof(buffer1));
       if (count==0&&check2==0)
       {
	 child_ID =waitpid(cc,&status,0);
	 if (child_ID==cc)
	   {
	     printf("status = %d\n",status);
	     if(WIFEXITED(status))
	       printf("WIFEXITED is set:%d\n",WEXITSTATUS(status));
	     if (WIFSIGNALED(status))
	       printf("SINGALED is set:%d\n",WTERMSIG(status));
	   }
	 printf("Shell EOF\n"); //printf \\004  to make the EOF
	 printf("exit status(RC): %d\n",num1);
	 tcsetattr( STDIN_FILENO, TCSANOW, &oldt );//Restores terminal modes on program exit
	 _exit(num1);
       }
   for (i=0;i<count;i++)
   {
  
     /*  if (*(buffer1+i)=='\004') // check the eof from shell
   {
    printf("Shell EOF\n"); //printf \\004  to make the EOF
    printf("exit status(RC): %d\n",num1);
     tcsetattr( STDIN_FILENO, TCSANOW, &oldt );//Restores terminal modes on program exit
      _exit(num1);
   }
   else */
   write(1,buffer1+i,1);
   //}
   } 
	}
}

int main (int argc, char **argv)
{ 
atexit (reset_input_mode);
		 int opt= 0;    
		 tcgetattr(STDIN_FILENO, &oldt); //store old settings 
     		 
		static struct option long_options[] = 
		{    
		  { "shell",     no_argument,       NULL,     's'   },
		{0,           0,                 0,  0   }
		};
		
		 int long_index =0;
		  while ((opt = getopt_long(argc, argv,"", 
		  long_options, &long_index )) != -1) 
		  {
			switch (opt) {
				 case 's' :
				 check=1;
				break;
				 default: 
				   fprintf(stdout,"'%s' has no argument.\n","lab1");
					 _exit(0);
			}
		}
    
  if (check==0)
  {
    
		newt = oldt; // copy old settings to new settings 
		newt.c_lflag &= ~(ICANON | ECHO); // make  changes 
		 newt.c_cc[VMIN] = 1;
		newt.c_cc[VTIME] = 0;
		tcsetattr(STDIN_FILENO, TCSANOW, &newt); //apply the new settings  
     
     
        char buffer;
    while (1)
       {
			read(0,&buffer,sizeof(char));
		if (buffer== 0x0D || buffer== 0x0A)
		{
				write(1, cr_lf, 2);
		}
		 else if (buffer==4)
		{
			 write(1, &buffer, sizeof(char));
		 tcsetattr( STDIN_FILENO, TCSANOW, &oldt );//Restores terminal modes on program exit
		break;
		}
		else
		{
		 write(1, &buffer, sizeof(char));
		}
	}
		
		 tcsetattr( STDIN_FILENO, TCSANOW, &oldt );//Restores terminal modes on program exit
		}


		else
		{
		  newt = oldt; // copy old settings to new settings 
		newt.c_lflag &= ~(ICANON | ECHO); // make  changes 
		 newt.c_cc[VMIN] = 1;
		newt.c_cc[VTIME] = 0;
		tcsetattr(STDIN_FILENO, TCSANOW, &newt); //apply the new settings  
		signal(SIGINT, intHandler);
		 signal(SIGPIPE,mysigpipe);
		  if (pipe(to_c_pipe) ==-1)
		  {
		   fprintf(stderr, "pipe failed!/n");
		   exit(1);
		  }
		if (pipe(from_c_pipe) ==-1)
		  {
		   fprintf(stderr, "pipe failed!/n");
		   exit(1);
		  }
		  signal(SIGPIPE ,mysigpipe);
		  cc=fork();
  
		  if (cc>0) //parent process
		  {
			close(to_c_pipe[0]); //close to child pipe(read)
			close(from_c_pipe[1]);//close from child pipe(write)
			char buffer2;
      
			  pthread_t to_child_thread;
			pthread_create(&to_child_thread,NULL,read_from,NULL);
			 while (1)
		   {
		   
			read(0,&buffer2,1);
		if (buffer2== 0x0D || buffer2== 0x0A)
		{
				 write(to_c_pipe[1],linefeed,1);
				 write(1,cr_lf,2);
		}
		 else if (buffer2=='\004')
		{
                int num0=0;
                 int  child_state;
		 check2=1;
   			kill(cc,SIGHUP);
    close(to_c_pipe[1]);
	   close(from_c_pipe[0]);
		 child_ID =waitpid(cc,&child_state,0);
		if (child_ID==cc)
		{
		 printf("get EOF from ^D\n");
		 printf(" status = %d\n",child_state);
		 if(WIFEXITED(child_state))
		 printf("WIFEXITED is set:%d\n",WEXITSTATUS(child_state));
		 if (WIFSIGNALED(child_state))
		 printf("SINGALED is set:%d\n",WTERMSIG(child_state));
       tcsetattr( STDIN_FILENO, TCSANOW, &oldt );//Restores terminal modes on program exit
        printf("exit status(RC): %d\n",num0);
   	  _exit(num0);
		}
   }
   else if (buffer2=='\003')
		{
      fprintf(stdout,"'%s' catch the  Ctrl-C.(EOF COME BEOFRE SIGPIPE HERE BUT MY SIGPIPE IS WORK)\n","lab1");
	    	kill(cc,SIGINT);
		}
		else
		{
		write(to_c_pipe[1],&buffer2,sizeof(char));
		write(1,&buffer2,sizeof(char));
		}
			   
		}  
			pthread_join(to_child_thread,NULL);
			_exit(0);
	}
		  
		  else if (cc==0)//child process
		  {
		 
		   close(to_c_pipe[1]);
		   close(from_c_pipe[0]);
		   dup2(to_c_pipe[0],0);
		   dup2(from_c_pipe[1],1);
		   close(to_c_pipe[0]);
		   close(from_c_pipe[1]);
		   char *exec_argv[2] ={"/bin/bash",NULL};
		  
		   if (execvp(exec_argv[0],exec_argv)==-1)
		   {
		   fprintf(stderr,"execvp() failed");
		   exit(1);
		   }
		  
		   }
		   else//cc<0 fork() failed
		  {
			fprintf(stderr,"fork failed");
		   exit(1);
		  }
		}
		 
		 _exit(0);
    } 
