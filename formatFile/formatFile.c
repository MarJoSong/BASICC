#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

#define GHOST_TIPS  "\n[中]系统中存在以下相同UID的用户 | [EN]The following users with the same uid exist in the system\n"
#define USER_TIPS   "[中]系统中存在 %d 天内未登录过的用户 | " \
        "[EN]There are users in the system who have not logged in for %d days\n"

int format_file(char *result_file, int unused_time) {
  FILE *fp = NULL;
  char *buffer = NULL;
  char *buffer_format = NULL;
  size_t buffer_format_len = 0;
  size_t line_len = 0;
  int connect_fail = 0;
  char write_checkGhostAccount = 0;
  char write_checkUserLogin = 0;
  struct stat file_stat;

  if (-1 == stat(result_file, &file_stat)) {
    perror("stat resultfile");
    return __LINE__;
  }

  buffer_format = malloc(file_stat.st_size + 512);
  if (NULL == buffer_format) {
    perror("malloc");
    return __LINE__;
  }

  fp = fopen(result_file, "r");
  if (NULL == fp)
    return __LINE__;

  while (-1 != getline(&buffer, &line_len, fp)) {
    if (strstr(buffer, "Host Connection Failed")) {
      connect_fail = 1;
    }

    if (write_checkUserLogin == 0) {
      if (write_checkGhostAccount == 0) {
        if (strstr(buffer, "checkGhostAccount")) {
          write_checkGhostAccount = 1;
          strcpy(buffer_format, GHOST_TIPS);
        }
      } else {
        if (strstr(buffer, "checkUserLogin")) {
          write_checkGhostAccount = 0;
          write_checkUserLogin = 1;
          char tmp[1024];
          sprintf(tmp, USER_TIPS, unused_time, unused_time);
          strcat(buffer_format, tmp);
        } else {
          strcat(buffer_format, buffer);
        }
      }
    } else {
      buffer_format_len = strlen(buffer_format);
      strcat(buffer_format, buffer);
    }
  }
  free(buffer);
  buffer = NULL;
  fclose(fp);
  fp = NULL;

  fp = fopen(result_file, "w");
  if (NULL == fp)
    return __LINE__;

  if (connect_fail) {
    sprintf(buffer_format, "[中]主机连接失败 | [EN]Host Connection Failed\n");
  } else {
    buffer_format[buffer_format_len] = '\0';
  }
  printf("buffer_fromat:[%s]", buffer_format);
  fwrite(buffer_format, strlen(buffer_format), 1, fp);
  fclose(fp);
  fp = NULL;

  return 0;
}

int main(int argc, char *argv[]) {
  format_file("test.txt", 3);

  return 0;
}
