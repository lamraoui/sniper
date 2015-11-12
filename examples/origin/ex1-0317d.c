/*
  pre:  16 > x >= 0
  post: ((0 <= x <= 1) and (y == x + 6)) 
    or ((1 < x <= 6)   and (y == 3 - x))
    or ((6 < x < 10)   and (y == 6))
    or ((10 <= x < 16) and (y == -1)) 

post condition takes the form of (A or B or C or D)
its negagtion will be
not(A or B or C or D) == (not A) and (not B) and (not C) and (not D)
thus translated into
(assert (not A))
(assert (not B))
(assert (not C))
(assert (not D))
which might be simpler than the previous example...
*/

int y;

int foo (int x) {
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

  if (v < 2) {
    y = u + v;
  } else {
    y = v - u;
  }

  return(y);
}
