
/*  -*- Last-Edit:  Fri Jan 29 11:13:27 1993 by Tarak S. Goradia; -*- */
/* $Log: tcas.c,v $
 * Revision 1.2  1993/03/12  19:29:50  foster
 * Correct logic bug which didn't allow output of 2 - hf
 * */

#define OLEV       600		/* in feets/minute */
#define MAXALTDIFF 600		/* max altitude difference in feet */
#define MINSEP     300          /* min separation in feet */
#define NOZCROSS   100		/* in feet */
				/* variables */

typedef int bool;

				/* state variables */
#define NO_INTENT 0
#define DO_NOT_CLIMB 1
#define DO_NOT_DESCEND 2

#define TCAS_TA 1
#define OTHER 2

#define UNRESOLVED 0
#define UPWARD_RA 1
#define DOWNWARD_RA 2



// Modified MAIN()
int run(
    int Cur_Vertical_Sep,
    int High_Confidence,
    int Two_of_Three_Reports_Valid,
    int Own_Tracked_Alt,
    int Own_Tracked_Alt_Rate,
    int Other_Tracked_Alt,
    int Alt_Layer_Value,
    int Up_Separation,
    int Down_Separation,
    int Other_RAC,
    int Other_Capability,
    int Climb_Inhibit
) {
    
    int Positive_RA_Alt_Thresh[4];
    
    // Init
    Positive_RA_Alt_Thresh[0] = 400;
    Positive_RA_Alt_Thresh[1] = 500;
    Positive_RA_Alt_Thresh[2] = 640;
    Positive_RA_Alt_Thresh[3] = 740;

    
    // Inline
    bool enabled, tcas_equipped, intent_not_known;
    bool need_upward_RA, need_downward_RA;
    int alt_sep;
    
    enabled = High_Confidence && (Own_Tracked_Alt_Rate <= OLEV) && (Cur_Vertical_Sep > MAXALTDIFF);
    tcas_equipped = Other_Capability == TCAS_TA;
    intent_not_known = Two_of_Three_Reports_Valid && Other_RAC == NO_INTENT; 
    
    alt_sep = UNRESOLVED;
    
    if (enabled && ((tcas_equipped && intent_not_known) || !tcas_equipped))
    {
        // Inline
        bool Non_Crossing_Biased_Climb;
        int upward_preferred;
        int upward_crossing_situation;
        upward_preferred = (Climb_Inhibit ? Up_Separation + NOZCROSS : Up_Separation) > Down_Separation;
        if (upward_preferred)
        {
            /* opertor mutation */ // correct: Down_Separation >=
            Non_Crossing_Biased_Climb = !((Own_Tracked_Alt < Other_Tracked_Alt)) || (((Own_Tracked_Alt < Other_Tracked_Alt)) && (!(Down_Separation > Positive_RA_Alt_Thresh[Alt_Layer_Value])));        
        }
        else
        {	
            Non_Crossing_Biased_Climb = (Other_Tracked_Alt < Own_Tracked_Alt) && (Cur_Vertical_Sep >= MINSEP) && (Up_Separation >= Positive_RA_Alt_Thresh[Alt_Layer_Value]);
        }
        // Inline
        bool Non_Crossing_Biased_Descend;
        upward_preferred = (Climb_Inhibit ? Up_Separation + NOZCROSS : Up_Separation) > Down_Separation;
        if (upward_preferred)
        {
            Non_Crossing_Biased_Descend = (Own_Tracked_Alt < Other_Tracked_Alt) && (Cur_Vertical_Sep >= MINSEP) && (Down_Separation >= Positive_RA_Alt_Thresh[Alt_Layer_Value]);
        }
        else
        {
            Non_Crossing_Biased_Descend = !((Other_Tracked_Alt < Own_Tracked_Alt)) || (((Other_Tracked_Alt < Own_Tracked_Alt)) && (Up_Separation >= Positive_RA_Alt_Thresh[Alt_Layer_Value]));
        }        
        
        need_upward_RA = Non_Crossing_Biased_Climb && (Own_Tracked_Alt < Other_Tracked_Alt);
        need_downward_RA = Non_Crossing_Biased_Descend && (Other_Tracked_Alt < Own_Tracked_Alt);
        if (need_upward_RA && need_downward_RA)
            alt_sep = UNRESOLVED;
        else if (need_upward_RA)
            alt_sep = UPWARD_RA;
        else if (need_downward_RA)
            alt_sep = DOWNWARD_RA;
        else
            alt_sep = UNRESOLVED;
    }

    assert(alt_sep==1);
    return alt_sep;
}


int main() {
    
    int r = run(1258,1,0,897,174,7253,1,629,500,0,0,1);
    
    //int r = run(601,742,774,-1,0,0,2,641,640,0,743,0);
    
    
    //printf("result: %d\n", r);
   
    return 0;
}
