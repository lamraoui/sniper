int add2(int x) {
    int y = x + 2;
    return y;
}

int foo(int index) 
{
  int i;
  if(index!=1)             // Potential Bug 1
    index = 2;
  else
      index = add2(index); //index + 2;    // Potential Bug 2

  i = index;
  assert(i>=0 && i<3);
  return i;
}
