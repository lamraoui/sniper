
int main(argc, argv)
int argc;
int argv[];
{
    
    argv[1] = 3;
    
    int x;
    x = argv[1];
    return x;
}

/*
 
 int main(argc, argv)
 int argc;
 char *argv[];
 {
 
 argv[1] = "3";
 
 int x;
 x = atoi(argv[1]);
 return x;
 }
 
*/