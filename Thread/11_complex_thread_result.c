#include <io_utils.h>
#include <time_utils.h>
#include <tinycthread.h>

typedef struct {
  int a;
  char *b;
} ComplexResult;

typedef struct {
  int arg;
  ComplexResult result;
} ComplexArg;


int ComplexReturnValue(ComplexArg *arg) {
  ComplexResult *ret = &arg->result;
  ret->a = 10;
  ret->b = "Hello";
}

int main(void) {
  thrd_t t_1;
  ComplexArg arg = {.arg = 110};
  thrd_create(&t_1, ComplexReturnValue, &arg);

  int result;
  thrd_join(t_1, &result);
  PRINT_INT(arg.result.a);
  puts(arg.result.b);
}
