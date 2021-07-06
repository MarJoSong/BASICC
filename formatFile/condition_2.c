#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>             //创建两个线程一个wait print,一个signal sleep()

pthread_cond_t cond;
pthread_mutex_t mutex;

void* f1(void* arg)
{
  while(1)
  {
    pthread_cond_wait(&cond, &mutex);
    printf("running!\n");
  }
}
void* f2(void* arg)
{
  while(1)
  {
    sleep(1);
    pthread_cond_signal(&cond);
  }
}

int main()
{
  pthread_t tid1, tid2;
  pthread_cond_init(&cond, NULL);
  pthread_mutex_init(&mutex, NULL);

  pthread_create(&tid1, NULL, f1, NULL);
  pthread_create(&tid2, NULL, f2, NULL);

  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);

  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&mutex);
  return 0;
}