

 
   
   
   
   

   
   
   

int Bf18KO(int nodecount){
  int INFINITY = 899;
  int edgecount = 18;
  int nodecount = 5;
  int source = 0;
  int x,y,val;
  int i,j; int distance[18];int Source[18];int Dest[18];int Weight[18]; __CPROVER_assume((Source[0]==0 && Source[1]==2 && Source[2]==2 && Source[3]==1 && Source[4]==1 && Source[5]==0 && Source[6]==2 && Source[7]==2 && Source[8]==3 && Source[9]==4 && Source[10]==2 && Source[11]==2 && Source[12]==2 && Source[13]==2 && Source[14]==2 && Source[15]==1 && Source[16]==2 && Source[17]==1) && (Dest[0]==1 && Dest[1]==1 && Dest[2]==1 && Dest[3]==4 && Dest[4]==4 && Dest[5]==3 && Dest[6]==1 && Dest[7]==2 && Dest[8]==3 && Dest[9]==1 && Dest[10]==3 && Dest[11]==2 && Dest[12]==3 && Dest[13]==1 && Dest[14]==2 && Dest[15]==0 && Dest[16]==2 && Dest[17]==4) && (Weight[0]==0 && Weight[1]==1 && Weight[2]==2 && Weight[3]==3 && Weight[4]==4 && Weight[5]==5 && Weight[6]==6 && Weight[7]==7 && Weight[8]==8 && Weight[9]==9 && Weight[10]==10 && Weight[11]==11 && Weight[12]==12 && Weight[13]==13 && Weight[14]==14 && Weight[15]==15 && Weight[16]==16 && Weight[17]==17));
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
  i=0;
  while(i < nodecount)
    { 
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
      i=i+1;
    }
  i=0;
  while(i < edgecount)
    {
      x = Dest[i];
      y = Source[i];
      val = distance[y] + Weight[i];
      if(distance[x] > val)
        {
          return 0;
        }
      i=i+1;
    }

  assert(distance[0] >= 0 && distance[1] >= 0 && distance[2] >= 0 && distance[3] >= 0 && distance[4] >= 0);
  return 0;
}
