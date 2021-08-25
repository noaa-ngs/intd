#ifndef RUN_BBK_H
#define RUN_BBK_H

// %P%
// ----- constants ---------------------------------------------------
#pragma ident "$Id: run_bbk.h 65661 2012-10-04 15:02:05Z Srinivas.Reddy $	20$Date: 2009/06/19 12:40:59 $ NGS"

// ----- standard library --------------------------------------------
#include <stdio.h>

// ----- classes, structs, types -------------------------------------
#include "grid_header.h"

// ----- functions ---------------------------------------------------
void run_bbk(FILE* ifp, FILE* ofp,
             FILE** vec_xifp, FILE** vec_eifp, GRID_HEADER vec_hdr[50],
             char vec_xfnames[50][256], int nfiles, int imodel);

#endif //~RUN_BBK_H

