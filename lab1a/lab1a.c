#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <termios.h> 
#include <string.h> 
#include <getopt.h>
#include <signal.h> 
#include <sys/types.h>
#include <sys/wait.h>

int getch(void) //Dynamic String Input function from Daniweb.com
{  
    int ch; 
    struct termios oldt; 
    struct termios newt; 
    tcgetattr(STDIN_FILENO, &oldt); //store old settings 
    newt = oldt; // copy old settings to new settings 
    newt.c_lflag &= ~(ICANON | ECHO); // make one change to old settings in new settings 
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); //apply the new settings immediatly 
    ch = getchar(); // standard getchar call 
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // reapply the old settings 
    return ch; // return received char  
} 
void changet()
{
  setvbuf(stdout,NULL,_IONBF,0);

  struct termios old_termios, new_termios;
  tcgetattr(0,&old_termios);
   new_termios             = old_termios;
  new_termios.c_cc[VEOF]  = 3; // ^C
  new_termios.c_cc[VINTR] = 4; // ^D
  tcsetattr(0,TCSANOW,&new_termios);
  // tcsetattr(0,TCSANOW,&old_termios);
}

int main (int argc, char **argv)
{ 
    char x; 
    int *temp, *temp2;
     int opt= 0;
    char outputTemp[] = {'H','W',' ','3',' ','i','n','p','u','t',':',' '};
    char *output[]={};
    x = ' ';
    int i, check=0; 
   changet();
    int arraySize = 0; 
    temp=malloc(sizeof(char)); 
    if (temp == NULL) exit(1);  
    temp2=malloc(sizeof(char)); 
    if (temp2 == NULL) exit(1);  
 
 static struct option long_options[] = {    
        { "shell",     no_argument,       NULL,     's'   },
        {0,           0,                 0,  0   }
    };
     int long_index =0;
    while ((opt = getopt_long(argc, argv,"", 
                   long_options, &long_index )) != -1) {
        switch (opt) {
             case 's' :
             check=1;
            // shellcall();
            break;
             default: 
               fprintf(stdout,"'%s' has no argument.\n","lab1");
                 _exit(0);
        }
    }
  
    while (1) 
    {  
        x = getch(); 
        if (x=='\n'||x=='\r')
        {
         printf("%s","\r\n");
        }
        if (check==1)
        {
         pid_t pid;
          pid = fork ();
        execv("/bin/bash", output);
        }
        printf("%c", x); 
        if (arraySize % 2) //if arraySize is odd 
        { 
            temp[arraySize]=x; 
            //printf("%s", &temp[arraySize]); 
            arraySize++;
            //free(temp2); 
            temp2=malloc(arraySize*sizeof(char)); 
            memcpy(temp2, temp, sizeof(temp2)); 
        } 
        else //if arraySize is even 
        { 
            temp2[arraySize]=x; 
            //printf("%s", &temp2[arraySize]); 
            arraySize++;
           // free(temp); 
            temp=malloc(arraySize*sizeof(char)); 
            memcpy(temp, temp2, sizeof(temp)); 
        } 
    } 
    
  /*  printf("Total Characters: %d\n", arraySize-1); 
    if (arraySize % 2) 
    { 
       arraySize++;
       //free(temp); 
       temp=malloc(arraySize*sizeof(char)); 
       memcpy(temp, temp2, sizeof(temp));
       temp[arraySize] = '\0';
       int totalSize = arraySize + 12;
       output=calloc(totalSize, arraySize*sizeof(char));
       memcpy(output, temp, arraySize*sizeof(char));
       memcpy(output, outputTemp, sizeof(12));
       for (i = 0; i < totalSize; i++) {
          printf("%s", output[i]);
       } 
    } 
    else
    { 
       arraySize++;
       //free(temp2); 
       temp2=malloc(arraySize*sizeof(char)); 
       memcpy(temp2, temp, sizeof(temp2));
       temp2[arraySize] = '\0';
       int totalSize = arraySize + 12;
       output = calloc(totalSize,totalSize*sizeof(char));
       memcpy(output, temp2, arraySize*sizeof(char));
       memcpy(output, outputTemp, sizeof(12));
       for (i = 0; i < totalSize; i++) {
          printf("%s", output[i]);
       } 
    } 
    */
}
