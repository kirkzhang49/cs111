#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <math.h>


#include <pthread.h>
#include <time.h>
#include "SortedList.c"

volatile int exclusion = 0;

int iterations_n=1;

int threads_n=1;

char** keys;

long long count=0;

pthread_mutex_t lock;

int spin_lock=0 ,mutex_lock=0; 

int opt_yield=0;

volatile int locked=0;

SortedList_t LIST;

SortedListElement_t* TOTALELE;

void mylock()
{
  if (mutex_lock)
    {
      pthread_mutex_lock(&lock);
    }
  else if (spin_lock)
    {
      while(__sync_lock_test_and_set(&exclusion,1));
    }
  else
    {
      return;
    }
}  

void myunlock()
{
  if (mutex_lock)
    { 
      pthread_mutex_unlock(&lock);
    }
  else if (spin_lock)
    {
      __sync_lock_release(&exclusion);
    }
  else
    {
      return;
    }
} 

void*  exefunc(void* arg)
{
  int i;
  int num = *((int*) arg);
  int length;
     

  for(int i =0; i < iterations_n; i++)
    {
      mylock();
      // printf("xxxx");
      SortedList_insert(&LIST, &TOTALELE[num*(iterations_n) + i]);
      // printf("yyy");
      myunlock();
    }
  mylock();
  length = SortedList_length(&LIST);
  myunlock();
  SortedListElement_t* temp;
     
  

  for(int i =0; i < iterations_n; i++)
    {
      mylock();
      temp = SortedList_lookup(&LIST, TOTALELE[num*(iterations_n) + i].key);
      SortedList_delete(temp);
      myunlock();
    }
}


int main (int argc, char **argv)
{ 
  int i;
  int opt= 0;
  struct  timespec time1, time2,temp;
      
  static struct option long_options[] = {    
    {"iterations",    required_argument, NULL,  'i' },
    {"threads",   required_argument, NULL,  't' },
    {"sync",   required_argument, NULL,  's' },
    {"yield",   required_argument, NULL,  'y' },
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
              
      case 's' :  
	if(optarg != NULL){
	  switch((int)optarg[0]){
	  case 'm':
	    mutex_lock = 1;
	    break;
	  case 's':
	    spin_lock = 1;
	    break;
	  }
	}
	break;                        
      case 'y' :
	if (optarg == NULL)
	  {
	    break;
	  }
	else
	  {
	    for (i = 0; i < strlen(optarg); i++)
	      {
		if (optarg[i] == 'i')
		  opt_yield |= INSERT_YIELD;
		else if (optarg[i] == 'd')
		  opt_yield |= DELETE_YIELD;
		else if (optarg[i] == 's')
		  opt_yield |= SEARCH_YIELD;
		else
		  {
		    fprintf(stderr, "ERROR: wrong yield option!\n");
		    exit(1);
		  }
	      }
	    break;
	  }      
     
           
	break;
      default:
	fprintf(stderr,"'%s'arg or opt not right .\n","lab2a");
	_exit(0);            
      }
    }
  int total=iterations_n*threads_n;
  TOTALELE = malloc(total*sizeof(SortedListElement_t));
  if (TOTALELE == NULL) {
    fprintf(stderr, "ERROR: can't to allocate memory\n");
    exit(1);
  }
  keys = malloc(total*sizeof(char *));
  if (keys == NULL) {
    fprintf(stderr, "ERROR: can't to allocate memory\n");
    exit(1);
  }
  LIST.prev = &LIST;
  LIST.next = &LIST;
  LIST.key = NULL;
  //int lengths[niter*nthreads];
  for(i =0; i < total; i++)
    {
      int length = rand()%51+1;
      keys[i] = malloc(length*sizeof(char));
      if (keys[i] == NULL) {
	fprintf(stderr, "ERROR: unable to allocate memory\n");
	exit(1);
      }
      for( int j = 0; j < length-1; ++j)
	keys[i][j] = '0' + rand()%51;
      
      keys[i][length-1] = '\0';

      TOTALELE[i].key = keys[i];
    }
    
  for(i = 0 ; i < total; i++)
    {
      TOTALELE[i].key = keys[i];
      TOTALELE[i].next = NULL;
      TOTALELE[i].prev = NULL;
    }
    
  //create threads
  pthread_t * threads = malloc(sizeof(pthread_t)* threads_n);
        
  clock_gettime(CLOCK_MONOTONIC, &time1);
     
  for(i = 0; i < threads_n; i++) {
    int *arg = malloc(sizeof(*arg));
    *arg = i;
    pthread_create(&threads[i], NULL, exefunc, (void *) arg);  
  }
  for(i=0;i<threads_n;i++){
    pthread_join(threads[i],NULL); 
  }
      
  clock_gettime(CLOCK_MONOTONIC, &time2);
    
  if (mutex_lock ==1)
    pthread_mutex_destroy(&lock);
  int TOTALOP=2*total;
  fprintf(stdout,"%d threads x %d iterations x (insert + lookup/delete) = %d operations\n",threads_n,iterations_n,TOTALOP);
   
  unsigned long long int temptime=(time2.tv_sec-time1.tv_sec)*1000000000;
    
  temptime+=time2.tv_nsec;
    
  temptime-=time1.tv_nsec;
  
  fprintf(stdout,"elapsed time: %lldns\n", temptime);
   
  fprintf(stdout,"per operation: %lldns\n",  temptime/TOTALOP);
    
  int finalLength = SortedList_length(&LIST);
  if( finalLength != 0) {
    fprintf(stderr, "Error! Final list length is not zero!\n");
    exit(1);
  }
  free(threads);
  for(i=0;i<total;i++)
    {
  free(keys[i]);
    }
  exit(0); 
}
