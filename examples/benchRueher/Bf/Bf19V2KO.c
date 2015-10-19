

 
   
   
   
   
   
   
   
   

void Bf19V2KO(int nodecount){ 
  int INFINITY = 899;
  int edgecount = 19;
  int nodecount = 5;
  int source = 0;
  int x,y,val;
  int i,j;
  int distance[5];int Source[19];int Dest[19];int Weight[19]; __CPROVER_assume((Source[0]==0 && Source[1]==3 && Source[2]==0 && Source[3]==3 && Source[4]==3 && Source[5]==2 && Source[6]==3 && Source[7]==3 && Source[8]==2 && Source[9]==1 && Source[10]==3 && Source[11]==0 && Source[12]==3 && Source[13]==1 && Source[14]==2 && Source[15]==2 && Source[16]==4 && Source[17]==0 && Source[18]==2) && (Dest[0]==1 && Dest[1]==1 && Dest[2]==2 && Dest[3]==3 && Dest[4]==1 && Dest[5]==0 && Dest[6]==4 && Dest[7]==3 && Dest[8]==1 && Dest[9]==1 && Dest[10]==0 && Dest[11]==4 && Dest[12]==0 && Dest[13]==3 && Dest[14]==0 && Dest[15]==0 && Dest[16]==1 && Dest[17]==3 && Dest[18]==4) && (Weight[0]==0 && Weight[1]==1 && Weight[2]==2 && Weight[3]==3 && Weight[4]==4 && Weight[5]==5 && Weight[6]==6 && Weight[7]==7 && Weight[8]==8 && Weight[9]==9 && Weight[10]==10 && Weight[11]==11 && Weight[12]==12 && Weight[13]==13 && Weight[14]==14 && Weight[15]==15 && Weight[16]==16 && Weight[17]==17 && Weight[18]==18));
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
