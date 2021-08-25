#ifndef GETGRD_DEFL_H
#define GETGRD_DEFL_H

// %P%
// ----- constants ---------------------------------------------------
#pragma ident "$Id: getgrd_defl.h 35309 2010-06-11 13:17:45Z Srinivas.Reddy $	20$Date: 2010/03/24 15:22:51 $ NGS"

// ----- standard library --------------------------------------------
#include <stdio.h>

// ----- classes, structs, types -------------------------------------
// ----- functions ---------------------------------------------------
void getgrd_defl(int  imodel, char* dirnam, int is_subr, int* nfiles,
                 int*  nxff,                 int*  neff,  
                 char  vec_xfnames[50][256], char  vec_efnames[50][256],
                 FILE* vec_xifp[50],         FILE* vec_eifp[50]);

#endif //~GETGRD_DEFL_H

