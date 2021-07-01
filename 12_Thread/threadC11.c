#include <io_utils.h>
#include <time_utils.h>
#include <tinycthread.h>

int SayHello(void *word) {
  PRINTLNF("Run in New Thread[%#x], param:[%s]", thrd_current(), (char *)word);
  return 0;
}

int main() {
  thrd_t newthred;
  int ret = thrd_create(&newthred, SayHello, "TinyCThread");
  if (ret == thrd_success) {
    PRINTLNF("Run in Main Thread[%#x], newThread:[%#x]", thrd_current(), newthred);
  } else {
    PRINTLNF("Run in Main Thread[%#x], failed to create newThread", thrd_current());
  }

#if 0
  int res;
  thrd_join(newthred, &res);
  PRINTLNF("New Thread return:[%d]", res);
#else
  thrd_detach(newthred);
  thrd_sleep(&(struct timespec){.tv_sec = 0, .tv_nsec = 1000000000}, NULL);
#endif

  //thrd_yield();放弃CPU时间片

  return 0;
}
