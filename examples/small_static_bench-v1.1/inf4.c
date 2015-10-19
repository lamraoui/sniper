int isoscles;
int scalene;
int triangle;
int equilateral;


int main(int a, int b, int c){
  isoscles=scalene=triangle=equilateral=0;
  
  if (a > 0 && b > 0 && c > 0 && a < b +c){
    triangle = 1;
  } else {
    triangle=-1;
  }

  if (a >= b){
    if ( b >= a){
      isoscles = 1;
    }
  } 
  
  if (b >= c){
    if (c >= b){
      isoscles =1 ;
    }
  }

  if (a >= b){
    if (b >= c){
      if (c >= a){
	equilateral =1;
      }
    }
  }
  
  if (a + b > c){
    triangle=1;
  
    if (isoscles == 0 || equilateral == 0){
      scalene=1;
    }
  }
  if (triangle == 1){
    if (equilateral == 1){
      ASSERT(isoscles == 1);
      ASSERT(scalene== 0);
    } else {
      if (isoscles == 0){
	ASSERT(scalene==1);
      }
    }
  }
  
  return equilateral+isoscles + triangle+scalene;
}
