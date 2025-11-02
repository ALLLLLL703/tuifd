#include <stdio.h>
#include <string.h>
typedef struct _cmd cmd;
struct _cmd {
  char **opt;
  char *dir;
  char *pattern;
};
int main(int argc, char **argv) {
  char *s1 = "fd";
  char **opts = NULL;
  cmd c1 = {} FILE *fp = popen(cmd, "r");
  if (!fp) {
    printf("bro miss it");
    return 0;
  }
  char buf[256];
  printf("output: \n");
  while (fgets(buf, sizeof(buf), fp)) {
    printf("%s", buf);
  }
  pclose(fp);
  return 0;
}
