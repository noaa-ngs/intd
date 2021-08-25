// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: getdir_defl.c 65880 2012-10-11 19:57:28Z Srinivas.Reddy $	20$Date: 2010/02/22 14:29:39 $ NGS";

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>

// ----- classes, structs, types -------------------------------------
// ----- functions ---------------------------------------------------
#include "getdir_defl.h"


int getdir_defl(int imodel, char* dirnam) {
/*******************************************************************************
* For PC network system, returns the directory location 
* where vertical deflection binary model grid files are located. 
*  in - imodel : user selected geoid model   int
*  in - dirnam : char vector, empty          character*256
*  out- dirnam : char vector, filled
*******************************************************************************/
#ifdef NGS_PC_ENV
    strncpy(dirnam, "\0", 256);

    printf("\n\
      What is the **FULL** directory name (including trailing slashes) \n\
      where the vertical deflection (*.bin) files may be found? \n\
         (Unix Example:  /export/home/deflec12/) \n\
         (PC   Example:  C:\\DEFLEC12\\) \n\
         Hit <RETURN> to default to this directory \n\
         -> ");

    fgets(dirnam, 256, stdin);
#else
/*******************************************************************************
* For the NGS unix network system, returns the directory location 
* where geoid binary model grid files are located. 
*  in - imodel : user selected geoid model   int
*  in - dirnam : char vector, empty          character*256 dirnam
*  out- dirnam : char vector, filled
*******************************************************************************/
    switch (imodel) {
    case 1: 
        strcpy(dirnam, "/ngslib/data/Deflec/Deflec99/");
        break;
    case 2: 
        strcpy(dirnam, "/ngslib/data/Deflec/Usdov2009/Format_unix/");
        break;
    case 3: 
        strcpy(dirnam, "/ngslib/data/Deflec/Deflec09/Format_unix/");
        break;
    case 4: 
        strcpy(dirnam, "/ngslib/data/Deflec/Usdov2012/Format_unix/");
        break;
    case 5: 
        strcpy(dirnam, "/ngslib/data/Deflec/Deflec12A/Format_unix/");
        break;

    default: 
        fprintf(stderr, "ERROR: in file getdir_deflux, invalid option %d\n", 
                imodel);
        break;
    }//~switch
#endif

    return(0);

}//~getdir_deflpc

