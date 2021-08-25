// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: ff2out.c 44078 2010-07-12 20:17:25Z bruce.tran $	20$Date: 2010/03/15 17:16:05 $ NGS";
static const int  DEBUG = 0;           // diagnostics print if != 0

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ----- classes, structs, types -------------------------------------
#include "dataset1.h"

// ----- functions ---------------------------------------------------
#include "ff2out.h"
#include "dd_dms.h"
#include "trim_c.h"


int ff2out(FILE* ofp, DATASET1 vec_data, 
           double xi, double eta, double lap, int poseast) {
/*******************************************************************************
* Writes a "Free Format, (For Geoid) Type 2" record
* 
* was: string ff2out( iinput,outfil,lout,card, poseast,xlat,xlon,val ) {
*******************************************************************************/
    char  fcard[40];
    char  lat_dms[30];
    char  lon_dms[30];
    char  outrec[42];

    // Initialize local variables
    strncpy(fcard,   "\0", 40);
    strncpy(lat_dms, "\0", 30);
    strncpy(lon_dms, "\0", 30);
    strncpy(outrec,  "\0", 42);

    trim_c( vec_data.text, 'l');
    strncpy(fcard, vec_data.text, 23);
    trim_c( fcard, 'r');

    if (vec_data.lat == -999  ||  vec_data.lon == -999) {
        strcpy(outrec, " 99 99 99.99999 99 99 99.99999");
    } else {
        dd_dms( vec_data.lat, lat_dms );
        dd_dms( vec_data.lon, lon_dms );

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
    // Print to an output file
    // -----------------------------------------------
    if (ofp != NULL) {
        fprintf(ofp, "%s %7.2lf %7.2lf %7.2lf %-s\n", 
            outrec, xi, eta, lap, fcard);
    } else {
        printf(      "%s %7.2lf %7.2lf %7.2lf %-s\n", 
            outrec, xi, eta, lap, fcard);
    }

    return( 0 );

}//~ff2out

