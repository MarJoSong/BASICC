#include <io_utils.h>
#include <time_utils.h>
#include <tinycthread.h>
#include <stdatomic.h>

//定义线程存储期整形变量，在每个线程开始时初始化，线程结束时销毁，各线程(包括主线程)之间互不干涉 (C11)
_Thread_local int count = 0;

int Counter(int *step) {
  for (int i = 0; i < 1000000; ++i) {
	count += *step;
  }
  PRINT_INT(count);
  return 0;
}

int main(void) {
  thrd_t t_1, t_2;
  int step_1 = 1, step_2 = 2;
  thrd_create(&t_1, Counter, &step_1);
  thrd_create(&t_2, Counter, &step_2);

  thrd_join(t_1, NULL);
  thrd_join(t_2, NULL);

  PRINT_INT(count);
}
