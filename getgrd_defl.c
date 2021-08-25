// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: getgrd_defl.c 82040 2015-01-22 17:53:15Z bruce.tran $	20$Date: 2010/03/24 15:23:06 $ NGS";

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ----- classes, structures, types ----------------------------------
// ----- functions ---------------------------------------------------
#include "getgrd_defl.h"
#include "trim_c.h"


void getgrd_defl(int  imodel, char* dirnam, int is_subr, int* nfiles, 
            int*  nxff,                 int*  neff, 
            char  vec_xfnames[50][256], char  vec_efnames[50][256], 
            FILE* vec_xifp[50],         FILE* vec_eifp[50]) {
/*******************************************************************************
* "get (deflection) grids"
* Fills a vector with the file names of the gridded input data,
* and open all files that can be found in the given directory.
* Additional geoid models may be added as developed.
*  in - imodel  : integer key to specific geoid model
*  in - dirnam  : directory location of binary geoid model files
*               : special- "ngs" is key to use NGS network files
*  in - IS_SUBR : run as subroutine: false=0;  true=1; (c std notation)
*  out- nfiles  : number of binary files used for the specific deflection model
*  out- vec_fnames : array (vector of character vectors) of geoid model filenames
*  out- vec_xifp : vector of file unit numbers  (type = long int)
*                : of geoid model files opened in this subroutine
*  out- vec_eifp : vector of file pointers  
*                : of deflection model files opened in this subroutine
*  out- neff     : "Number of Eta Files Found" and --successfully opened--
*  out- nxff     : "Number of Zi  Files Found" and --successfully opened--
* 
* Notes: 
* was: (imodel, dirnam, is_subr, nfiles, fnam, lin, nff )
*      int* imodel; char* dirnam; int*  is_subr; int*  nfiles;
*      char* fnam;   int*  lin;   int*  nff;
*******************************************************************************/
    char  this_efname[256];
    char  this_xfname[256];
    char  cval[3];
    const char suffix[] = ".bin";
    char  this_efname_prefix[30];
    char  this_xfname_prefix[30];
    const char conus_id[] = "u";
    const char ak_id[] = "a";
    const char hw_id[] = "h";
    const char pr_id[] = "p";
    const char as_id[] = "s";
    const char gu_id[] = "g";

    int  dirlen = 0;
    int  ii;

    trim_c(dirnam, 'b');
    dirlen = strlen(dirnam);

    // -----------------------------------------------------
    // The DEFLEC99 file names
    // -----------------------------------------------------
    if (imodel == 1) {
        *nfiles = 14;    // 14 of xi, 14 of eta
        int numXiVecFiles = 0;
        int numEtaVecFiles = 0;

        FILE* ifp_xconus;
        FILE* ifp_econus;
        strncpy(this_xfname_prefix, "\0", sizeof(this_xfname_prefix));
        strncpy(this_efname_prefix, "\0", sizeof(this_efname_prefix));
        strcpy(this_xfname_prefix, "x1999");
        strcpy(this_efname_prefix, "e1999");

        // CONUS files
        ii=0;
        sprintf(this_xfname, "%s%s%s%02d%s", dirnam, this_xfname_prefix, conus_id, ii, suffix);
        sprintf(this_efname, "%s%s%s%02d%s", dirnam, this_efname_prefix, conus_id, ii, suffix);
        if ( ((ifp_xconus = fopen(this_xfname, "rb")) != NULL) &&
             ((ifp_econus = fopen(this_efname, "rb")) != NULL) ) {
            *nfiles -= 7;
            ii=0;
            sprintf(vec_xfnames[numXiVecFiles++], "%s", this_xfname);
            sprintf(vec_efnames[numEtaVecFiles++], "%s", this_efname);
            fclose(ifp_xconus);
            fclose(ifp_econus);
        } else {
           // First 8 files are CONUS -----
           for (ii = 1; ii <= 8; ++ii) {
               sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, conus_id, ii, suffix);
               sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, conus_id, ii, suffix);
           }
        }

        // Next 4 files are ALASKA -----
        // Alaska -----
        // Attempt to open the one file model
        FILE* ifp_xak;
        FILE* ifp_eak;
        ii=0;
        sprintf(this_xfname, "%s%s%s%02d%s", dirnam, this_xfname_prefix, ak_id, ii, suffix);
        sprintf(this_xfname, "%s%s%s%02d%s", dirnam, this_efname_prefix, ak_id, ii, suffix);

        if ( ((ifp_xak = fopen(this_xfname, "rb")) != NULL) && 
             ((ifp_eak = fopen(this_efname, "rb")) != NULL) ) {
            *nfiles -= 3;
            ii=0;
            sprintf(vec_xfnames[numXiVecFiles++], "%s", this_xfname);
            sprintf(vec_efnames[numEtaVecFiles++], "%s", this_efname);
            fclose(ifp_xak);
            fclose(ifp_eak);

        } else {
           for (ii = 1; ii <= 4; ++ii) {
               sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, ak_id, ii, suffix);
               sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, ak_id, ii, suffix);
           }
        }

        // Next 1 file is HAWAII -----
        ii=1;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, hw_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, hw_id, ii, suffix);
     
        // Next 1 file is PR  
        ii=1;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, pr_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, pr_id, ii, suffix);

        // No Guam 

        // No American Samoa 

    // -----------------------------------------------------
    // The USDOV2009 file names
    // -----------------------------------------------------
    } else if (imodel == 2) {
        *nfiles = 16;   // 16 files of xi, 16 for eta
        int numXiVecFiles = 0;
        int numEtaVecFiles = 0;

        FILE* ifp_xconus;
        FILE* ifp_econus;
        strncpy(this_xfname_prefix, "\0", sizeof(this_xfname_prefix));
        strncpy(this_efname_prefix, "\0", sizeof(this_efname_prefix));
        strcpy(this_xfname_prefix, "x2009");
        strcpy(this_efname_prefix, "e2009");

        // CONUS files
        ii=0;
        sprintf(this_xfname, "%s%s%s%02d%s", dirnam, this_xfname_prefix, conus_id, ii, suffix);
        sprintf(this_efname, "%s%s%s%02d%s", dirnam, this_efname_prefix, conus_id, ii, suffix);
        if ( ((ifp_xconus = fopen(this_xfname, "rb")) != NULL) &&
             ((ifp_econus = fopen(this_efname, "rb")) != NULL) ) {
            *nfiles -= 7;
            ii=0;
            sprintf(vec_xfnames[numXiVecFiles++], "%s", this_xfname);
            sprintf(vec_efnames[numEtaVecFiles++], "%s", this_efname);
            fclose(ifp_xconus);
            fclose(ifp_econus);
        } else {
           // First 8 files are CONUS -----
           for (ii = 1; ii <= 8; ++ii) {
               sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, conus_id, ii, suffix);
               sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, conus_id, ii, suffix);
           }
        }

        // Next 4 files are ALASKA -----
        // Alaska -----
        // Attempt to open the one file model
        FILE* ifp_xak;
        FILE* ifp_eak;
        ii=0;
        sprintf(this_xfname, "%s%s%s%02d%s", dirnam, this_xfname_prefix, ak_id, ii, suffix);
        sprintf(this_xfname, "%s%s%s%02d%s", dirnam, this_efname_prefix, ak_id, ii, suffix);

        if ( ((ifp_xak = fopen(this_xfname, "rb")) != NULL) && 
             ((ifp_eak = fopen(this_efname, "rb")) != NULL) ) {
            *nfiles -= 3;
            ii=0;
            sprintf(vec_xfnames[numXiVecFiles++], "%s", this_xfname);
            sprintf(vec_efnames[numEtaVecFiles++], "%s", this_efname);
            fclose(ifp_xak);
            fclose(ifp_eak);

        } else {
           for (ii = 1; ii <= 4; ++ii) {
               sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, ak_id, ii, suffix);
               sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, ak_id, ii, suffix);
           }
        }

        // Next 1 file is HAWAII -----
        ii=1;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, hw_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, hw_id, ii, suffix);
     
        // Next 1 file is PR/VI -----
        ii=1;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, pr_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, pr_id, ii, suffix);

        // Next 1 file is Guam/Northern Marianas -----
        ii=1;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, gu_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, gu_id, ii, suffix);

        // Next 1 file is SAMOA -----
        ii=1;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, as_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, as_id, ii, suffix);

    // -----------------------------------------------------
    // The DEFLEC09 file names
    // -----------------------------------------------------
    } else if (imodel == 3) {
        *nfiles = 15;   // 15 files of xi, 15 for eta,      no PR/VI
        int numXiVecFiles = 0;
        int numEtaVecFiles = 0;

        FILE* ifp_xconus;
        FILE* ifp_econus;
        strncpy(this_xfname_prefix, "\0", sizeof(this_xfname_prefix));
        strncpy(this_efname_prefix, "\0", sizeof(this_efname_prefix));
        strcpy(this_xfname_prefix, "xh009");
        strcpy(this_efname_prefix, "eh009");

        // CONUS files
        ii=0;
        sprintf(this_xfname, "%s%s%s%02d%s", dirnam, this_xfname_prefix, conus_id, ii, suffix);
        sprintf(this_efname, "%s%s%s%02d%s", dirnam, this_efname_prefix, conus_id, ii, suffix);
        if ( ((ifp_xconus = fopen(this_xfname, "rb")) != NULL) &&
             ((ifp_econus = fopen(this_efname, "rb")) != NULL) ) {
            *nfiles -= 7;
            ii=0;
            sprintf(vec_xfnames[numXiVecFiles++], "%s", this_xfname);
            sprintf(vec_efnames[numEtaVecFiles++], "%s", this_efname);
            fclose(ifp_xconus);
            fclose(ifp_econus);
        } else {
           // First 8 files are CONUS -----
           for (ii = 1; ii <= 8; ++ii) {
               sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, conus_id, ii, suffix);
               sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, conus_id, ii, suffix);
           }
        }

        // Next 4 files are ALASKA -----
        // Alaska -----
        // Attempt to open the one file model
        FILE* ifp_xak;
        FILE* ifp_eak;
        ii=0;
        sprintf(this_xfname, "%s%s%s%02d%s", dirnam, this_xfname_prefix, ak_id, ii, suffix);
        sprintf(this_xfname, "%s%s%s%02d%s", dirnam, this_efname_prefix, ak_id, ii, suffix);

        if ( ((ifp_xak = fopen(this_xfname, "rb")) != NULL) && 
             ((ifp_eak = fopen(this_efname, "rb")) != NULL) ) {
            *nfiles -= 3;
            ii=0;
            sprintf(vec_xfnames[numXiVecFiles++], "%s", this_xfname);
            sprintf(vec_efnames[numEtaVecFiles++], "%s", this_efname);
            fclose(ifp_xak);
            fclose(ifp_eak);

        } else {
           for (ii = 1; ii <= 4; ++ii) {
               sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, ak_id, ii, suffix);
               sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, ak_id, ii, suffix);
           }
        }

        // Next 1 file is HAWAII -----
        ii=1;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, hw_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, hw_id, ii, suffix);
     
        // No PR for DEFLEC09

        // Next 1 file is Guam/Northern Marianas -----
        ii=1;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, gu_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, gu_id, ii, suffix);

        // Next 1 file is SAMOA -----
        ii=1;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, as_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, as_id, ii, suffix);

    // -----------------------------------------------------
    // The USDOV2012 file names
    // -----------------------------------------------------
    } else if (imodel == 4) {
        *nfiles = 16;   // 16 files of xi, 16 for eta
        int numXiVecFiles = 0;
        int numEtaVecFiles = 0;

        FILE* ifp_xconus;
        FILE* ifp_econus;
        strncpy(this_xfname_prefix, "\0", sizeof(this_xfname_prefix));
        strncpy(this_efname_prefix, "\0", sizeof(this_efname_prefix));
        strcpy(this_xfname_prefix, "x2012");
        strcpy(this_efname_prefix, "e2012");

        // CONUS files
        ii=0;
        sprintf(this_xfname, "%s%s%s%02d%s", dirnam, this_xfname_prefix, conus_id, ii, suffix);
        sprintf(this_efname, "%s%s%s%02d%s", dirnam, this_efname_prefix, conus_id, ii, suffix);
        if ( ((ifp_xconus = fopen(this_xfname, "rb")) != NULL) &&
             ((ifp_econus = fopen(this_efname, "rb")) != NULL) ) {
            *nfiles -= 7;
            ii=0;
            sprintf(vec_xfnames[numXiVecFiles++], "%s", this_xfname);
            sprintf(vec_efnames[numEtaVecFiles++], "%s", this_efname);
            fclose(ifp_xconus);
            fclose(ifp_econus);
        } else {
           // First 8 files are CONUS -----
           for (ii = 1; ii <= 8; ++ii) {
               sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, conus_id, ii, suffix);
               sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, conus_id, ii, suffix);
           }
        }

        // Next 4 files are ALASKA -----
        // Alaska -----
        // Attempt to open the one file model
        FILE* ifp_xak;
        FILE* ifp_eak;
        ii=0;
        sprintf(this_xfname, "%s%s%s%02d%s", dirnam, this_xfname_prefix, ak_id, ii, suffix);
        sprintf(this_xfname, "%s%s%s%02d%s", dirnam, this_efname_prefix, ak_id, ii, suffix);

        if ( ((ifp_xak = fopen(this_xfname, "rb")) != NULL) && 
             ((ifp_eak = fopen(this_efname, "rb")) != NULL) ) {
            *nfiles -= 3;
            ii=0;
            sprintf(vec_xfnames[numXiVecFiles++], "%s", this_xfname);
            sprintf(vec_efnames[numEtaVecFiles++], "%s", this_efname);
            fclose(ifp_xak);
            fclose(ifp_eak);

        } else {
           for (ii = 1; ii <= 4; ++ii) {
               sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, ak_id, ii, suffix);
               sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, ak_id, ii, suffix);
           }
        }

        // Next 1 file is HAWAII -----
        ii=0;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, hw_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, hw_id, ii, suffix);
     
        // Next 1 file is PR/VI -----
        ii=0;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, pr_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, pr_id, ii, suffix);

        // Next 1 file is Guam/Northern Marianas -----
        ii=0;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, gu_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, gu_id, ii, suffix);

        // Next 1 file is SAMOA -----
        ii=0;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%02d%s", dirnam, this_xfname_prefix, as_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%02d%s", dirnam, this_efname_prefix, as_id, ii, suffix);

    // -----------------------------------------------------
    // The DEFLEC12A file names
    // -----------------------------------------------------
    } else if (imodel == 5) {
        *nfiles = 16;   // 16 files of xi, 16 for eta
        int numXiVecFiles = 0;
        int numEtaVecFiles = 0;

        FILE* ifp_xconus;
        FILE* ifp_econus;
        strncpy(this_xfname_prefix, "\0", sizeof(this_xfname_prefix));
        strncpy(this_efname_prefix, "\0", sizeof(this_efname_prefix));
        strcpy(this_xfname_prefix, "xhg12A");
        strcpy(this_efname_prefix, "ehg12A");

        // CONUS files
        ii=0;
        sprintf(this_xfname, "%s%s%s%01d%s", dirnam, this_xfname_prefix, conus_id, ii, suffix);
        sprintf(this_efname, "%s%s%s%01d%s", dirnam, this_efname_prefix, conus_id, ii, suffix);
        if ( ((ifp_xconus = fopen(this_xfname, "rb")) != NULL) &&
             ((ifp_econus = fopen(this_efname, "rb")) != NULL) ) {
            *nfiles -= 7;
            ii=0;
            sprintf(vec_xfnames[numXiVecFiles++], "%s", this_xfname);
            sprintf(vec_efnames[numEtaVecFiles++], "%s", this_efname);
            fclose(ifp_xconus);
            fclose(ifp_econus);
        } else {
           // First 8 files are CONUS -----
           for (ii = 1; ii <= 8; ++ii) {
               sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%01d%s", dirnam, this_xfname_prefix, conus_id, ii, suffix);
               sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%01d%s", dirnam, this_efname_prefix, conus_id, ii, suffix);
           }
        }

        // Next 4 files are ALASKA -----
        // Alaska -----
        // Attempt to open the one file model
        FILE* ifp_xak;
        FILE* ifp_eak;
        ii=0;
        sprintf(this_xfname, "%s%s%s%01d%s", dirnam, this_xfname_prefix, ak_id, ii, suffix);
        sprintf(this_efname, "%s%s%s%01d%s", dirnam, this_efname_prefix, ak_id, ii, suffix);

        if ( ((ifp_xak = fopen(this_xfname, "rb")) != NULL) && 
             ((ifp_eak = fopen(this_efname, "rb")) != NULL) ) {
            *nfiles -= 3;
            ii=0;
            sprintf(vec_xfnames[numXiVecFiles++], "%s", this_xfname);
            sprintf(vec_efnames[numEtaVecFiles++], "%s", this_efname);
            fclose(ifp_xak);
            fclose(ifp_eak);

        } else {
           for (ii = 1; ii <= 4; ++ii) {
               sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%01d%s", dirnam, this_xfname_prefix, ak_id, ii, suffix);
               sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%01d%s", dirnam, this_efname_prefix, ak_id, ii, suffix);
           }
        }

        // Next 1 file is HAWAII -----
        ii=0;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%01d%s", dirnam, this_xfname_prefix, hw_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%01d%s", dirnam, this_efname_prefix, hw_id, ii, suffix);
     
        // Next 1 file is PR/VI -----
        ii=0;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%01d%s", dirnam, this_xfname_prefix, pr_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%01d%s", dirnam, this_efname_prefix, pr_id, ii, suffix);

        // Next 1 file is Guam/Northern Marianas -----
        ii=0;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%01d%s", dirnam, this_xfname_prefix, gu_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%01d%s", dirnam, this_efname_prefix, gu_id, ii, suffix);

        // Next 1 file is SAMOA -----
        ii=0;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%01d%s", dirnam, this_xfname_prefix, as_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%01d%s", dirnam, this_efname_prefix, as_id, ii, suffix);

    // -----------------------------------------------------
    // The DEFLEC12B file names
    // -----------------------------------------------------
    } else if (imodel == 6) {
        *nfiles = 16;   // 16 files of xi, 16 for eta
        int numXiVecFiles = 0;
        int numEtaVecFiles = 0;

        FILE* ifp_xconus;
        FILE* ifp_econus;
        strncpy(this_xfname_prefix, "\0", sizeof(this_xfname_prefix));
        strncpy(this_efname_prefix, "\0", sizeof(this_efname_prefix));
        strcpy(this_xfname_prefix, "xhg12B");
        strcpy(this_efname_prefix, "ehg12B");

        // CONUS files
        ii=0;
        sprintf(this_xfname, "%s%s%s%01d%s", dirnam, this_xfname_prefix, conus_id, ii, suffix);
        sprintf(this_efname, "%s%s%s%01d%s", dirnam, this_efname_prefix, conus_id, ii, suffix);
        if ( ((ifp_xconus = fopen(this_xfname, "rb")) != NULL) &&
             ((ifp_econus = fopen(this_efname, "rb")) != NULL) ) {
            *nfiles -= 7;
            ii=0;
            sprintf(vec_xfnames[numXiVecFiles++], "%s", this_xfname);
            sprintf(vec_efnames[numEtaVecFiles++], "%s", this_efname);
            fclose(ifp_xconus);
            fclose(ifp_econus);
        } else {
           // First 8 files are CONUS -----
           for (ii = 1; ii <= 8; ++ii) {
               sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%01d%s", dirnam, this_xfname_prefix, conus_id, ii, suffix);
               sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%01d%s", dirnam, this_efname_prefix, conus_id, ii, suffix);
           }
        }

        // Next 4 files are ALASKA -----
        // Alaska -----
        // Attempt to open the one file model
        FILE* ifp_xak;
        FILE* ifp_eak;
        ii=0;
        sprintf(this_xfname, "%s%s%s%01d%s", dirnam, this_xfname_prefix, ak_id, ii, suffix);
        sprintf(this_efname, "%s%s%s%01d%s", dirnam, this_efname_prefix, ak_id, ii, suffix);

        if ( ((ifp_xak = fopen(this_xfname, "rb")) != NULL) && 
             ((ifp_eak = fopen(this_efname, "rb")) != NULL) ) {
            *nfiles -= 3;
            ii=0;
            sprintf(vec_xfnames[numXiVecFiles++], "%s", this_xfname);
            sprintf(vec_efnames[numEtaVecFiles++], "%s", this_efname);
            fclose(ifp_xak);
            fclose(ifp_eak);

        } else {
           for (ii = 1; ii <= 4; ++ii) {
               sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%01d%s", dirnam, this_xfname_prefix, ak_id, ii, suffix);
               sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%01d%s", dirnam, this_efname_prefix, ak_id, ii, suffix);
           }
        }

        // Next 1 file is HAWAII -----
        ii=0;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%01d%s", dirnam, this_xfname_prefix, hw_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%01d%s", dirnam, this_efname_prefix, hw_id, ii, suffix);
     
        // Next 1 file is PR/VI -----
        ii=0;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%01d%s", dirnam, this_xfname_prefix, pr_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%01d%s", dirnam, this_efname_prefix, pr_id, ii, suffix);

        // Next 1 file is Guam/Northern Marianas -----
        ii=0;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%01d%s", dirnam, this_xfname_prefix, gu_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%01d%s", dirnam, this_efname_prefix, gu_id, ii, suffix);

        // Next 1 file is SAMOA -----
        ii=0;
        sprintf(vec_xfnames[numXiVecFiles++], "%s%s%s%01d%s", dirnam, this_xfname_prefix, as_id, ii, suffix);
        sprintf(vec_efnames[numEtaVecFiles++], "%s%s%s%01d%s", dirnam, this_efname_prefix, as_id, ii, suffix);

    }//~if(imodel)

    // -----------------------------------------------------
    // Now open all the files
    // Iterate thru the Filenames vector
    // Open files, store file pointers to vector of FILE pointers
    // Grid file format is unformatted, direct access
    // -----------------------------------------------------
    for (ii = 0; ii < *nfiles; ++ii) {
        FILE* xifp;
        FILE* eifp;

        xifp = fopen(vec_xfnames[ii], "rb");
        if (xifp == NULL){ printf("Open file failed for %s\n",vec_xfnames[ii]);
        } else {           ++(*nxff);
        }

        eifp = fopen(vec_efnames[ii], "rb");
        if (eifp == NULL){ printf("Open file failed for %s\n", vec_efnames[ii]);
        } else {           ++(*neff);
        }

        vec_xifp[ii] = xifp;
        vec_eifp[ii] = eifp;

        if( (xifp != NULL)  &&  (is_subr == 0) ) {   // (0 := false)
            fprintf(stdout, " *** Opening File: %s\n", vec_xfnames[ii]);
        }
        if( (eifp != NULL)  &&  (is_subr == 0) ) {   // (0 := false)
            fprintf(stdout, " *** Opening File: %s\n", vec_efnames[ii]);
        }
    }

    // -----------------------------------------------------
    // Check and see if at least ONE file was opened,
    // and make a count of how many WERE opened.
    // Abort if we find no geoid files.
    // -----------------------------------------------------
    if ((*neff == 0) && (*nxff == 0)) {
        fprintf(stderr, "\
ERROR(209):  No files found -- aborting \n\
    Input directory  = %s \n\
    Input model code = %d \n", dirnam, imodel);
        abort();
    }

    return;

}//~getgrd_defl

