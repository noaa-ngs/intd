#ifndef FF1OUT_H
#define FF1OUT_H

// %P%
// ----- constants ---------------------------------------------------
#pragma ident "$Id: ff1out.h 44062 2010-07-12 20:16:23Z bruce.tran $	20$Date: 2009/07/06 18:11:44 $ NGS"

// ----- standard library --------------------------------------------
#include <stdio.h>

// ----- classes, structs, types -------------------------------------
#include "dataset1.h"

// ----- functions ---------------------------------------------------
int ff1out( FILE* ofp, DATASET1 vec_data,
            double xi, double eta, double lap, int poseast );

#endif //~FF1OUT_H

