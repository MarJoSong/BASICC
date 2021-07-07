#include <io_utils.h>
#include <time_utils.h>
#include <tinycthread.h>
#include <stdatomic.h>

int count = 0;
mtx_t mutex;

int Counter(void *arg) {
  PRINTLNF("Counter.");
  for (int i = 0; i < 1000000; ++i) {
	mtx_lock(&mutex);
	count++;
	mtx_unlock(&mutex);
  }
  return 0;
}

int main(void) {
  mtx_init(&mutex, mtx_plain);
  /* 锁的类型
   * mtx_plain     普通锁
   * mtx_timed     超时锁
   * mtx_recursive 可重入锁(递归调用导致的重复加锁)
   *
   */

  TimeCost(NULL);
  thrd_t t_1, t_2;
  thrd_create(&t_1, Counter, NULL);
  thrd_create(&t_2, Counter, NULL);

  thrd_join(t_1, NULL);
  thrd_join(t_2, NULL);

  PRINT_INT(count);
  TimeCost("atomic");
  mtx_destroy(&mutex);
}
