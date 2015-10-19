/*
  pre:  x >= 0
  post: ((x =< 1) implies (y > 5)) and ((1 < x) implies (y <= 1)) 
*/

int y;

void foo (int x) {
  int a, b, u, v;

  if (x < 10) {
    a = 4; b = 1;
  } else {
    a = 1; b = 4;
  }

  if (x + a > 10) {
    u = a + b; v = b;
  } else if (x + a <= 5) {
    u = a + b; v = x + b;
  } else {
    u = x + b; v = a;
  }

  if (v < 2) { // BUG
    y = u + v;
  } else {
    y = v - u;
  }
}

