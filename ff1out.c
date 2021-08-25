// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: ff1out.c 44067 2010-07-12 20:16:43Z bruce.tran $	20$Date: 2009/11/30 16:20:33 $ NGS";
static const int  DEBUG = 0;           // diagnostics print if != 0

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ----- classes, structs, types -------------------------------------
#include "dataset1.h"

// ----- functions ---------------------------------------------------
#include "ff1out.h"
#include "dd_dms.h"


int ff1out( FILE* ofp, DATASET1 vec_data, 
            double xi, double eta, double lap, int poseast ) {
/*******************************************************************************
* Writes a "Free Format, (For Geoid) Type 1" record
* 
* was: string ff1out( iinput,outfil,lout,card, poseast,xlat,xlon,val ) {
*******************************************************************************/
    char  fcard[40];
    char  lat_dms[30];
    char  lon_dms[30];
    char  outrec[42];

    if (DEBUG > 0) {
        printf("In ff1out  vec_data.lat = %lf  vec_data.lon = %lf\n", 
                vec_data.lat, vec_data.lon);
    }

    // Initialize local variables
    strncpy(fcard,   "\0", 40);
    strncpy(lat_dms, "\0", 30);
    strncpy(lon_dms, "\0", 30);
    strncpy(outrec,  "\0", 42);

    strncpy(fcard, vec_data.text, 23);

    if (vec_data.lat == -999  ||  vec_data.lon == -999) {
        strcpy(outrec, "99 99 99.99999 99 99 99.99999");
    } else {
        dd_dms( vec_data.lat, lat_dms );

        if (poseast == 0) {                              // if west longitude
            dd_dms( (360.0 - vec_data.lon), lon_dms );   // from East to West
        } else {
            dd_dms( vec_data.lon, lon_dms );
        }

        if (DEBUG > 0) {
            printf("In ff1out lat_dms = %s  lon_dms = %s\n", lat_dms, lon_dms);
        }

        strncpy( outrec, &lat_dms[0], 3);
        strncat( outrec, " ",         1);
        strncat( outrec, &lat_dms[3], 2);
        strncat( outrec, " ",         1);
        strncat( outrec, &lat_dms[5], 8);
        strncat( outrec, " ",         1);

        strncat( outrec, &lon_dms[0], 3);
        strncat( outrec, " ",         1);
        strncat( outrec, &lon_dms[3], 2);
        strncat( outrec, " ",         1);
        strncat( outrec, &lon_dms[5], 8);


        if (outrec[0] == '0')  outrec[0] = ' ';
        if (outrec[4] == '0')  outrec[4] = ' ';
        if (outrec[7] == '0')  outrec[7] = ' ';

        if( (outrec[16] == '0') 
        &&  (outrec[17] == '0') ) {
            outrec[16] = ' ';
            outrec[17] = ' ';
        } else
        if  (outrec[16] == '0') outrec[16] = ' ';

        if (outrec[20] == '0')  outrec[20] = ' ';
        if (outrec[23] == '0')  outrec[23] = ' ';

    }

    // -----------------------------------------------
    // Print to output file (input by file form 1)  or
    // Print to display     (input by keyboard)
    // -----------------------------------------------
    //was:    fprintf(ofp, "%22s %s %9.5lf %9.5lf %9.5lf",

    if (ofp != NULL) {
        fprintf(ofp, "%-22s %s %7.2lf %7.2lf %7.2lf\n",
            fcard, outrec, xi, eta, lap);
    }

    return( 0 );

}//~ff1out

