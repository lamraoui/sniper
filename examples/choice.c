
int main(int b) {
    
    int z = 100;
    int x;
  
    x = (b ? z + 100 : 300);
    
    assert(x<300);
    
    return x;
}

