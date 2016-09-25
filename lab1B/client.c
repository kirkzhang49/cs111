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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <fcntl.h>
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
 int sockfd, portno, n;
 int ccc=0;
int file;
  MCRYPT td;
      int lll=0;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer;
    char buffer1[256];
     char *key;

  char *IV;
  int keysize=16; /* 128 bits */
    void error(char *msg)
{
    perror(msg);
    exit(0);
}

/*void reset_input_mode(void)
{
 printf("atexit mode!\n");
tcsetattr( STDIN_FILENO, TCSANOW, &oldt );//Restores terminal modes on program exit
if (check)
{
 int status;
 waitpid(-1,&status,0);
 printf("exit code:%d \n",WEXITSTATUS(status));
}
}*/

void mysigpipe() //SIGPIPE HANDLE
{
    
        close(sockfd);
   tcsetattr( STDIN_FILENO, TCSANOW, &oldt );//Restores terminal modes on program exit
   _exit(2); 
}

void *input_ff()
{
  char buffer2[256];
while (1)
{
  // bzero(buffer2,256);
    memset(buffer2,0,256);
    n = read(sockfd,buffer2,255);
      if (n <= 0)
      { 
       close(sockfd);
      tcsetattr( STDIN_FILENO, TCSANOW, &oldt );//Restores terminal modes on program exit
      _exit(1);
      }
    if (lll==1)
    {
      dprintf(file,"RECEIVED %d bytes: %s\n",n,buffer2);
    }
     if (ccc==1)
     {
     mdecrypt_generic (td, buffer2, n); 
    }
  
    printf("%s",buffer2);
    
    }
}


int main (int argc, char **argv)
{ 
      
      int opt= 0;
      static struct option long_options[] = {    
        {"port",    required_argument, NULL,  'p' },
        {"log",   required_argument, NULL,  'l' },
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
				 case 'l' :
          lll=1;
          file= open(optarg, O_RDWR|O_CREAT, 0644);
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
       signal(SIGPIPE ,mysigpipe);
   //###############################
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        printf("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        printf("ERROR connecting");
    //old code
    // atexit (reset_input_mode);  
		 tcgetattr(STDIN_FILENO, &oldt); //store old settings 
		  newt = oldt; // copy old settings to new settings 
		newt.c_lflag &= ~(ICANON |ECHO); // make  changes 
		 newt.c_cc[VMIN] = 1;
		newt.c_cc[VTIME] = 0;
		tcsetattr(STDIN_FILENO, TCSANOW, &newt); //apply the new settings
	//	signal(SIGINT, intHandler);
		// signal(SIGPIPE,mysigpipe);
    /* char password[20];
  strcpy(password, "A_large_key");*/
   key=calloc(1, keysize);
   
  char *password = NULL;
FILE *fp = fopen("my.key", "r");
if (fp != NULL) {
    /* Go to the end of the file. */
    if (fseek(fp, 0L, SEEK_END) == 0) {
        /* Get the size of the file. */
        long bufsize = ftell(fp);
        if (bufsize == -1) { /* Error */ }

        /* Allocate our buffer to that size. */
        password = malloc(sizeof(char) * (bufsize + 1));

        /* Go back to the start of the file. */
        if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */ }

        /* Read the entire file into memory. */
        size_t newLen = fread(password, sizeof(char), bufsize, fp);
        if (newLen == 0) {
            fputs("Error reading file", stderr);
        } else {
            password[newLen++] = '\0'; /* Just to be safe. */
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
  
       pthread_t input_thread;
			pthread_create(&input_thread,NULL,input_ff,NULL);
      while (1)
      {
  int xxx= read(0,&buffer,1);
  if (xxx<=0)
  {
     close(sockfd);
      tcsetattr( STDIN_FILENO, TCSANOW, &oldt );//Restores terminal modes on program exit
      _exit(1);
  }
    if (buffer=='\004')
    {
    close(sockfd);
      tcsetattr( STDIN_FILENO, TCSANOW, &oldt );//Restores terminal modes on program exit
      _exit(0);
    }
   else
   {
   write(0,&buffer,1);
     if (ccc==1)
     {
      mcrypt_generic (td, &buffer, 1);
     }
     n=write(sockfd,&buffer,1);

    if (lll==1)
    {
      dprintf(file,"SENT %d bytes: %c\n",n,buffer);
    }
    }
	}
 
      	pthread_join(input_thread,NULL);
      
     // mcrypt_generic_end(td);
       	_exit(0);
   
}
