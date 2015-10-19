
int Positive_RA_Alt_Thresh[4];
int Alt_Layer_Value;

void initialize()
{
    Positive_RA_Alt_Thresh[0] = 400;
    Positive_RA_Alt_Thresh[1] = 500;
    Positive_RA_Alt_Thresh[2] = 640;
    Positive_RA_Alt_Thresh[3] = 740;
}

int ALIM ()
{
    return Positive_RA_Alt_Thresh[Alt_Layer_Value];
}

int foo(int x) {

    initialize();
    
    if (x==0) {
        Alt_Layer_Value = 0;
    } else if (x==1) {
        Alt_Layer_Value = 1;
    } else if (x==2) {
        Alt_Layer_Value = 2;
    } else if (x==3) {
        Alt_Layer_Value = 3;
    } else {
        Alt_Layer_Value = 3;
    }
    
    int result = ALIM();

    
    assert(result>400);
    return result;
}