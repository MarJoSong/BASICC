//
// Created by Mars on 2021/7/1.
//

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define handle_error_(msg) \
  do {perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error_en(en, msg) \
  do {errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

struct thread_info {
  pthread_t thread_id;
  int thread_num;
  char *argv_string;
};

/* Thread start function: display address near top of our stack,
 * and return upper-cased copy of argv_string */
static void *thread_start(void *arg) {
  struct thread_info *tinfo = (struct thread_info *) arg;
  char *uargv, *p;

  printf("Thread %d: top of stack near %p; argv_string= %s\n",
         tinfo->thread_num, &p, tinfo->argv_string);

  uargv = strdup(tinfo->argv_string);
  if (uargv == NULL)
    handle_error_("strdup");

  for (p = uargv; *p != 0; p++)
    *p = toupper(*p);

  if (tinfo->thread_num < 2) {
    nanosleep(&(struct timespec){.tv_sec = 0, .tv_nsec= 999999999}, NULL);
  }

  return uargv;
}

int main(int argc, char *argv[]) {
  int s, tnum, opt, num_threads;
  struct thread_info *tinfo;
  pthread_attr_t attr;
  int stack_size;
  void *res;

  /* The "-s" option specifies a stack size for our threads */

  stack_size = -1;
  while ((opt = getopt(argc, argv, "s:")) != -1) {
    switch (opt) {
      case 's' :stack_size = strtoul(optarg, NULL, 0);
        break;
      default:fprintf(stderr, "Usage: %s [-s stack-size] arg...\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  num_threads = argc - optind;

  /* Initialize thread creation attributes */

  s = pthread_attr_init(&attr);
  if (s != 0)
    handle_error_en(s, "pthread_attr_init");

  if (stack_size > 0) {
    s = pthread_attr_setstacksize(&attr, stack_size);
    if (s != 0)
      handle_error_en(s, "pthread_attr_setstacksize");
  }

  /* Allocate memory for pthread_create() arguments */

  tinfo = calloc(num_threads, sizeof(struct thread_info));
  if (tinfo == NULL)
    handle_error_("calloc");

  /* Create one thread for each command-line argument */

  for (tnum = 0; tnum < num_threads; tnum++) {
    tinfo[tnum].thread_num = tnum + 1;
    tinfo[tnum].argv_string = argv[optind + tnum];

    /* The pthread_create() call stores the thread ID into corresponding element of tinfo[] */

    s = pthread_create(&tinfo[tnum].thread_id, &attr,
                       thread_start, &tinfo[tnum]);
    if (s != 0)
      handle_error_en(s, "pthread_create");
  }

  /* Destroy the thread attributes object, since it is no longer needed */

  s = pthread_attr_destroy(&attr);
  if (s != 0)
    handle_error_en(s, "pthread_attr_destroy");

  /* Now join with each thread, and display its returned value */
  /* 主线程可以通过pthread_detach来指定新线程结束后的资源由系统自动回收
   * 也可以通过pthread_join来指定资源由主线程回收，在这种情况下
   * 如果新线程仍在执行，主线程会阻塞，新线程执行完毕后资源被回收
   * 如果在调用pthread_join之前，新线程已经执行结束，主线程不阻塞，直接回收资源
   */

  for (tnum = 0; tnum < num_threads; tnum++) {
    s = pthread_join(tinfo[tnum].thread_id, &res);
    if (s != 0)
      handle_error_en(s, "pthread_join");

    printf("Joined with thread: %d; return value was %s\n",
           tinfo[tnum].thread_num, (char *) res);
    free(res);
  }

  free(tinfo);
  exit(EXIT_SUCCESS);
}

