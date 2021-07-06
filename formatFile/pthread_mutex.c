#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>         //创建两个线程，分别对两个全变量进行++操作，判断两个变量是否相等，不相等打印

int a = 0;
int b = 0;
// 未初始化 和0初始化的成员放在bbs
pthread_mutex_t mtx;
static int c_int = 0;

void call_bacK(void *arg) {
  printf("call back\n");
  pthread_mutex_unlock(&mtx);
}

void *route1() {
  while (1)            //初衷不会打印
  {
    printf("route 1 ready to lock\n");
    pthread_cleanup_push(call_bacK, NULL) ;
        pthread_mutex_lock(&mtx);
        printf("route 1 lock succ\n");
        a++;
        b++;
        sleep(1);
        printf("route 1 ready to unlock\n");
        pthread_mutex_unlock(&mtx);
    pthread_cleanup_pop(c_int);
    printf("route 1 unlock success\n");
  }
}

void *route2() {
  while (1)            //初衷不会打印
  {
    printf("route 2 ready to lock\n");
    pthread_mutex_lock(&mtx);
    printf("route 2 lock succ\n");
    printf("a =%d, b = %d\n", a, b);
    printf("route 2 ready to unlock\n");
    pthread_mutex_unlock(&mtx);
    printf("route 2 unlock success\n");
    sleep(1);
  }
}

int main() {
  pthread_t tid1, tid2;
  pthread_mutex_init(&mtx, NULL);

  pthread_create(&tid1, NULL, route1, NULL);
  pthread_create(&tid2, NULL, route2, NULL);

  sleep(7);
  pthread_cancel(tid1);
  printf("route 1 dead\n");

  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);

  pthread_mutex_destroy(&mtx);
  return 0;
}