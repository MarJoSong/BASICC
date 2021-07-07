//
// Created by Mars on 2021/7/7.
//
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void printSignal(sigset_t *sig) {
  for (int i=1; i <= 32; ++i) {
    if (sigismember(sig, i)) {
      putchar('1');
    } else {
      putchar('0');
    }
  }
  puts("");
}

int main() {
  sigset_t p, q;
  sigemptyset(&p);
  int i = 0;
  sigaddset(&p, SIGINT);
  sigprocmask(SIG_BLOCK, &p, NULL);

  while (1) {
    sigpending(&q);
    printSignal(&q);
    i++;
    sleep(1);
    if (i==6)
      sigprocmask(SIG_UNBLOCK, &p, NULL);
  }

}

