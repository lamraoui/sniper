struct TEST {
    char a;
    int b;
    int c;
    int d[200];
};

int main() {
    
    struct TEST myStruct;
    myStruct.a = 10;
    myStruct.b = 20;
    myStruct.c = 30;
    
    return myStruct.b;
}
