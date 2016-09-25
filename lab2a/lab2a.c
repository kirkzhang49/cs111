#define _GNU_SOURCE
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <pthread.h>
#include <getopt.h>
#include <time.h>
#include <stdint.h>

volatile int exclusion = 0;

int iterations_n=1;

int threads_n=1;

long long count=0;

pthread_mutex_t lock;

int which_lock=0; 

int opt_yield=0;

volatile int locked=0;

void add(long long *pointer, long long value) {
 	   
         long long sum;
        
	       	sum = *pointer + value;
                         
		      if (opt_yield)
		     	pthread_yield();
                          
		      *pointer = sum;
}

void*  exefunc()
 {
  
   long long pre;
   long long sum;
    int x;
 
    for (x=0;x<iterations_n;x++)
    {
    switch(which_lock)
    {
    
    case 1:
    {
      pthread_mutex_lock(&lock);
      add(&count,1);
      pthread_mutex_unlock(&lock);
      break;
    }
    
    case 2:
    {
     while (__sync_lock_test_and_set(&exclusion, 1)) ;
      add(&count,1);
      __sync_lock_release(&exclusion);
      break;
    }
    
   case 3:
    {
      do
      {
         pre=count;
        
         sum = pre+1;
        
		     if (opt_yield)
		    	pthread_yield();
                        
       }while((__sync_val_compare_and_swap(&count, pre, sum))!=pre);
      break;
    }
    default:
      {
        add(&count,1);
        break;
      }
     }
    }
   for (x=0;x<iterations_n;x++)
    {
    switch(which_lock)
    {
    
    case 1:
    {
      pthread_mutex_lock(&lock);
      add(&count,-1);
      pthread_mutex_unlock(&lock);
      break;
    }
    
    case 2:
    {
     while (__sync_lock_test_and_set(&exclusion, -1)) ;
      add(&count,-1);
      __sync_lock_release(&exclusion);
      break;
    }
    
   case 3:
    {
    
       do
      {
         pre=count;
        
         sum = pre-1;
        
		     if (opt_yield)
		    	pthread_yield();
                        
       }while((__sync_val_compare_and_swap(&count, pre, sum))!=pre);
      break;
    }
    default:
      {
        add(&count,1);
        break;
      }
     }
    }
 }

int main (int argc, char **argv)
{ 
      
      int opt= 0;
      struct  timespec time1, time2,temp;
      
      static struct option long_options[] = {    
        {"iterations",    required_argument, NULL,  'i' },
        {"threads",   required_argument, NULL,  't' },
        {"yield",   no_argument, NULL,  'y' },
         {"sync",   required_argument, NULL,  's' },
        {0,           0,                 0,  0   }
    };
		 
      char *temp1;
		  int long_index =0;
        
		  while ((opt = getopt_long(argc, argv,"", 
		  long_options, &long_index )) != -1) 
		  {
			switch (opt) {
       
       case 'i' : 
       if((iterations_n= atoi(optarg)) ==0)
       {
         fprintf(stderr,"./lab2a: require an argument\n");
         _exit(1);
       }
       break;
       
		   case 't' :
           if((threads_n= atoi(optarg)) ==0)
       {
         fprintf(stderr,"./lab2a: require an argument\n");
         _exit(1);
       }
		 	 break;
                                  
       case 'y' :
          opt_yield=1;
  	   break;
                
      case 's' :
         temp1=optarg;
        if(*temp1=='m')
          which_lock=1;
        else if(*temp1=='s')
         which_lock=2;
        else if(*temp1=='c')
          which_lock=3;
        else
            fprintf(stderr,"'the optarg for sync is not correct!\n");
           
				break;
         	 default:
				   fprintf(stderr,"'%s'arg or opt not right .\n","lab2a");
					 _exit(0);            
			}
		}
   
     int total=2*iterations_n*threads_n;
     
     int i;
     
     pthread_t threads[threads_n];
     
     clock_gettime(CLOCK_MONOTONIC, &time1);
     
     for(i=0;i<threads_n;i++){
        pthread_create(&threads[i], NULL,exefunc,NULL);
      }
         
       for(i=0;i<threads_n;i++){
         pthread_join(threads[i],NULL); 
      }
      
    clock_gettime(CLOCK_MONOTONIC, &time2);
    
     if (which_lock ==1)
     pthread_mutex_destroy(&lock);
    
    fprintf(stdout,"%d threads x %d iterations x (add + subtract) = %d operations\n",threads_n,iterations_n,total);
   
    unsigned long long temptime=(time2.tv_sec-time1.tv_sec)*1000000000;
    
    temptime+=time2.tv_nsec;
    
    temptime-=time1.tv_nsec;
  
   fprintf(stdout,"elapsed time: %dns\n", temptime);
   
   fprintf(stdout,"per operation: %dns\n", temptime/total);
    
    if (count !=0)
    {
        fprintf(stderr,"ERROR: final count = %d\n",count);
        exit(1); 
    }
      exit(0); 
}