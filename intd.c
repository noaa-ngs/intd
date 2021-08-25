// %P%
// ----- constants ---------------------------------------------------
static const char PGMVER[]="4.2";
static const char PGMDAT[]="01/22/2015";
       const int  DEBUG = 0;           // diagnostics print if != 0
static const int  MEM_STEP = 40;       // dynamic allocation increment

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>      // sqrt(), M_PI

// ----- classes, structures, types ----------------------------------
#include "grid_header.h"
#include "dataset1.h"

// ----- functions ---------------------------------------------------
#include "bb80ll.h"
#include "expform.h"
#include "c2v.h"
#include "ff1.h"
#include "ff1out.h"
#include "ff2.h"
#include "ff2out.h"
#include "ff4out.h"
#include "getdir_defl.h"
#include "getgrd_defl.h"
#include "getheaders.h"
#include "intro.h"
#include "interg.h"
#include "run_bbk.h"
#include "trim_c.h"
#include "which_defl.h"

FILE *efp;
int fatal_error;
int vc_unit;
int car97_unit;
char old_cluster_rec[255];

/*********************************************************************
* For further technical information, questions, or comments:
*   NOAA, National Geodetic Survey, N/NGS6, Silver Spring, MD  U.S.A.
*   Attn   : Daniel R. Roman, Ph.D.
*   Phone  : 301-713-3202
*   Fax    : 301-713-4172
*   e-mail : dan.roman@noaa.gov
*********************************************************************/

/*********************************************************************
*                                                                    *
*                            DISCLAIMER                              *
*                                                                    *
*   THIS PROGRAM AND SUPPORTING INFORMATION IS FURNISHED BY THE      *
* GOVERNMENT OF THE UNITED STATES OF AMERICA, AND IS ACCEPTED AND    *
* USED BY THE RECIPIENT WITH THE UNDERSTANDING THAT THE UNITED STATES*
* GOVERNMENT MAKES NO WARRANTIES, EXPRESS OR IMPLIED, CONCERNING THE *
* ACCURACY, COMPLETENESS, RELIABILITY, OR SUITABILITY OF THIS        *
* PROGRAM, OF ITS CONSTITUENT PARTS, OR OF ANY SUPPORTING DATA.      *
*                                                                    *
*   THE GOVERNMENT OF THE UNITED STATES OF AMERICA SHALL BE UNDER NO *
* LIABILITY WHATSOEVER RESULTING FROM ANY USE OF THIS PROGRAM.  THIS *
* PROGRAM SHOULD NOT BE RELIED UPON AS THE SOLE BASIS FOR SOLVING A  *
* PROBLEM WHOSE INCORRECT SOLUTION COULD RESULT IN INJURY TO PERSON  *
* OR PROPERTY.                                                       *
*                                                                    *
*   THIS PROGRAM IS PROPERTY OF THE GOVERNMENT OF THE UNITED STATES  *
* OF AMERICA.  THEREFORE, THE RECIPIENT FURTHER AGREES NOT TO ASSERT *
* PROPRIETARY RIGHTS THEREIN AND NOT TO REPRESENT THIS PROGRAM TO    *
* ANYONE AS BEING OTHER THAN A GOVERNMENT PROGRAM.                   *
*                                                                    *
*********************************************************************/


int main( const int argc, const char* argv[] ) {
/*******************************************************************************
* NGS program intd - "INTerpolate from Deflection files"
* Program will do the following:
*   1) Take two or more associated deflection files
*      in binary (*.bin) format and compute,
*      via interpolation from modelled grid files,
*      the N/S and E/W components of the deflection of the vertical
*      at any given latitude/longitude combination.
*      In addition, the horizontal Laplace azimuth correction is computed.
*      This interpolation can be done for one or more points,
*      interactively or through input/output files.
*
* Note that PAIRS of files must exist together for this program to work
* (i.e. For each Xi grid, there must be an associated Eta grid)
*
* Note that "*.bin" format is binary, unformatted, direct access
* and that the order of bytes in the geoid model data files
* --- depends on which platform the file was created ---
* Platform dependant endian condition is corrected for the binary data.
*******************************************************************************/
    FILE* ifp;
    FILE* ofp;
    FILE* vec_xifp[50];            // vector of FILE*     for Xi
    FILE* vec_eifp[50];            // vector of FILE*     for Eta
    char  vec_xfnames[50][256];    // vector of filenames for Xi
    char  vec_efnames[50][256];    // vector of filenames for Eta

    GRID_HEADER vec_xhdr[50];      // vector of header file data, Xi only
    DATASET1*   vec_data;  // ptr to vector of lat-lon-text input data

    char    dash70[]=
"----------------------------------------------------------------------";
    char final_header1[] = "Basic Statistics: ";
    char final_header2[] = 
"Val   AVE   Sigma    Min  Lat of Min Lon of Min   Max   Lat of Max Lon of Max";
    char final_header3[] = 
"     (sec)  (sec)   (sec)  (dec deg)  (dec deg)  (sec)   (dec deg)  (dec deg)";

    char    card[90];
    char    card2[90];
    char    card86[90];
    char    dirnam[256];
    char    rec_in[90];
    char    ifname[256];  //  input file name
    char    ofname[256];  // output file name   
    char    ofyn[2];
    char    cinput[42];
    char    text[42];

    double  xlat;
    double  xlon;

    // Variables for statistics of the run
    double  minx    =  999999.0;    // min Xi  value
    double  mine    =  999999.0;    // min Eta value
    double  minh    =  999999.0;    // min Lap value
    double  maxx    = -999999.0;    // max Xi  value
    double  maxe    = -999999.0;    // max Eta value
    double  maxh    = -999999.0;    // max Lap value

    double  minxlat =      90.0;    // lat at min Xi location 
    double  minxlon =     360.0;    // lon at min Xi location 
    double  maxxlat =     -90.0;    // lat at max Xi location 
    double  maxxlon =    -180.0;    // lon at max Xi location 

    double  minelat =      90.0;    // lat at min Eta location 
    double  minelon =     360.0;    // lon at min Eta location 
    double  maxelat =     -90.0;    // lat at max Eta location 
    double  maxelon =    -180.0;    // lon at max Eta location 

    double  minhlat =      90.0;    // lat at min Lap location 
    double  minhlon =     360.0;    // lon at min Lap location 
    double  maxhlat =     -90.0;    // lat at max Lap location 
    double  maxhlon =    -180.0;    // lon at max Lap location 
    double  xn      = -999999.0;    // northernmost lat
    double  xs      =  999990.0;    // southernmost lat
    double  xe      = -999999.0;    //  easternmost lon
    double  xw      =  999999.0;    //  westernmost lon

    double  fact    = 0.0;

    double  avex    = 0.0;          // component statistics
    double  avee    = 0.0;
    double  aveh    = 0.0;
    double  stdx    = 0.0;
    double  stde    = 0.0;
    double  stdh    = 0.0;
    double  rmsx    = 0.0;
    double  rmse    = 0.0;
    double  rmsh    = 0.0;

    double  valx    = 0.0;          // Xi  solution
    double  vale    = 0.0;          // Eta solution
    double  valh    = 0.0;          // Lap solution

    int  iform  = 0;
    int  ipos;
    int  keep   = 0;
    int  kount  = 0;
    int  mem_limit = 0;
    int  kk  = 0;
    int  iii = 0;
    int  iinput;                 // Keyboard/File input option
    int  imodel;
    int  is_subr;
    int  nfiles;
    int  nxff;
    int  neff;
    int  ii=0;
    int  jj=0;
    int  poseast = 0;  // Assume west longitude
    int  outfil  = 0;  // Cstd:  0 = false;  !0 = not false


    // -----------------------------------------------------
    // Initialize variables - Zero out the statistics for this run
    // -----------------------------------------------------
    strncpy(dirnam, "\0", 256);
    strncpy(cinput, "\0",  42);
    strncpy(text,   "\0",  42);
    strncpy(card,   "\0",  90);
    strncpy(card2,  "\0",  90);
    strncpy(card86, "\0",  90);
    strncpy(rec_in, "\0",  90);
    strncpy(ofyn,   "\0",   2);

    for (ii = 0; ii < 50; ++ii) {
        strncpy(vec_xfnames[ii], "\0", 256);
        strncpy(vec_efnames[ii], "\0", 256);
    }

    // ---------------------------------------------------------
    // Write out the introductory/disclaimer screens
    // ---------------------------------------------------------
    // printf("NGS program intd    version: %s     date(ccyy/mm/dd): %s\n", 
    //         PGMVER, PGMDAT); 

    intro( PGMVER, PGMDAT );    // prints intro and disclaimer to stdout

    // ---------------------------------------------------------
    // Select geoid model
    // Experimental has no prompt, yet no error message
    // ---------------------------------------------------------
    iii = 0;
    while (iii == 0) {
        printf("%s\n", dash70);
        printf("\
Which geoid model do you wish to use?\n\n\
   1 = DEFLEC99                 2 = USDOV2009 \n\
   3 = DEFLEC09                 4 = USDOV2012 \n\
   5 = DEFLEC12A  \n\
   6 = DEFLEC12B  \n\n\
  99 = END PROGRAM\n\n\
   -> ");
        strncpy(cinput, "\0", 42);
        fgets( cinput, 40, stdin);
        imodel = atoi(cinput);

        if (imodel == 99) return(0);

        if (imodel >= 1 && imodel <= 6) {  
            ++iii;
        } else {
            fprintf(stderr,"Error: Not a valid response. Try again.\n");
            exit(-1);
        }
    }//~while

    // ---------------------------------------------------------
    // Which directory are the deflection files in?
    // For Unix platform, set the directory location for the user.
    // ---------------------------------------------------------
    getdir_defl(imodel, dirnam);

    // ---------------------------------------------------------
    // Create the list of files that must be opened, and open them.
    // Return with a count of how many files were opened,
    // and a flag of which files are open and which are not.
    // IS_SUBR : run as subroutine: false=0; true=(not zero) (c std notation)
    // ---------------------------------------------------------
    is_subr = 0;
    getgrd_defl( imodel, dirnam, is_subr, &nfiles, 
                   &nxff,       &neff, 
                 vec_xfnames, vec_efnames, 
                 vec_xifp,    vec_eifp );

    // ---------------------------------------------------------
    // Read the headers of all Xi deflection files 
    // which where opened, and store that information.
    // Compute the max lat/lon from the header information.
    // Apply endian correction if required. 
    // ---------------------------------------------------------
    getheaders( vec_xifp, vec_xhdr, nfiles );

    // ---------------------------------------------------------
    // How to input?
    // ---------------------------------------------------------
    iii = 0;
    while (iii == 0) {
        printf("%s\n", dash70);
        printf("\
How would you like to input the data? \n\
    1 = By Keyboard (with prompts) \n\
    2 = By File     (using allowed formats) \n\n\
    -> ");

        strncpy(cinput, "\0", 42);
        fgets(  cinput, 40, stdin);
        iinput = atoi(cinput);

        if ((iinput > 0 && iinput < 3)) {
            ++iii;
        } else {
            fprintf(stderr, "Error: not an option - try again\n");
            exit(-1);
        }
    }

    // ---------------------------------------------------------
    // If using an input file, select format
    // ---------------------------------------------------------
    if (iinput == 2) {
        iii = 0;
        while (iii == 0) {
            printf("\
Which format will you use for input? \n\
    1 = File - Free Format (For Geoid) Type 1 \n\
    2 = File - Free Format (For Geoid) Type 2 \n\
    3 = File - NGS Blue Book Vol 1 (*80* and *86* records) \n\n\
    0 = End Program \n\
   99 = Please explain to me the formats \n\n\
    -> ");

            strncpy(cinput, "\0", 42);
            fgets(  cinput, 40, stdin);
            iform = atoi(cinput);

            if (iform == 0) {
                exit(0);
            } else if (iform == 99) {
                expform();
            }
            else if ((iform > 0 && iform < 4)) {
                ++iii;
            } else {
                fprintf(stderr, "Error: not an option - try again\n");
                exit(-1);
            }
        }
    }

    // ---------------------------------------------------------
    // Select positive longitude convention.
    // DON'T ask this if the input is Blue Book
    // ---------------------------------------------------------
    iii = 0;
    if ((iform == 1 || iform == 2 || iinput == 1)) {
        while (iii == 0) {
            printf("%s\n", dash70);
            printf("\
Which longitude convention will you use? \n\
    1 - Positive EAST \n\
    2 - Positive WEST \n\n\
    -> ");

            strncpy(cinput, "\0", 42);
            fgets(  cinput, 40, stdin);
            ipos = atoi(cinput);

            if (ipos == 1) {
                poseast = 1;     // C-std: Nonzero = true
                ++iii;
            } 
            else if (ipos == 2) {
                poseast = 0;
                ++iii;
            } else {
                fprintf(stderr, "Error: not an option - try again\n");
                exit(-1);
            }
        }
    }

    // ---------------------------------------------------------
    // Open the Input file if necessary
    // ---------------------------------------------------------
    iii = 0;
    if (iinput == 2) {
        while (iii == 0) {
            printf("\n");
            printf("Enter your  input file name   : ");
            strncpy(ifname, "\0", 256);
            fgets(  ifname, 256, stdin);
            trim_c( ifname, 'b' );

            if ((ifp = fopen( ifname, "r" )) == NULL) {
                printf("Error: Cannot find input file: %s\nTry again", ifname);
            } else {
                ++iii;
            }
        }//~while
    }//~if

    // ---------------------------------------------------------
    // Output file? (Forced to 'yes' if there was an input file)
    // ---------------------------------------------------------
    if (iinput == 1) {  // 1 = keyboard
        printf("\n");
        printf("Write output to a file (y/n)? : ");

        strncpy(cinput, "\0", 42);
        fgets(  cinput, 40, stdin);
        trim_c( cinput, 'b' );
        strcpy(ofyn, cinput);
    } else {
        strcpy(ofyn, "y");
    }

    // ---------------------------------------------------------
    // Open Output file if necessary
    // ---------------------------------------------------------
    outfil = 0;       // Cstd:  0 := false   !0 := not false
    iii    = 0;
    if (strcmp(ofyn, "Y") == 0 || strcmp(ofyn, "y") == 0) {
        while (iii == 0) {
            outfil = 1;
            printf("Enter your output file name   : ");

            strncpy(ofname, "\0", 256);
            fgets(  ofname, 256, stdin);
            trim_c( ofname, 'b');

            if ((ofp = fopen(ofname, "w")) == NULL) {
                printf("Error: File exists - try again\n");
            } else {
                ++iii;
            }
        }
    }


    // ======================================================================
    // Now handle the 4 input types
    // ======================================================================

    mem_limit += MEM_STEP;
    vec_data = (DATASET1*) calloc(mem_limit, sizeof(DATASET1));
    if (vec_data == NULL) {
        fprintf(stderr, "Out of system memory - allocation fails\n");
        exit(-1);
    }

    // -----------------------------------------------
    // Input by file, free format type 1
    // -----------------------------------------------
    if (iform == 1) {
        while( fgets(rec_in, 90, ifp) ) {

            // Find the lat/lon value
            // Longitude always returns as 0->360...whether this
            // is positive east or west is fixed a few lines down
            strncpy(text, "\0", 42);
            ff1(rec_in, &xlat, &xlon, text);

            // If the lat/lon values came back as -999, set the 
            // geoid value to -999 and skip the interpolation 
            if (xlat == -999. || xlon == -999.) {
                valx = (double)-999.;
                vale = (double)-999.;
                valh = (double)-999.;
                continue;
            }

            // Force longitude to be positive East
            if (poseast != 1) {
                xlon = 360. - xlon;
            }
            // Now have the lat/lon pair from input file type 2

            if (xlat > xn) xn = xlat;
            if (xlat < xs) xs = xlat;
            if (xlon > xe) xe = xlon;    // assume East Lon
            if (xlon < xw) xw = xlon;

            DATASET1 thisSet;
            strncpy(thisSet.text, "\0", 42);
            thisSet.lat = xlat;
            thisSet.lon = xlon;
            strcpy(thisSet.text, text);
            vec_data[kount] = thisSet;
            ++kount;
            if (kount >= mem_limit) {
                mem_limit += MEM_STEP;
            vec_data = (DATASET1*)realloc(vec_data, mem_limit*sizeof(DATASET1));
                if (vec_data == NULL) {
                    printf("Out of system memory - allocation fails\n");
                }
            }
        }//~while
    }//~if (iform = 1)

    // -----------------------------------------------
    // Input file, free format type 2
    // -----------------------------------------------
    else if (iform == 2) {
        while( fgets(rec_in, 90, ifp) ) {

            // Find the lat/lon value
            // Longitude always returns as 0->360...whether this
            // is positive east or west is fixed a few lines down
            strncpy(text, "\0", 42);
            ff2(rec_in, &xlat, &xlon, text);

            // If the lat/lon values came back as -999, set the 
            // deflection values to -999 and skip the interpolation 
            if (xlat == -999. || xlon == -999.) {
                valx = (double) -999.;
                vale = (double) -999.;
                valh = (double) -999.;
                continue;
            }

            // Force longitude to be positive East
            if (poseast != 1) {
                xlon = 360. - xlon;
            }
            // Now have the lat/lon pair from input file type 1

            if (xlat > xn) xn = xlat;
            if (xlat < xs) xs = xlat;
            if (xlon > xe) xe = xlon;    // assume East Lon
            if (xlon < xw) xw = xlon;

            DATASET1 thisSet;
            strncpy(thisSet.text, "\0", 42);
            thisSet.lat = xlat;
            thisSet.lon = xlon;
            strcpy(thisSet.text, text);
            vec_data[kount] = thisSet;
            ++kount;
            if (kount >= mem_limit) {
                mem_limit += MEM_STEP;
            vec_data = (DATASET1*)realloc(vec_data, mem_limit*sizeof(DATASET1));
                if (vec_data == NULL) {
                    printf("Out of system memory - allocation fails\n");
                }
            }

        }//~while
    }//~if (iform = 2)

    // -----------------------------------------------
    // Input by file, horizontal bluebook
    // Function contains all processes. 
    // -----------------------------------------------
    else if (iform == 3) {
        run_bbk( ifp, ofp, vec_xifp, vec_eifp, vec_xhdr, vec_xfnames, nfiles, imodel );

        fclose(ifp);
        fclose(ofp);
        return(0);

    }//~(iform == 3) bluebook

    // -----------------------------------------------
    // Input by prompts
    // -----------------------------------------------
    else if (iinput == 1) {
        jj = 0;
        do {
            printf("\n");
            printf("What is the name of this point?           : ");
            strncpy(cinput, "\0", 42);
            fgets(  cinput, 40, stdin);
            strncpy(text, "\0", 42);
            strncpy( text, cinput, (strlen(cinput)-1));

            ii = 0;
            while (ii == 0) {
                printf("\n");
                printf("What is the North Latitude of this point? : ");
                strncpy(cinput, "\0", 42);
                fgets(  cinput, 40, stdin);
                trim_c( cinput, 'b');
                xlat = c2v(cinput, 1);
                if (fabs(xlat + 999) < 0.001) {
                    printf("Error(501): Bad Latitude ... try again\n");
                } else {
                    ++ii;
                }
            }

            ii = 0;
            while (ii == 0) {
                if (poseast == 1) {
                    printf("\n");
                    printf("What is the East Longitude of this point? : ");
                } else {
                    printf("\n");
                    printf("What is the West Longitude of this point? : ");
                }
                strncpy(cinput, "\0", 42);
                fgets(  cinput, 40, stdin);
                trim_c( cinput, 'b');
                xlon = c2v(cinput, 2);
                if (fabs(xlon + 999) < 0.001) {
                    printf("Error(501): Bad Longitude ... try again\n");
                } else {
                    ++ii;
                }
                // Force longitude to be positive East
                if (poseast != 1) {
                    xlon = 360. - xlon;
                }

            }

            if (xlat > xn) xn = xlat;
            if (xlat < xs) xs = xlat;
            if (xlon > xe) xe = xlon;    // assume East Lon
            if (xlon < xw) xw = xlon;

            // load lat/lon struct to 1-D array
            DATASET1 thisSet;
            strncpy(thisSet.text, "\0", 42);
            thisSet.lat  = xlat;
            thisSet.lon  = xlon;
            strcpy(thisSet.text, text);
            vec_data[kount] = thisSet;
            ++kount;
            if (kount >= mem_limit) {
                mem_limit += MEM_STEP;
                vec_data = 
                    (DATASET1*)realloc(vec_data, mem_limit * sizeof(DATASET1));
                if (vec_data == NULL) {
                    printf("Out of system memory - allocation fails\n");
                }
            }

            printf("\n");
            printf("Enter another station? (y/n): ");
            strncpy(cinput, "\0",  42);
            fgets(  cinput, 40, stdin);
            trim_c( cinput, 'b');

            if( (strncmp(cinput, "y", 1) == 0)
            ||  (strncmp(cinput, "Y", 1) == 0) ) {
                ;  // do nothing
            } else {
                ++jj;
            }
        } while (jj == 0);

    }//~(iinput == 1) keyboard prompt

    // ---------------------------------------------
    // Now have the lat/lon pair(s) in a data vector
    // ---------------------------------------------


    // ===== COMMON TO NON-BLUEBOOK INPUT TYPES ============

    // ===== INTERPOLATE DEFLECTION VALUE ==================

    // iterate thru the array -----
    for (ii = 0; ii < kount; ++ii) {
        xlat = vec_data[ii].lat;
        xlon = vec_data[ii].lon;

        if (DEBUG != 0) {
            printf("kount = %d  ii = %d    xlat  = %lf   xlon = %lf\n", 
                    kount, ii, xlat, xlon);
        }

        // If the lat/lon values came back as -999, set the 
        // deflection values to -999 and skip the interpolation 
        if (xlat == -999. || xlon == -999.) {
            valx = (double) -999.;
            vale = (double) -999.;
            valh = (double) -999.;
            continue;
        } else {

            // Find which deflec file to use, based on the lat/lon input
            kk = which_defl(xlat, xlon, nfiles, kk, imodel, vec_xfnames, vec_xhdr, vec_xifp );

            if (DEBUG != 0) { printf("kk = %d \n", kk); }

            // If the point isn't in any of our grid areas, set to -999
            if (kk == -1) {
                valx = (double) -999.;
                vale = (double) -999.;
                valh = (double) -999.;

            // Otherwise, do the interpolation
            } else {
                if (DEBUG != 0) {
                    printf("xlat = %lf  xlon = %lf  kk = %d\n ", xlat,xlon,kk);
                }

                // Interpolate Xi
                valx = interg(xlat, xlon, vec_xhdr, vec_xifp, kk );
                if (DEBUG != 0) { printf("Xi  = %lf \n", valx); }

                // Interpolate Eta
                vale = interg(xlat, xlon, vec_xhdr, vec_eifp, kk );
                if (DEBUG != 0) { printf("Eta = %lf \n", vale); }

                // Calculate the Horizontal Laplace correction
                valh = -tan(xlat * M_PI / 180.0) * vale;
                if (DEBUG != 0) { printf("Lap = %lf \n", valh); }

                ++keep;

                avex += valx;
                rmsx += valx * valx;
                if (valx < minx) {
                    minx    = valx;
                    minxlat = xlat;
                    minxlon = xlon;
                }
                if (valx > maxx) {
                    maxx    = valx;
                    maxxlat = xlat;
                    maxxlon = xlon;
                }

                avee += vale;
                rmse += vale * vale;
                if (vale < mine) {
                    mine    = vale;
                    minelat = xlat;
                    minelon = xlon;
                }
                if (vale > maxe) {
                    maxe    = vale;
                    maxelat = xlat;
                    maxelon = xlon;
                }

                aveh += valh;
                rmsh += valh * valh;
                if (valh < minh) {
                    minh    = valh;
                    minhlat = xlat;
                    minhlon = xlon;
                }
                if (valh > maxh) {
                    maxh    = valh;
                    maxhlat = xlat;
                    maxhlon = xlon;
                }
            }

            if (strcmp(ofyn, "Y") == 0 || strcmp(ofyn, "y") == 0) {
                if (iform == 1) 
                    ff1out(ofp, vec_data[ii], valx,vale,valh, poseast);
                else 
                if (iform == 2)
                    ff2out(ofp, vec_data[ii], valx,vale,valh, poseast);
                // else if (iform == 3)  // bluebook - do nothing -----

                if (iinput == 1)
                    ff1out(ofp, vec_data[ii], valx,vale,valh, poseast);
            }

            if (iinput == 1)
                ff4out(vec_data[ii], valx,vale,valh, poseast);

        }//~if(xlat == -999. || xlon == -999.)

    }//~for(ii)

    free( (void*)vec_data );

    // Finally, write out the record to screen and possibly to an output file

    // ===== OUTPUT =======================
    // write output to screen and possibly to an output file
    // go get another input dataset


    // =============================================================
    // This is for input file format = 1
    // When finished, give a little report to the screen and end program.
    // 
    avex = avex/keep;
    avee = avee/keep;
    aveh = aveh/keep;
    rmsx = sqrt(rmsx/keep);
    rmse = sqrt(rmse/keep);
    rmsh = sqrt(rmsh/keep);

    if (keep > 1) {
        fact = (double)(keep) / (double)(keep-1);
        stdx = sqrt( fact * fabs(rmsx*rmsx - avex*avex) );
        stde = sqrt( fact * fabs(rmse*rmse - avee*avee) );
        stdh = sqrt( fact * fabs(rmsh*rmsh - aveh*aveh) );
    } else {
        stdx = 0;
        stde = 0;
        stdh = 0;
    }

    printf("\n%s\n", dash70);


    if (poseast) {      // East Longitude
        printf("\
FINAL REPORT: \n\
 Points Input / Points Kept  : %d   %d \n\
 North/South/West/East Bounds: %10.6lf  %10.6lf   %10.6lf  %10.6lf\n\n\
 %s\n\
 %s\n\
 %s\n\
 Xi  %6.2lf %6.2lf  %6.2lf %10.6lf %10.6lf %6.2lf  %10.6lf %10.6lf\n\
 Eta %6.2lf %6.2lf  %6.2lf %10.6lf %10.6lf %6.2lf  %10.6lf %10.6lf\n\
 Lap %6.2lf %6.2lf  %6.2lf %10.6lf %10.6lf %6.2lf  %10.6lf %10.6lf\n\n", 
        kount, keep, 
        xn, xs, xw, xe, 
        final_header1, 
        final_header2, 
        final_header3, 
        avex, stdx, minx, minxlat, minxlon, maxx, maxxlat, maxxlon, 
        avee, stde, mine, minelat, minelon, maxe, maxelat, maxelon, 
        aveh, stdh, minh, minhlat, minhlon, maxh, maxhlat, maxhlon );

    } else {            // West Longitude

        printf("\
 FINAL REPORT: \n\
 Points Input / Points Kept  : %d   %d \n\
 North/South/West/East Bounds: %10.6lf  %10.6lf   %10.6lf  %10.6lf\n\n\
 %s\n\
 %s\n\
 %s\n\
 Xi  %6.2lf %6.2lf  %6.2lf %10.6lf %10.6lf %6.2lf %10.6lf %10.6lf\n\
 Eta %6.2lf %6.2lf  %6.2lf %10.6lf %10.6lf %6.2lf %10.6lf %10.6lf\n\
 Lap %6.2lf %6.2lf  %6.2lf %10.6lf %10.6lf %6.2lf %10.6lf %10.6lf\n\n", 
        kount, keep, 
        xn, xs, (360.0 - xw), (360.0 - xe), 
        final_header1, 
        final_header2, 
        final_header3, 
        avex, stdx, minx, minxlat, 360.0-minxlon, maxx, maxxlat, 360.0-maxxlon, 
        avee, stde, mine, minelat, 360.0-minelon, maxe, maxelat, 360.0-maxelon, 
        aveh, stdh, minh, minhlat, 360.0-minhlon, maxh, maxhlat, 360.0-maxhlon);
    }


    if (iinput == 2) {  // input by file
        fclose(ifp);
    }
    if (strcmp(ofyn, "Y") == 0 || strcmp(ofyn, "y") == 0) {
        fclose(ofp);
    }

    return(0);

}//~intd

