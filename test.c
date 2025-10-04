#include <stdio.h>
int main() {
  FILE *fp = popen("ls", "r");
  if (!fp) {
    printf("bro miss it");
    return 0;
  }
  char buf[256];
  while (fgets(buf, sizeof(buf), fp)) {
    printf("%s", buf);
  }
  pclose(fp);
  return 0;
}
