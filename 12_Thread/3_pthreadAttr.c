//
// Created by Mars on 2021/7/1.
//
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

static void
display_pthread_attr(pthread_attr_t *attr, char *prefix) {
  int s, i;
  size_t v;
  void *stkaddr;
  struct sched_param sp;

  s = pthread_attr_getdetachstate(attr, &i);
  if (s != 0)
    handle_error_en(s, "pthread_attr_getdetachstate");
  printf("%sDetach state        = %s\n", prefix,
         (i == PTHREAD_CREATE_DETACHED) ? "PTHREAD_CREATE_DETACHED" :
         (i == PTHREAD_CREATE_JOINABLE) ? "PTHREAD_CREATE_JOINABLE" :
         "???");

  s = pthread_attr_getscope(attr, &i);
  if (s != 0)
    handle_error_en(s, "pthread_attr_getscope");
  printf("%sScope               = %s\n", prefix,
         (i == PTHREAD_SCOPE_SYSTEM) ? "PTHREAD_SCOPE_SYSTEM" :
         (i == PTHREAD_SCOPE_PROCESS) ? "PTHREAD_SCOPE_PROCESS" :
         "???");

  s = pthread_attr_getschedpolicy(attr, &i);
  if (s != 0)
    handle_error_en(s, "pthread_attr_getschedpolicy");
  printf("%sScheduling policy   = %s\n", prefix,
         (i == SCHED_OTHER) ? "SCHED_OTHER" :
         (i == SCHED_FIFO) ? "SCHED_FIFO" :
         (i == SCHED_RR) ? "SCHED_RR" :
         "???");

  s = pthread_attr_getschedparam(attr, &sp);
  if (s != 0)
    handle_error_en(s, "pthread_attr_getschedparam");
  printf("%sScheduling priority = %d\n", prefix, sp.sched_priority);

  s = pthread_attr_getguardsize(attr, &v);
  if (s != 0)
    handle_error_en(s, "pthread_attr_getguardsize");
  printf("%sGuard size          = %lu bytes\n", prefix, v);

  s = pthread_attr_getstack(attr, &stkaddr, &v);
  if (s != 0)
    handle_error_en(s, "pthread_attr_getstack");
  printf("%sStack address       = %p\n", prefix, stkaddr);
  printf("%sStack size          = %#x bytes\n", prefix, v);
}

static void *
thread_start(void *arg) {
  int s;
  pthread_attr_t gattr;

  //资源调度优先级
  s = pthread_setschedprio(pthread_self(), sched_get_priority_min(SCHED_OTHER));
  if (s != 0)
    handle_error_en(s, "pthread_setschedprio");


  /* pthread_getattr_np() is a non-standard GNU extension that
              retrieves the attributes of the thread specified in its
              first argument */

  s = pthread_getattr_np(pthread_self(), &gattr);
  if (s != 0)
    handle_error_en(s, "pthread_getattr_np");

  printf("Thread attributes:\n");
  display_pthread_attr(&gattr, "\t");

  exit(EXIT_SUCCESS);         /* Terminate all threads */
}

int
main(int argc, char *argv[]) {
  pthread_t thr;
  pthread_attr_t attr;
  pthread_attr_t *attrp;      /* NULL or &attr */
  int s;

  attrp = NULL;

  /* If a command-line argument was supplied, use it to set the
     stack-size attribute and set a few other thread attributes,
     and set attrp pointing to thread attributes object */

  if (argc > 1) {
    int stack_size;
    void *sp;

    attrp = &attr;

    s = pthread_attr_init(&attr);
    if (s != 0)
      handle_error_en(s, "pthread_attr_init");

    /*
     * detachstate:
     * PTHREAD_CREATE_JOINABLE (default) 线程创建之后可以调用pthread_join
     * PTHREAD_CREATE_DETACHED 线程创建之后不可以调用pthread_join
     */

    s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (s != 0)
      handle_error_en(s, "pthread_attr_setdetachstate");

    /*
     * inherit scheduler(线程调度属性规则):
     * PTHREAD_INHERIT_SCHED (default) 继承
     * PTHREAD_EXPLICIT_SCHED 指定
     */

    s = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (s != 0)
      handle_error_en(s, "pthread_attr_setinheritsched");

    /*
     * contention scope(资源争用域):
     * POSIX.1-2001 only requires that an implementation support  one  of  these  contention  scopes,
     * but permits both  to  be  supported.
     * Linux  supports PTHREAD_SCOPE_SYSTEM, but not PTHREAD_SCOPE_PROCESS.
     * PTHREAD_SCOPE_SYSTEM (Linux提供此实现)
     * PTHREAD_SCOPE_PROCESS (Linux不提供此实现)
     */

    s = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    if (s != 0)
      handle_error_en(s, "pthread_attr_setscope");

    // 资源调度策略 SCHED_FIFO, SCHED_RR, and SCHED_OTHER
    //pthread_attr_setschedpolicy

    //

    stack_size = strtoul(argv[1], NULL, 0);

    s = posix_memalign(&sp, sysconf(_SC_PAGESIZE), stack_size);
    if (s != 0)
      handle_error_en(s, "posix_memalign");

    printf("posix_memalign() allocated at %p\n", sp);

    //set stack address and stack size

    s = pthread_attr_setstack(&attr, sp, stack_size);
    if (s != 0)
      handle_error_en(s, "pthread_attr_setstack");
  }

  s = pthread_create(&thr, attrp, &thread_start, NULL);
  if (s != 0)
    handle_error_en(s, "pthread_create");

  if (attrp != NULL) {
    s = pthread_attr_destroy(attrp);
    if (s != 0)
      handle_error_en(s, "pthread_attr_destroy");
  }

  pause();    /* Terminates when other thread calls exit() */
}
