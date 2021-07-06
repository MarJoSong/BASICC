#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>                //创建8个线程，3个写线程，5个读线程

pthread_rwlock_t rwlock;
int counter = 0;

void* readfunc(void* arg)
{
  int id = *(int*)arg;
  free(arg);
  while(1)
  {
    pthread_rwlock_rdlock(&rwlock);
    printf("read thread %d : %d\n", id, counter);
    pthread_rwlock_unlock(&rwlock);
    usleep(100000);
  }
}

void* writefunc(void* arg)
{
  int id = *(int*)arg;
  free(arg);
  while(1)
  {
    int t = counter;
    pthread_rwlock_wrlock(&rwlock);
    printf("write thread %d : t= %d,  %d\n", id, t, ++counter);
    pthread_rwlock_unlock(&rwlock);
    usleep(100000);
  }
}
int main()
{
  pthread_t tid[8];
  pthread_rwlock_init(&rwlock, NULL);
  int i = 0;
  for(i = 0; i < 3; i++)
  {
    int* p =(int*) malloc(sizeof(int));
    *p = i;
    pthread_create(&tid[i], NULL, writefunc, (void*)p);
  }
  for(i = 0; i < 5; i++)
  {
    int* p = (int*)malloc(sizeof(int));
    *p = i;
    pthread_create(&tid[3+i], NULL, readfunc, (void*)p);
  }

  for(i = 0; i < 8; i++)
  {
    pthread_join(tid[i], NULL);
  }

  pthread_rwlock_destroy(&rwlock);

  return 0;
}