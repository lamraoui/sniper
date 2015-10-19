

int main() {

    int y;
    int x = 1;
    int z = 3;
    
    if (x==1) {
        y = z;
    } else {
        y = 1;
    }

    
    int ret = x+y+z;
    assert(ret<4);
    return ret;
}