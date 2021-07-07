#include <io_utils.h>
#include <time_utils.h>
#include <tinycthread.h>
#include <stdatomic.h>

//原子操作标志位
atomic_flag resume_flag = ATOMIC_FLAG_INIT;

int current = 0;

int PrintNumbers(void *arg) {
  int i = 10000;
  //原子操作标志位保证了下面的操作原子性通过无锁来实现
  while (atomic_flag_test_and_set(&resume_flag) && i-- > 0) {
	current++;
	PRINT_INT(current);
  }

  return current;
}

int main(void) {
  atomic_flag_test_and_set(&resume_flag);

  int i = 10000;
  thrd_t t;
  thrd_create(&t, PrintNumbers, NULL);

  while (atomic_flag_test_and_set(&resume_flag) && i-- > 0) {
	current++;
  }
  thrd_sleep(&(struct timespec){.tv_sec=1}, NULL);
  atomic_flag_clear(&resume_flag);

  int last_number;
  thrd_join(t, &last_number);
  PRINT_INT(last_number);
  return 0;
}
