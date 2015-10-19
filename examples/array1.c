struct TEST {
    char a;
    int b;
};

int main() {
    
    
    struct TEST array[4];
    array[0].b = 1;
    
    
    return array[0].b;
}
