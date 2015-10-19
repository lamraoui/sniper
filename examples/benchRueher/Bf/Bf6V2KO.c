

 
   
   
   
   
   
   
   
   

void Bf6V2KO(int nodecount){ 
  int INFINITY = 899;
  int edgecount = 6;
  int nodecount = 5;
  int source = 0;
  int x,y,val;
  int i,j;
  int distance[5];int Source[6];int Dest[6];int Weight[6]; __CPROVER_assume((Source[0]==0 && Source[1]==1 && Source[2]==4 && Source[3]==2 && Source[4]==3 && Source[5]==1) && (Dest[0]==1 && Dest[1]==2 && Dest[2]==4 && Dest[3]==0 && Dest[4]==2 && Dest[5]==4) && (Weight[0]==0 && Weight[1]==1 && Weight[2]==2 && Weight[3]==3 && Weight[4]==4 && Weight[5]==5));
  i=0;
  while(i < nodecount){
    if(i == source){
      distance[i] = -1; // error in the assignment : should be "distance[i] = 0;"
    }
    else {
      distance[i] = INFINITY;
    }
    i=i+1;
  }
  j=0;
  while(j < edgecount)
  {
    x = Dest[j];
    y = Source[j];
    val = distance[y] + Weight[j];
    if(distance[x] > val)
    {
       distance[x] = val;
    }
    j=j+1;
  } 
  assert(distance[0] >= 0 && distance[1] >= 0 && distance[2] >= 0 && distance[3] >= 0 && distance[4] >= 0);
}
