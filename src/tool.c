#include "tools.h"
int insert_left_int(int x, int n) {
  int t = n;
  if (t == 0)
    return x;
  int mul = 1;
  while (t > 0) {
    mul *= 10;
    t /= 10;
  }
  return x * mul + n;
}
