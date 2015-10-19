

 
   
   
   
   
   
   
   
   

void Bf16V2KO(int nodecount){ 
  int INFINITY = 899;
  int edgecount = 16;
  int nodecount = 5;
  int source = 0;
  int x,y,val;
  int i,j;
  int distance[5];int Source[16];int Dest[16];int Weight[16]; __CPROVER_assume((Source[0]==0 && Source[1]==4 && Source[2]==2 && Source[3]==3 && Source[4]==4 && Source[5]==2 && Source[6]==0 && Source[7]==3 && Source[8]==3 && Source[9]==0 && Source[10]==1 && Source[11]==3 && Source[12]==1 && Source[13]==3 && Source[14]==3 && Source[15]==3) && (Dest[0]==1 && Dest[1]==0 && Dest[2]==3 && Dest[3]==0 && Dest[4]==0 && Dest[5]==1 && Dest[6]==2 && Dest[7]==4 && Dest[8]==1 && Dest[9]==2 && Dest[10]==0 && Dest[11]==2 && Dest[12]==2 && Dest[13]==0 && Dest[14]==1 && Dest[15]==3) && (Weight[0]==0 && Weight[1]==1 && Weight[2]==2 && Weight[3]==3 && Weight[4]==4 && Weight[5]==5 && Weight[6]==6 && Weight[7]==7 && Weight[8]==8 && Weight[9]==9 && Weight[10]==10 && Weight[11]==11 && Weight[12]==12 && Weight[13]==13 && Weight[14]==14 && Weight[15]==15));
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
