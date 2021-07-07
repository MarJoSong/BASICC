#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

int pipefd[2];
pthread_mutex_t wmtx;
pthread_mutex_t rmtx;

void *writeBuffer(void *str) {
  while (1) {
    pthread_mutex_lock(&wmtx);
    write(pipefd[1], str, strlen(str));
    pthread_mutex_unlock(&wmtx);
    sleep(1);
  }
}

int
main(int argc, char *argv[]) {
  pid_t cpid;

  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  pthread_mutex_init(&wmtx, NULL);
  pthread_mutex_init(&rmtx, NULL);

  pthread_t tid1, tid2;
  pthread_create(&tid1, NULL, writeBuffer, "write by parent");

  cpid = fork();
  if (cpid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  if (cpid == 0) {
    char buf[20];

    pthread_create(&tid2, NULL, writeBuffer, "write by Child ");

    while (read(pipefd[0], buf, 15) > 0) {
      pthread_mutex_lock(&rmtx);
      write(STDOUT_FILENO, "Child got:", 10);
      write(STDOUT_FILENO, buf, 15);
      write(STDOUT_FILENO, "\n", 1);
      pthread_mutex_unlock(&rmtx);
    }
    _exit(EXIT_SUCCESS);
  } else { /* Parent writes argv[1] to pipe */
    char buf[20];

    while (read(pipefd[0], buf, 15) > 0) {
      pthread_mutex_lock(&rmtx);
      write(STDOUT_FILENO, "Parent got:", 11);
      write(STDOUT_FILENO, buf, 15);
      write(STDOUT_FILENO, "\n", 1);
      pthread_mutex_unlock(&rmtx);
    }
    wait(NULL); /* Wait for child */
    exit(EXIT_SUCCESS);
  }

  pthread_mutex_destroy(&wmtx);
  pthread_mutex_destroy(&rmtx);
}
