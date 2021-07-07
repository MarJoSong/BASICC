#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

void sigint_handler(int signo)
{
  printf("recv the signo: %d\n", signo);
}

int main(int argc, char *argv[])
{
  signal(SIGINT, sigint_handler);

  while (1) {
    sleep(1);
  }

  return 0;
}