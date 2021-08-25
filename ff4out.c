// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: ff4out.c 44064 2010-07-12 20:16:31Z bruce.tran $	20$Date: 2009/07/06 18:12:05 $ NGS";

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ----- classes, structs, types -------------------------------------
#include "dataset1.h"

// ----- functions ---------------------------------------------------
#include "ff4out.h"
#include "dd_dms.h"


int ff4out(DATASET1 vec_data, double xi, double eta, double lap, int poseast) {
/*******************************************************************************
* Writes a record from keyboard input to display only
* 
* 
*******************************************************************************/
    // char out_hdr1[]=" Your Result:";
    char out_hdr1[]=
"Station Name            latitude       longitude       Xi      Eta     Hor_Lap";
    char out_hdr2[]=
"                        dd mm ss.sssss ddd mm ss.sssss arc-sec arc-sec arc-sec";
    char  lat_dms[22];
    char  lon_dms[22];
    char  outrec[42];
    static int HEADER_PRINTED = 0;

    // Initialize local variables
    strncpy(lat_dms, "\0", 22);
    strncpy(lon_dms, "\0", 22);
    strncpy(outrec,  "\0", 42);

    if (vec_data.lat == -999  ||  vec_data.lon == -999) {
        strcpy(outrec, " 99 99 99.99999 99 99 99.99999");
    } else {
        dd_dms( vec_data.lat, lat_dms );

        if (poseast == 0) {
            dd_dms( (360.0 - vec_data.lon), lon_dms );
        } else {
            dd_dms( vec_data.lon, lon_dms );
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

        if (strncmp(&outrec[0], "0", 1) == 0)  strncpy(&outrec[0], " ", 1);
    }

    // -----------------------------------------------
    // Print to display
    // -----------------------------------------------
    if (HEADER_PRINTED == 0) {
        printf("\n%s\n%s\n", out_hdr1, out_hdr2);
    }
    printf("%-22s %s %7.2lf %7.2lf %7.2lf\n",
        vec_data.text, outrec, xi, eta, lap);

    ++HEADER_PRINTED;

    return( 0 );

}//~ff4out

