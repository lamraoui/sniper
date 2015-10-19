
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
int Positive_RA_Alt_Thresh[4];

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
    Positive_RA_Alt_Thresh[0] = 400;
    Positive_RA_Alt_Thresh[1] = 500;
    Positive_RA_Alt_Thresh[2] = 640;
    Positive_RA_Alt_Thresh[3] = 740;
}

int ALIM ()
{
    return Positive_RA_Alt_Thresh[Alt_Layer_Value];
}


//Positive_RA_Alt_Thresh[0] = 400;
//Positive_RA_Alt_Thresh[1] = 500;
//Positive_RA_Alt_Thresh[2] = 640;
//Positive_RA_Alt_Thresh[3] = 740;
// return Positive_RA_Alt_Thresh[Alt_Layer_Value];

// Down_Separation:
((Alt_Layer_Value==0 && Down_Separation>=400)
|| (Alt_Layer_Value==1 && Down_Separation>=500)
|| (Alt_Layer_Value==2 && Down_Separation>=640)
|| (Alt_Layer_Value==3 && Down_Separation>=740))

// Up_Separation:
((Alt_Layer_Value==0 && Up_Separation>=400)
|| (Alt_Layer_Value==1 && Up_Separation>=500)
|| (Alt_Layer_Value==2 && Up_Separation>=640)
|| (Alt_Layer_Value==3 && Up_Separation>=740))




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







       
        
        
        ((((High_Confidence && (Own_Tracked_Alt_Rate <= 600) && (Cur_Vertical_Sep > 600)) 
         && (((Other_Capability == 1) 
              && (Two_of_Three_Reports_Valid && Other_RAC == 0)) 
             || !(Other_Capability == 1))) (Alt_Layer_Value==0) &&
       // need_upward_RA =
        ((alt_sep==UPWARD_RA) && 
        ((((Climb_Inhibit ? Up_Separation + 100 : Up_Separation) > Down_Separation)
                           && (!(Own_Tracked_Alt < Other_Tracked_Alt) || ((Own_Tracked_Alt < Other_Tracked_Alt) && (!(Down_Separation >= 400)))))  
                          ||
                          (((Climb_Inhibit ? Up_Separation + 100 : Up_Separation) > Down_Separation)
                           && ((Other_Tracked_Alt < Own_Tracked_Alt) && (Cur_Vertical_Sep >= 300) && (Up_Separation >= 400)))
                          && (Own_Tracked_Alt < Other_Tracked_Alt)))
        
        
        //need_downward_RA = 
         ((alt_sep==DOWNWARD_RA) && 
        ((((Climb_Inhibit ? Up_Separation + 100 : Up_Separation) > Down_Separation)
                            && ((Own_Tracked_Alt < Other_Tracked_Alt) && (Cur_Vertical_Sep >= 300) && (Down_Separation >= 400))
                            || (((Climb_Inhibit ? Up_Separation + 100 : Up_Separation) > Down_Separation)
                             && (!(Other_Tracked_Alt < Own_Tracked_Alt) || ((Other_Tracked_Alt < Own_Tracked_Alt) && (Up_Separation >= 400)))))
                            && (Other_Tracked_Alt < Own_Tracked_Alt))))
        // UNRESOLVED
        ||  (alt_sep==UNRESOLVED))
        
        
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
