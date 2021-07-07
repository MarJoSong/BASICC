#include <io_utils.h>
#include <time_utils.h>
#include <tinycthread.h>
#include <stdatomic.h>

//原子类型
atomic_int count = 0;

int Counter(void *arg) {
  PRINTLNF("Counter.");
  for (int i = 0; i < 1000000; ++i) {
	count++;

	// int temp = count, count = temp + 1, temp;
  }
  return 0;
}

int main(void) {
  TimeCost(NULL);
  thrd_t t_1, t_2;
  thrd_create(&t_1, Counter, NULL);
  thrd_create(&t_2, Counter, NULL);

  thrd_join(t_1, NULL);
  thrd_join(t_2, NULL);

  PRINT_INT(count);
  TimeCost("atomic");

  //atomic_store(PTR, VAL) 不限制类型赋值，PTR为地址
  //atomic_load(PTR) 不限制类型取值
  //atomic_exchange(PTR, VAL) 不限制类型赋值VAL，返回旧的值
  atomic_is_lock_free(&count); //判断原子类型实现 True:通过CPU原子指令实现; False:通过加锁实现
}
