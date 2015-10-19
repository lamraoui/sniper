

int main(argc, argv)
int argc;
char *argv[];
{
    if(argc < 3) {
        return 1;
         //exit(1);
    }

    int arg1 = atoi(argv[1]);
    int arg2 = atoi(argv[2]);
    int arg3 = atoi(argv[3]);
    
    

    int x = arg1+arg2+arg3;
    assert(x==42);
    
    //exit(0);
    return 0;
}
