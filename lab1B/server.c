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
#include <sys/socket.h>
#include <netinet/in.h>
#include <mcrypt.h>
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
int ppp=0;
pid_t child_ID;
int sockfd, newsockfd, portno, clilen;
struct sockaddr_in serv_addr, cli_addr;
 char *key;
  //char password[20];
  char *IV;
  int keysize=16; /* 128 bits */
    MCRYPT td;
    int ccc=0;


void mysigpipe() //SIGPIPE HANDLE
{
    
        close(newsockfd);
   tcsetattr( STDIN_FILENO, TCSANOW, &oldt );//Restores terminal modes on program exit
   _exit(2); 
}

void *read_from ()//thread for read from the shell pipe
{
       char buffer1[256];
       int count;
	while (1)
	{
    // bzero(buffer1,256);
   memset(buffer1,0,256);
     //count = fread (&buffer1, 255, 255, (FILE *)from_c_pipe[0]) ;
       count = read(from_c_pipe[0],buffer1,255);
       if (count<=0)
       {
        //
        close(newsockfd);
      //  close(sockfd);
      tcsetattr( STDIN_FILENO, TCSANOW, &oldt );//Restores terminal modes on program exit
      _exit(1);
	      }
          if (ccc==1)
          {
         mcrypt_generic(td, buffer1, count);
        }
       // fwrite ( &buffer1, count, count, stdout);
         write(1,buffer1,count);
	}
}

int main (int argc, char **argv)
{ 
   
      char buffer;
      int opt= 0;
     int n;
      static struct option long_options[] = {    
        {"port",    required_argument, NULL,  'p' },
        {"encrypt", no_argument,  NULL,  'e' },
        {0,           0,                 0,  0   }
    };
		
		 int long_index =0;
		  while ((opt = getopt_long(argc, argv,"", 
		  long_options, &long_index )) != -1) 
		  {
			switch (opt) {
       case 'p' : 
       ppp=1;
       portno= atoi(optarg);
                 break;
			 case 'e' :
         ccc=1;
        break;
         	 default:
				   fprintf(stdout,"'%s' has no argument.\n","lab1");
					 _exit(0);     
			}
		}
   if (ppp==0)
   _exit(0);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        printf("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              printf("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
          printf("ERROR on accept");
     //old code
        dup2(newsockfd,0);
       dup2(newsockfd,1);
       dup2(newsockfd,2);
       close(newsockfd);
     key=calloc(1, keysize);
    /* char password[20];
  strcpy(password, "A_large_key");*/
  char *password = NULL;
  
FILE *fp = fopen("my.key", "r");
if (fp != NULL) {
 
    if (fseek(fp, 0L, SEEK_END) == 0) {
         long bufsize = ftell(fp);
        if (bufsize == -1) {  }

     
        password = malloc(sizeof(char) * (bufsize + 1));

       
        if (fseek(fp, 0L, SEEK_SET) != 0) {  }

    
        size_t newLen = fread(password, sizeof(char), bufsize, fp);
        if (newLen == 0) {
            fputs("Error reading file", stderr);
        } else {
            password[newLen++] = '\0'; 
        }
    }
    fclose(fp);
}
   
    memmove( key, password, strlen(password));
  td = mcrypt_module_open("twofish", NULL, "cfb", NULL);
  if (td==MCRYPT_FAILED) {
     return 1;
  }
   IV = malloc(mcrypt_enc_get_iv_size(td));
   for (i=0; i< mcrypt_enc_get_iv_size( td); i++) {
    IV[i]=rand();
  }
  i=mcrypt_generic_init( td, key, keysize, IV);
  if (i<0) {
     mcrypt_perror(i);
     return 1;
  }
  free(password);
  //###############################
    
     //atexit (reset_input_mode);  
		 tcgetattr(STDIN_FILENO, &oldt); //store old settings 
		/*  newt = oldt; // copy old settings to new settings 
		newt.c_lflag &= ~(ICANON ); // make  changes 
		 newt.c_cc[VMIN] = 1;
		newt.c_cc[VTIME] = 0;
		tcsetattr(STDIN_FILENO, TCSANOW, &newt); //apply the new settings */ 
	//	signal(SIGINT, intHandler);
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
			  pthread_t to_child_thread;
			pthread_create(&to_child_thread,NULL,read_from,NULL);
       
			 while (1)
		   {
    
     n = read(0,&buffer,1);
       if (n<=0)
       {
       // 
        close(newsockfd);
        //close(sockfd);
      tcsetattr( STDIN_FILENO, TCSANOW, &oldt );//Restores terminal modes on program exit
      _exit(1);
	      }
     if (ccc==1)
     {
      mdecrypt_generic (td, &buffer, 1); 
     }
    
	/*	if (buffer== 0x0D || buffer== 0x0A)
		{
				 write(to_c_pipe[1],linefeed,1);
				 //write(1,cr_lf,2);
		}*/
		
	//	else
		//{
		write(to_c_pipe[1],&buffer,1);
		//}
			   
		  
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
       dup2(from_c_pipe[1],2);
      
       close(newsockfd);
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
		
		 
		 _exit(0);
    } 
