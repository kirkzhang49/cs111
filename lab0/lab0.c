#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>

#define B_size 8192
void sighandler (int signum)
{
   fprintf(stderr,"'Process %d got signal %d\n",getpid());
   signal(signum, SIG_DFL);
   kill(getpid(),signum);
}
int main(int argc, char * argv[])
{
     int opt= 0;
    int * s1 =NULL;
    int input = -1, output = -1, segfault  = -1;
    int check=0,check1=0,check2=0,check3=0;
      int fd0, fd1; //name of input output file descriptors
   ssize_t input_n,output_n; //number of bytes return by read and write 
   char buffer[B_size]; //character buffer
    int temp1,temp2;
   // const char * s1 =NULL;
          //   const char * s2 =NULL;
            // bool case1=false,case2=false,case3=false,case4=false;
     static struct option long_options[] = {    
        {"input",    required_argument, NULL,  'i' },
        {"output",   required_argument, NULL,  'o' },
        {"segfault", no_argument,  NULL,  's' },
        {"catch",    no_argument, NULL,  'c' },
        {0,           0,                 0,  0   }
    };
     int long_index =0;
    while ((opt = getopt_long(argc, argv,"ios:c:", 
                   long_options, &long_index )) != -1) {
        switch (opt) {
             case 'i' :check=1;
             fd0 = open(optarg,O_RDONLY);
	   
                       if (fd0 == -1)
                          {
                            fprintf(stderr,"'%s' file can't open\n",optarg);
                            perror ("error:can't open file");
                            _exit(1);
                           }
                           if (fd0 != 0)
	       {
		      close(0);
		       dup(fd0);
	       }
                 break;
             case 'o' : check1=1;
              fd1= open(optarg, O_RDWR|O_CREAT, 0644);
	   
                         if (fd1 == -1)
                              {
                               fprintf(stderr,"'%s' file can't open\n",optarg);
                               perror ("error:can't open file");
                                 _exit(2);
                                     }
   if (fd1!=1)
		{
		      close(1);
		       dup(fd1);
		}
                             if (check ==1)
                             {        
                           while((input_n=read(fd0,&buffer,B_size))>0)
                                        {
                                   output_n = write(fd1,&buffer, (ssize_t) input_n);
                                   if(input_n != output_n){
                                      /* Write error */
                                    perror("writing error!");
                                   
                                   }
                                }
                                }
                 break;
             case 's' : //case3=true;  
	       check2=1;
           
                 break;
             case 'c' : check3=1;
                    
                 break;
             default: 
               fprintf(stdout,"'%s' has no argument.\n","lab1");
                 _exit(0);
        }
    }
       
      
                              /*  else
                                {
                                  fprintf(stdout,"the input file not exist can't copy with only output file.\n");
                                 return (EXIT_SUCCESS);
                                }*/
 // if (case1==false&&case2==false&&case3==false&&case4==false)
 //  {
  //     fprintf(stdout,"'%s' has no argument.\n","lab0");
  //               exit(EXIT_SUCCESS);
  // }
   if (check2==1&&check3!=1)
   {
        int * s1 =NULL;
     *s1='a';
   }
    if (check2==1&&check3==1)
   {
     signal(SIGSEGV,sighandler);
                    fprintf(stderr,"'Process %d waits for someone to send it SIGSEGV\n",getpid());
                         int * s1 =NULL;
                           *s1='a';
                       sleep(1);
                       exit(3);
   }
     if (check2!=1&&check3==1)
   {
     signal(SIGSEGV,sighandler);
                    fprintf(stderr,"'Process %d waits for someone to send it SIGSEGV\n",getpid());
                       sleep(1);
                    //   exit(3);
   } 
  // if(case1==true)
   close (0);
  // if (case2==true)
   close (1);
   
   _exit(0);
   }
