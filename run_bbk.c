// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: run_bbk.c 82093 2015-01-26 15:26:43Z bruce.tran $	20$Date: 2010/02/17 18:24:48 $ NGS";
static const int  DEBUG = 0;           // diagnostics print if != 0
static char final_header1[]= "Basic Statistics: ";
static char final_header2[]=
"Val   AVE    Sigma  Minim. Lat of Min Lon of Min  Maxim. Lat of Max Lon of Max";
static char final_header3[]=
"     (sec)   (sec)  (sec)   (dec deg)  (dec deg)  (sec)   (dec deg)  (dec deg)";

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>     // M_PI

// ----- classes, structs, types -------------------------------------
#include "grid_header.h"

// ----- functions ---------------------------------------------------
#include "run_bbk.h"
#include "getheaders.h"
#include "bb80ll.h"
#include "interg.h"
#include "which_defl.h"


void run_bbk(FILE* ifp, FILE* ofp, 
             FILE** vec_xifp, FILE** vec_eifp, GRID_HEADER vec_hdr[50], 
             char vec_xfnames[50][256], int nfiles, int imodel) {
/*******************************************************************************
*   in - ifp      : input  bluebook file
*   in - ofp      : output bluebook file, optional
*   in - vec_xifp : vector of file ptrs, pointing to Xi  data files
*   in - vec_eifp : vector of file ptrs, pointing to Eta data files
*   in - vec_hdr  : vector of header data (same for Xi and Eta data files)
*   in - nfiles   : 
*   in - imodel   : user specified geoid model
*  INPUT FILE, BLUE BOOK FORMAT
* Spin through the blue book file, stopping at each * *80* record, 
* and while there, look forward to the next records to see 
* if there is an *86* * record.  
* If so,  overwrite the geoid field in it with an interpolated value.
* If not, create an *86* record.
*
* Read one line, and if it is NOT an *84* record, 
* replicate it immediately into the output file (if we're using one).
* If the record is not an *80* record, 
* come back and get a new record.
*******************************************************************************/
    int jjj        = 0;
    int kount      = 0;
    int keep       = 0;
    double xn      =   -90.0;
    double xs      =    90.0;
    double xw      =   360.0;
    double xe      =     0.0;
    double minx    =  1000.0;
    double mine    =  1000.0;
    double minh    =  1000.0;
    double maxx    = -1000.0;
    double maxe    = -1000.0;
    double maxh    = -1000.0;
    double avex    = 0.0;
    double avee    = 0.0;
    double aveh    = 0.0;
    double stdx    = 0.0;
    double stde    = 0.0;
    double stdh    = 0.0;
    double rmsx    = 0.0;
    double rmse    = 0.0;
    double rmsh    = 0.0;
    double minlatx =   90.0;
    double minlate =   90.0;
    double minlath =   90.0;

    double maxlatx =  -90.0;
    double maxlate =  -90.0;
    double maxlath =  -90.0;

    double minlonx =  360.0;
    double minlone =  360.0;
    double minlonh =  360.0;

    double maxlonx =    0.0;
    double maxlone =    0.0;
    double maxlonh =    0.0;
    double fact;
    char   card[90];
    char   card2[90];
    char   card85[90];
    double xlat;           // lat of point in question - decdeg
    double xlon;           // lon of point in question - decdeg
    double valx;           // interpolated value, Xi
    double vale;           // interpolated value, Eta
    double valh;           // calculated value, Laplace
    char   dirxi[2];       // Xi  hemisphere
    char   direta[2];      // Eta hemisphere
    int    ixi     = 0;    // Xi   for output
    int    ieta    = 0;    // Eta  for output
    int    poseast = 0;    // longitude Hemisphere toggle
    int    kk      = 0;
    char   bbcode;

    strncpy(card,   "\0", 90);
    strncpy(card2,  "\0", 90);
    strncpy(card85, "\0", 90);
    strncpy(dirxi,  "\0",  2);
    strncpy(direta, "\0",  2);


    // Spin through the blue book file, stopping at each *80* record,
    // and while there, look forward to the next records 
    // to see if there is an *85* record. 
    // If so, overwrite the geoid field in it with an interpolated value.
    // If not, create an *85* record.

    while( fgets(card, 90, ifp) ) {

        // Read one line, and if it is NOT an *84* record,
        // replicate it immediately into the output file (if we're using one).
        // If the record is not an *80* record, come back and get a new record.

        if( (strncmp(&card[7], "83", 2) == 0)
        ||  (strncmp(&card[7], "84", 2) == 0)) {
            continue;
        } else 
        if (ofp) {
            fprintf(ofp, "%s", card);
        }

        if (strncmp(&card[7], "80", 2) != 0) {
            continue;
        } else {
            // Arriving here, we have an *80* record as 'card'
            // Get the lat/lon value from the *80* record

            bb80ll(card, &xlat, &xlon);

            if (DEBUG != 0) {
                printf("card = \n%s\n", card);
                printf("xlat = %lf  xlon = %lf\n", xlat, xlon);
                printf("card[68] = %c \n", card[68]);
            }

            if (card[68] == 'W') { 
                 poseast = 0;     // 0 := false
                 xlon = 360.0 - xlon;
            }

            if (DEBUG != 0) {
                printf("xlat = %lf  xlon = %lf\n", xlat, xlon);
            }

            // If the xlat/xlon values are bad, don't do an interpolation.
            // Just move on to the next record...any existing *86*
            // record associated with this erroneous *80* record will
            // remain unmodified.
            if (xlat == -999.0 || xlon == -999.0)  continue;

            // Arriving here, the *80* record has a good lat/lon value
            ++kount;

            // The associated *85* record, if it exists, must
            // come AFTER the *80* record we're currently looking at, 
            // and the ONLY thing allowed between the 80 and 85 record 
            // are 81 and 82 records (83 and 84 records will be deleted).

            jjj = 0;
            while (jjj == 0) {
                strncpy(card2, "\0", 90);
                fgets(card2, 90, ifp);

                if (DEBUG != 0)  printf("card2 = \n%s\n", card2);

                // POSSIBILITY 1: We find an *83* or *84* record
                // ACTION: Do not allow it to go into the output file, and
                //         go back for another "card2" value

                if( (strncmp(&card2[7], "83", 2) == 0) 
                ||  (strncmp(&card2[7], "84", 2) == 0) ) {
                    continue;
                }

                // POSSIBILITY 2: We find an *81* or *82* record
                // ACTION: Replicate this record into the output file
                //         and go back for another "card2" value

                if( (strncmp(&card2[7], "81", 2) == 0) 
                ||  (strncmp(&card2[7], "82", 2) == 0) ) {
                    fprintf(ofp, "%s", card2);
                    continue;
                }

                // POSSIBILITY 3: We find an *85* record
                // ACTION: a) Modify it if it's the right *85* record
                //         b) If it is the wrong *85* record, 
                //            delete it and make a NEW *85* record, and
                //            then go back and get another "card2" value

                if( (strncmp(&card2[7], "85", 2)) == 0)  {

                    if( (strncmp(&card[10], &card2[10], 4)) == 0)  {

                        if (DEBUG != 0) { printf("match 80 and 85"); }

                        // Arriving here, we've found the *85* record 
                        // associated with the *80* record

                        // Find which geoid file to use, based on lat/lon 
                        kk = which_defl(xlat, xlon, nfiles, kk, imodel, vec_xfnames,
                               vec_hdr, vec_xifp );

                        if (DEBUG != 0)  printf("After which_defl kk = %d\n", kk);

                        if (kk == -1) {
                            // If not in any grid area, set to -999
                            valx = -999;
                            vale = -999;
                            valh = -999;
                        } else {
                            // Otherwise, do the interpolation

                            // Interpolate Xi
                            valx = interg(xlat, xlon, vec_hdr, vec_xifp, kk);
                            if (DEBUG != 0)  printf("Xi  = %lf \n", valx);

                            // Interpolate Eta
                            vale = interg(xlat, xlon, vec_hdr, vec_eifp, kk);
                            if (DEBUG != 0)  printf("Eta = %lf \n", vale);

                            // Calculate the Horizontal Laplace correction
                            valh = -tan(xlat * M_PI / 180.0) * vale;
                            if (DEBUG != 0) { printf("Lap = %lf \n", valh); }

                            ++keep;
                            avex += valx;
                            avee += vale;
                            aveh += valh;
                            rmsx  += (valx*valx);
                            rmse  += (vale*vale);
                            rmsh  += (valh*valh);

                            if (xlat > xn)  xn = xlat;
                            if (xlat < xs)  xs = xlat;
                            if (xlon > xe)  xe = xlon;
                            if (xlon < xw)  xw = xlon;

                            if (valx < minx)  {
                                minx    = valx;
                                minlatx = xlat;
                                minlonx = xlon;
                            }
                            if (valx > maxx) {
                                maxx    = valx;
                                maxlatx = xlat;
                                maxlonx = xlon;
                            }

                            if (vale < minx)  {
                                mine    = vale;
                                minlate = xlat;
                                minlone = xlon;
                            }
                            if (vale > maxe) {
                                maxe    = vale;
                                maxlate = xlat;
                                maxlone = xlon;
                            }

                            if (valh < minh)  {
                                minh    = valh;
                                minlath = xlat;
                                minlonh = xlon;
                            }
                            if (valh > maxh) {
                                maxh    = valh;
                                maxlath = xlat;
                                maxlonh = xlon;
                            }

                            // Modify the  *85* record and write it out
                            // Find the right deflection code
                            if(imodel == 1) {
                                strncpy( &card2[20], "DEFLEC99", 8);
                                bbcode = 'A';
                            }
                            else if(imodel == 3) {
                                strncpy( &card2[20], "DEFLEC09", 8);
                                bbcode = 'E';
                            } 
                            else if(imodel == 5) {
                                strncpy( &card2[20], "DEFLEC12A", 9);
                                bbcode = 'W';
                            }
                            else if(imodel == 6) {
                                strncpy( &card2[20], "DEFLEC12B", 9);
                                bbcode = 'X';
                            }
                            card2[62] = bbcode;
                            // strcpy(&card2[30], b33);

                            ixi  = (int)( floor(valx*100.0 + 0.5) );
                            ieta = (int)( floor(vale*100.0 + 0.5) );

                            if (ofp) {
                                if (ixi  >= 0.0)  strcpy(dirxi,  "N");
                                else              strcpy(dirxi,  "S");
                                if (ieta >= 0.0)  strcpy(direta, "E");
                                else              strcpy(direta, "W");

                                fprintf(ofp, "%-62s%5d%1s000%5d%1s000\n",
                                    card2, abs(ixi), dirxi,
                                    abs(ieta), direta);
                            }
                            ++jjj;
                            // goto L137;
                            break;  // go back, get "card"

                        }// if (kk == -1)

                    }//~if( strncmp(card2[11], card[11], 4) == 0 )

                    // If this is NOT the associated *85* record, delete
                    // it and make a new *85* record
                    else {
                        if (DEBUG != 0) { printf("NOT match 80 and 85"); }

                        // Find geoid file to use, based on input lat/lon 
                        kk = which_defl(xlat, xlon, nfiles, kk, imodel, vec_xfnames,
                               vec_hdr, vec_xifp );

                        if (kk == -1) {
                            // If not in any grid area, set = -999
                            valx = -999;
                            vale = -999;
                            valh = -999;
                        } else {
                            // Otherwise, do the interpolation
                            // Interpolate Xi
                            valx = interg(xlat, xlon, vec_hdr, vec_xifp, kk );
                            if (DEBUG != 0) { printf("Xi  = %lf \n", valx); }

                            // Interpolate Eta
                            vale = interg(xlat, xlon, vec_hdr, vec_eifp, kk );
                            if (DEBUG != 0) { printf("Eta = %lf \n", vale); }

                            // Calculate the Horizontal Laplace correction
                            valh = -tan(xlat * M_PI / 180.0) * vale;
                            if (DEBUG != 0) { printf("Lap = %lf \n", valh); }

                            keep +=   1;
                            avex += valx;
                            avee += vale;
                            aveh += valh;
                            rmsx += (valx*valx);
                            rmse += (vale*vale);
                            rmsh += (valh*valh);
                            if (xlat > xn)  xn = xlat;
                            if (xlat < xs)  xs = xlat;
                            if (xlon > xe)  xe = xlon;
                            if (xlon < xw)  xw = xlon;

                            if (valx < minx)  {
                                minx    = valx;
                                minlatx = xlat;
                                minlonx = xlon;
                            }
                            if (valx > maxx) {
                                maxx    = valx;
                                maxlatx = xlat;
                                maxlonx = xlon;
                            }

                            if (vale < mine)  {
                                mine    = vale;
                                minlate = xlat;
                                minlone = xlon;
                            }
                            if (vale > maxe) {
                                maxe    = vale;
                                maxlate = xlat;
                                maxlone = xlon;
                            }

                            if (valh < minh)  {
                                minh    = valh;
                                minlath = xlat;
                                minlonh = xlon;
                            }
                            if (valh > maxh) {
                                maxh    = valh;
                                maxlath = xlat;
                                maxlonh = xlon;
                            }

                            // Make a NEW *85* record and write it out
                            strncpy(card85, "\0",     90);
                            strcpy( card85, "      *85*");
                            strncat(card85, &card[10], 4);

                            if (imodel == 1) {
                                strcat(card85, "      DEFLEC99");
                                bbcode = 'A';
                            } 
                            else if (imodel == 3) {
                                strcat(card85, "      DEFLEC09");
                                bbcode = 'E';
                            } 
                            else if (imodel == 5) {
                                strcat(card85, "      DEFLEC12");
                                bbcode = 'W';
                            }

                            ixi  = (int)(fabs(valx)*100.0 + 0.5);
                            ieta = (int)(fabs(vale)*100.0 + 0.5);

                            if (ofp) {
                                if (valx >= 0.0)  strcpy(dirxi,  "N");
                                else              strcpy(dirxi,  "S");
                                if (vale >= 0.0)  strcpy(direta, "E");
                                else              strcpy(direta, "W");

                                fprintf(ofp, "%-61s%c%5d%1s000%5d%1s000\n",
                                    card85, bbcode, abs(ixi), dirxi,
                                    abs(ieta), direta);
                            }

                            // Go back and look for the next "card" value
                            // Any *86* records which follow this *85* record
                            // will be passed into the output file

                            ++jjj;
                            break;

                        }//~if (kk == -1) {

                    }//~if (card2.compare(11,4, card,11,4) == 0)

                    strncpy(card2, "\0", 90);
                }//~if ( (strncmp(&card2[7], "85", 2)) == 0) {


                // POSSIBILITY 4: We find something besides an 81,82,
                //                83, 84 or 85 record
                // ACTION: Create a new *85* based on the previous *80* record,
                //         print the new 85 record, then
                //         print the card2 record.
                //   was:  and then transfer the new 'card2' into 'card' and
                //   was:  go back just after the 'read' statement for 'card'

                if( (strncmp(&card2[7], "81", 2) != 0)
                &&  (strncmp(&card2[7], "82", 2) != 0)
                &&  (strncmp(&card2[7], "83", 2) != 0)
                &&  (strncmp(&card2[7], "84", 2) != 0)
                &&  (strncmp(&card2[7], "85", 2) != 0) ) {

                    // Find geoid file to use, based on input lat/lon 
                    kk = which_defl(xlat, xlon, nfiles, kk,imodel,vec_xfnames,vec_hdr,vec_xifp);

                    if (kk == -1) {
                        // If not in any of our grid areas, set to -999
                        valx = -999;
                        vale = -999;
                        valh = -999;
                    } else {
                        // Otherwise, do the interpolation

                        // Interpolate Xi
                        valx = interg(xlat, xlon, vec_hdr, vec_xifp, kk );
                        if (DEBUG != 0)  printf("Xi  = %lf \n", valx);

                        // Interpolate Eta
                        vale = interg(xlat, xlon, vec_hdr, vec_eifp, kk );
                        if (DEBUG != 0)  printf("Eta = %lf \n", vale);

                        // Calculate the Horizontal Laplace correction
                        valh = -tan(xlat * M_PI / 180.0) * vale;
                        if (DEBUG != 0) { printf("Lap = %lf \n", valh); }

                        ++keep;
                        avex  += valx;
                        avee  += vale;
                        aveh  += valh;
                        rmsx  += valx*valx;
                        rmse  += vale*vale;
                        rmsh  += valh*valh;
                        if (xlat > xn)  xn = xlat;
                        if (xlat < xs)  xs = xlat;
                        if (xlon > xe)  xe = xlon;
                        if (xlon < xw)  xw = xlon;

                        if (valx < minx) {
                            minx    = valx;
                            minlatx = xlat;
                            minlonx = xlon;
                        }
                        if (valx > maxx) {
                            maxx    = valx;
                            maxlatx = xlat;
                            maxlonx = xlon;
                        }

                        if (vale < mine) {
                            mine    = vale;
                            minlate = xlat;
                            minlone = xlon;
                        }
                        if (vale > maxe) {
                            maxe    = vale;
                            maxlate = xlat;
                            maxlone = xlon;
                        }

                        if (valh < minh) {
                            minh    = valh;
                            minlath = xlat;
                            minlonh = xlon;
                        }
                        if (valh > maxh) {
                            maxh    = valh;
                            maxlath = xlat;
                            maxlonh = xlon;
                        }
                    }

                    // Make a NEW *85* record
                    strncpy(card85, "\0",     90);
                    strcpy( card85, "      *85*");
                    strncat(card85, &card[10], 4);
                    if (imodel == 1) {
                        strcat(card85, "      DEFLEC99");
                        bbcode = 'A';
                    } 
                    else if (imodel == 3) {
                        strcat(card85, "      DEFLEC09");
                        bbcode = 'E';
                    } 
                    else if (imodel == 5) {
                        strcat(card85, "      DEFLEC12");
                        bbcode = 'W';
                    }

                    ixi  = (int)(fabs(valx)*100.0 + 0.5);
                    ieta = (int)(fabs(vale)*100.0 + 0.5);

                    // Print the NEW *85* record
                    if (ofp) {
                        if (valx >= 0.0)  strcpy(dirxi,  "N");
                        else              strcpy(dirxi,  "S");
                        if (vale >= 0.0)  strcpy(direta, "E");
                        else              strcpy(direta, "W");

                        fprintf(ofp, "%-61s%c%5d%1s000%5d%1s000\n",
                            card85, bbcode, abs(ixi), dirxi,
                            abs(ieta), direta);
                    }

                    // Put the latest 'card85' record into 'card' and go back to
                    // the top to search...if 'card2' is an *86* record, then
                    // it will be passed unchanged into the output file.

                    // Print the card2 record
                    fprintf(ofp, "%s", card2);

                    // strcpy(card, card2);
                    ++jjj;
                    break;

                }//~if ((card2.substr(7,2) != "81")


                // POSSIBILITY 5: We find the EOF
                // ACTION: Create a new *86* based on the previous *80* record
                //         and then go to the 'end report' phase

                // Find geoid file to use, based on input lat/lon 
                kk = which_defl(xlat, xlon, nfiles, kk, imodel, vec_xfnames,vec_hdr,vec_xifp);

                // If the point isn't in any of our grid areas, set to -999
                if (kk == -1) {
                    valx = -999;
                    vale = -999;
                    valh = -999;
                } else {
                    // Otherwise, do the interpolation

                    // Interpolate Xi
                    valx = interg(xlat, xlon, vec_hdr, vec_xifp, kk );
                    if (DEBUG != 0) { printf("Xi  = %lf \n", valx); }

                    // Interpolate Eta
                    vale = interg(xlat, xlon, vec_hdr, vec_eifp, kk );
                    if (DEBUG != 0) { printf("Eta = %lf \n", vale); }

                    // Calculate the Horizontal Laplace correction
                    valh = -tan(xlat * M_PI / 180.0) * vale;
                    if (DEBUG != 0) { printf("Lap = %lf \n", valh); }

                    keep +=   1;
                    avex  += valx;
                    avee  += vale;
                    aveh  += valh;
                    rmsx  += (valx*valx);
                    rmse  += (vale*vale);
                    rmsh  += (valh*valh);
                    if (xlat > xn)  xn = xlat;
                    if (xlat < xs)  xs = xlat;
                    if (xlon > xe)  xe = xlon;
                    if (xlon < xw)  xw = xlon;

                    if (valx < minx) {
                        minx    = valx;
                        minlatx = xlat;
                        minlonx = xlon;
                    }
                    if (valx > maxx) {
                        maxx    = valx;
                        maxlatx = xlat;
                        maxlonx = xlon;
                    }

                    if (vale < mine) {
                        mine    = vale;
                        minlate = xlat;
                        minlone = xlon;
                    }
                    if (vale > maxe) {
                        maxe    = vale;
                        maxlate = xlat;
                        maxlone = xlon;
                    }

                    if (valh < minh) {
                        minh    = valh;
                        minlath = xlat;
                        minlonh = xlon;
                    }
                    if (valh > maxh) {
                        maxh    = valh;
                        maxlath = xlat;
                        maxlonh = xlon;
                    }
                }

                // Make a NEW *85* record
                strncpy(card85, "\0",     90);
                strcpy( card85, "      *85*");
                strncat(card85, &card[10], 4);
                if (imodel == 1) {
                    strcat(card85, "      DEFLEC99");
                    bbcode = 'A';
                } 
                else if (imodel == 3) {
                    strcat(card85, "      DEFLEC09");
                    bbcode = 'E';
                } 
                else if (imodel == 5) {
                    strcat(card85, "      DEFLEC12");
                    bbcode = 'W';
                }

                ixi  = (int)(fabs(valx)*100.0 + 0.5);
                ieta = (int)(fabs(vale)*100.0 + 0.5);

                if (ofp) {
                    if (valx >= 0.0)  strcpy(dirxi,  "N");
                    else              strcpy(dirxi,  "S");
                    if (vale >= 0.0)  strcpy(direta, "E");
                    else              strcpy(direta, "W");

                    fprintf(ofp, "%-61s%c%5d%1s000%5d%1s000\n",
                        &card2[1], bbcode, abs(ixi), dirxi,
                        abs(ieta), direta);
                }

                strncpy(card,  "\0", 90);
                strncpy(card2, "\0", 90);

            }//~while (jjj == 0)

        }//~if ( card.compare(7, 2, "80") != 0)

    }//~while( fgets(card, 80, ifp) ) {



    // When finished, give a little report to the screen and end program.

    avex /= keep;
    avee /= keep;
    aveh /= keep;
    rmsx = sqrt(rmsx/keep);
    rmse = sqrt(rmse/keep);
    rmsh = sqrt(rmsh/keep);
    if (keep > 1) {
        fact = ((double)keep) / ((double)(keep-1));
        stdx = sqrt( fact * ((rmsx*rmsx) - (avex*avex)) );
        stde = sqrt( fact * ((rmse*rmse) - (avee*avee)) );
        stdh = sqrt( fact * ((rmsh*rmsh) - (aveh*aveh)) );
    } else {
        stdx = 0.0;
        stde = 0.0;
        stdh = 0.0;
    }

    if (poseast) {      // East Longitude
        printf("\n\
FINAL REPORT:   \n\
 Points Input / Points Kept  : %d   %d \n\
 North/South/West/East Bounds: %10.6lf  %10.6lf   %10.6lf  %10.6lf\n\
\n\
%s\n\
%s\n\
%s\n\
 Xi   %6.2lf  %5.2lf %5.2lf %10.6lf  %10.6lf  %6.2lf  %10.6lf  %10.6lf\n\
 Eta  %6.2lf  %5.2lf %5.2lf %10.6lf  %10.6lf  %6.2lf  %10.6lf  %10.6lf\n\
 Lap  %6.2lf  %5.2lf %5.2lf %10.6lf  %10.6lf  %6.2lf  %10.6lf  %10.6lf\n\n",
        kount, keep,
        xn, xs, xw, xe,
        final_header1,
        final_header2,
        final_header3,
        avex, stdx, minx, minlatx, minlonx, maxx, maxlatx, maxlonx,
        avee, stde, mine, minlate, minlone, maxe, maxlate, maxlone,
        aveh, stdh, minh, minlath, minlonh, maxh, maxlath, maxlonh);

    } else {            // West Longitude

        printf("\n\
FINAL REPORT:   \n\
 Points Input / Points Kept  : %d   %d \n\
 North/South/West/East Bounds: %10.6lf  %10.6lf   %10.6lf  %10.6lf\n\
\n\
 %s\n\
 %s\n\
 %s\n\
 Xi   %6.2lf  %5.2lf %5.2lf %10.6lf  %10.6lf  %6.2lf  %10.6lf  %10.6lf\n\
 Eta  %6.2lf  %5.2lf %5.2lf %10.6lf  %10.6lf  %6.2lf  %10.6lf  %10.6lf\n\
 Lap  %6.2lf  %5.2lf %5.2lf %10.6lf  %10.6lf  %6.2lf  %10.6lf  %10.6lf\n\n",
        kount, keep,
        xn, xs, xw, xe,
        final_header1,
        final_header2,
        final_header3,
        avex, stdx, minx, minlatx, 360.0-minlonx, maxx, maxlatx, 360.0-maxlonx,
        avee, stde, mine, minlate, 360.0-minlone, maxe, maxlate, 360.0-maxlone,
        aveh, stdh, minh, minlath, 360.0-minlonh, maxh, maxlath, 360.0-maxlonh);
    }

    return;

}//~run_bbk

