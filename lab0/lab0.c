#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>

#define MAX_SIZE 4096

static int segfault_flag = 0;

static void sigsegv_handler(int signo)
{
  fprintf(stderr, "SIGSEGV signal!\n");
  exit(3);
}

int main (int argc, char **argv)
{
  int c;
  static struct option long_options[] = {
    {"segfault", no_argument,       &segfault_flag, 's'},
    {"catch",    no_argument,       0,              'c'},
    {"input",    required_argument, 0,              'i'},
    {"output",   required_argument, 0,              'o'}
  };
  int ifd = 0;
  int ofd = 1;

  while (1) {
    int option_index = 0;

    c = getopt_long(argc, argv, "sci:o:", long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {
      case 'c':
        signal(SIGSEGV, sigsegv_handler);
        break;

      case 'i':
        ifd = open(optarg, O_RDONLY);
        if (ifd >= 0) {
          close(0);
          dup(ifd);
          close(ifd);
        } else {
          fprintf(stderr, "Unable to open the file: %s\n", optarg);
          perror("Unable to open the specified input file");
          exit(1);
        }

        break;

      case 'o':
        ofd = creat(optarg, 0666);
        if (ofd >= 0) {
          close(1);
          dup(ofd);
          close(ofd);
        } else {
          fprintf(stderr, "Unable to create the file: %s\n", optarg);
          perror("Unable to create the specified output file");
          exit(2);
        }

        break;
    }
  }

  if (segfault_flag) {
    char *segfault_char = NULL;
    *segfault_char = 3;
  }
  
  int read_size = 0;
  char buf[MAX_SIZE] = {};
  

  while ((read_size = read(0, buf, MAX_SIZE)) > 0) {
    write(1, buf, read_size);
  }

  return 0;
}
