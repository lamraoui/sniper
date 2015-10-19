
/*  -*- Last-Edit:  Fri Jan 29 11:13:27 1993 by Tarak S. Goradia; -*- */
/* $Log: tcas.c,v $
 * Revision 1.2  1993/03/12  19:29:50  foster
 * Correct logic bug which didn't allow output of 2 - hf
 * */

#include <stdio.h>

#define OLEV       600		/* in feets/minute */
#define MAXALTDIFF 600		/* max altitude difference in feet */
#define MINSEP     300          /* min separation in feet */
#define NOZCROSS   100		/* in feet */
/* variables */

typedef int bool;

int Cur_Vertical_Sep;
bool High_Confidence;
bool Two_of_Three_Reports_Valid;

int Own_Tracked_Alt;
int Own_Tracked_Alt_Rate;
int Other_Tracked_Alt;

int Alt_Layer_Value;		/* 0, 1, 2, 3 */
//int Positive_RA_Alt_Thresh[4];
int Positive_RA_Alt_Thresh_0;
int Positive_RA_Alt_Thresh_1;
int Positive_RA_Alt_Thresh_2;
int Positive_RA_Alt_Thresh_3;

int Up_Separation;
int Down_Separation;

/* state variables */
int Other_RAC;			/* NO_INTENT, DO_NOT_CLIMB, DO_NOT_DESCEND */
#define NO_INTENT 0
#define DO_NOT_CLIMB 1
#define DO_NOT_DESCEND 2

int Other_Capability;		/* TCAS_TA, OTHER */
#define TCAS_TA 1
#define OTHER 2

int Climb_Inhibit;		/* true/false */

#define UNRESOLVED 0
#define UPWARD_RA 1
#define DOWNWARD_RA 2

void initialize()
{
    //Positive_RA_Alt_Thresh[0] = 400;
    //Positive_RA_Alt_Thresh[1] = 500;
    //Positive_RA_Alt_Thresh[2] = 640;
    //Positive_RA_Alt_Thresh[3] = 740;
    Positive_RA_Alt_Thresh_0 = 400;
    Positive_RA_Alt_Thresh_1 = 500;
    Positive_RA_Alt_Thresh_2 = 640;
    Positive_RA_Alt_Thresh_3 = 700; // Positive_RA_Alt_Thresh[3] = 700; /* constant mutation */
}

int ALIM ()
{
    if (Alt_Layer_Value==0) {
        return Positive_RA_Alt_Thresh_0;
    }
    else if (Alt_Layer_Value==1) {
        return Positive_RA_Alt_Thresh_1;
    }
    else if (Alt_Layer_Value==2) {
        return Positive_RA_Alt_Thresh_2;
    }
    else if (Alt_Layer_Value==3) {
        return Positive_RA_Alt_Thresh_3;
    } else {
        return 0;
    }
    //return Positive_RA_Alt_Thresh[Alt_Layer_Value];
    
}

int Inhibit_Biased_Climb ()
{
    return (Climb_Inhibit ? Up_Separation + NOZCROSS : Up_Separation);
}

bool Non_Crossing_Biased_Climb()
{
    int upward_preferred;
    int upward_crossing_situation;
    bool result;
    
    upward_preferred = Inhibit_Biased_Climb() > Down_Separation;
    if (upward_preferred)
    {
        result = !(Own_Below_Threat()) || ((Own_Below_Threat()) && (!(Down_Separation >= ALIM()))); 
    }
    else
    {	
        result = Own_Above_Threat() && (Cur_Vertical_Sep >= MINSEP) && (Up_Separation >= ALIM());
    }
    return result;
}

bool Non_Crossing_Biased_Descend()
{
    int upward_preferred;
    int upward_crossing_situation;
    bool result;
    
    upward_preferred = Inhibit_Biased_Climb() > Down_Separation;
    if (upward_preferred)
    {
        result = Own_Below_Threat() && (Cur_Vertical_Sep >= MINSEP) && (Down_Separation >= ALIM());
    }
    else
    {
        result = !(Own_Above_Threat()) || ((Own_Above_Threat()) && (Up_Separation >= ALIM()));
    }
    return result;
}

bool Own_Below_Threat()
{
    return (Own_Tracked_Alt < Other_Tracked_Alt);
}

bool Own_Above_Threat()
{
    return (Other_Tracked_Alt < Own_Tracked_Alt);
}

int alt_sep_test()
{
    bool enabled, tcas_equipped, intent_not_known;
    bool need_upward_RA, need_downward_RA;
    int alt_sep;
    
    enabled = High_Confidence && (Own_Tracked_Alt_Rate <= OLEV) && (Cur_Vertical_Sep > MAXALTDIFF);
    tcas_equipped = Other_Capability == TCAS_TA;
    intent_not_known = Two_of_Three_Reports_Valid && Other_RAC == NO_INTENT;
    
    alt_sep = UNRESOLVED;
    
    if (enabled && ((tcas_equipped && intent_not_known) || !tcas_equipped))
    {
        need_upward_RA = Non_Crossing_Biased_Climb() && Own_Below_Threat();
        need_downward_RA = Non_Crossing_Biased_Descend() && Own_Above_Threat();
        if (need_upward_RA && need_downward_RA)
        /* unreachable: requires Own_Below_Threat and Own_Above_Threat
         to both be true - that requires Own_Tracked_Alt < Other_Tracked_Alt
         and Other_Tracked_Alt < Own_Tracked_Alt, which isn't possible */
            alt_sep = UNRESOLVED;
        else if (need_upward_RA)
            alt_sep = UPWARD_RA;
        else if (need_downward_RA)
            alt_sep = DOWNWARD_RA;
        else
            alt_sep = UNRESOLVED;
    }
    
    return alt_sep;
}

//main(argc, argv)
//int argc;
//char *argv[];
//{
void foo(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9
         , int arg10, int arg11, int arg12) {
    /*if(argc < 13)
     {
     //fprintf(stdout, "Error: Command line arguments are\n");
     //fprintf(stdout, "Cur_Vertical_Sep, High_Confidence, Two_of_Three_Reports_Valid\n");
     //fprintf(stdout, "Own_Tracked_Alt, Own_Tracked_Alt_Rate, Other_Tracked_Alt\n");
     //fprintf(stdout, "Alt_Layer_Value, Up_Separation, Down_Separation\n");
     //fprintf(stdout, "Other_RAC, Other_Capability, Climb_Inhibit\n");
     exit(1);
     }*/
    if(arg7<0 || arg7>3) exit(1);
    initialize();
    Cur_Vertical_Sep = arg1;
    High_Confidence = arg2;
    Two_of_Three_Reports_Valid = arg3;
    Own_Tracked_Alt = arg4;
    Own_Tracked_Alt_Rate = arg5;
    Other_Tracked_Alt = arg6;
    Alt_Layer_Value = arg7;
    Up_Separation = arg8;
    Down_Separation = arg9;
    Other_RAC = arg10;
    Other_Capability = arg11;
    Climb_Inhibit = arg12;
    
    
    int ret_alt_sep_test = alt_sep_test();
    
    
    
    assert((((High_Confidence && (Own_Tracked_Alt_Rate <= 600) && (Cur_Vertical_Sep > 600)) 
             && (((Other_Capability == 1) 
                  && (Two_of_Three_Reports_Valid && Other_RAC == 0)) 
                 || !(Other_Capability == 1))) && (Alt_Layer_Value==0) &&
            // need_upward_RA =
            ((ret_alt_sep_test==UPWARD_RA) && 
             ((((Climb_Inhibit ? Up_Separation + 100 : Up_Separation) > Down_Separation)
               && (!(Own_Tracked_Alt < Other_Tracked_Alt) || ((Own_Tracked_Alt < Other_Tracked_Alt) && (!((Alt_Layer_Value==0 && Down_Separation>=400)
                                                                                                          || (Alt_Layer_Value==1 && Down_Separation>=500)
                                                                                                          || (Alt_Layer_Value==2 && Down_Separation>=640)
                                                                                                          || (Alt_Layer_Value==3 && Down_Separation>=740))))))  
              ||
              (((Climb_Inhibit ? Up_Separation + 100 : Up_Separation) > Down_Separation)
               && ((Other_Tracked_Alt < Own_Tracked_Alt) && (Cur_Vertical_Sep >= 300) && ((Alt_Layer_Value==0 && Up_Separation>=400)
                                                                                          || (Alt_Layer_Value==1 && Up_Separation>=500)
                                                                                          || (Alt_Layer_Value==2 && Up_Separation>=640)
                                                                                          || (Alt_Layer_Value==3 && Up_Separation>=740))))
              && (Own_Tracked_Alt < Other_Tracked_Alt)))
            
            ||
            
            //need_downward_RA = 
            ((ret_alt_sep_test==DOWNWARD_RA) && 
             ((((Climb_Inhibit ? Up_Separation + 100 : Up_Separation) > Down_Separation)
               && ((Own_Tracked_Alt < Other_Tracked_Alt) && (Cur_Vertical_Sep >= 300) && ((Alt_Layer_Value==0 && Down_Separation>=400)
                                                                                          || (Alt_Layer_Value==1 && Down_Separation>=500)
                                                                                          || (Alt_Layer_Value==2 && Down_Separation>=640)
                                                                                          || (Alt_Layer_Value==3 && Down_Separation>=740)))
               || (((Climb_Inhibit ? Up_Separation + 100 : Up_Separation) > Down_Separation)
                   && (!(Other_Tracked_Alt < Own_Tracked_Alt) || ((Other_Tracked_Alt < Own_Tracked_Alt) && ((Alt_Layer_Value==0 && Up_Separation>=400)
                                                                                                            || (Alt_Layer_Value==1 && Up_Separation>=500)
                                                                                                            || (Alt_Layer_Value==2 && Up_Separation>=640)
                                                                                                            || (Alt_Layer_Value==3 && Up_Separation>=740))))))
              && (Other_Tracked_Alt < Own_Tracked_Alt))))
           // UNRESOLVED
           ||  (ret_alt_sep_test==UNRESOLVED));
    
    
    
    
    //fprintf(stdout, "%d\n", alt_sep_test());
    exit(0);
}
