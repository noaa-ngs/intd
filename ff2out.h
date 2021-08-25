#ifndef FF2OUT_H
#define FF2OUT_H

// %P%
// ----- constants ---------------------------------------------------
#pragma ident "$Id: ff2out.h 44063 2010-07-12 20:16:27Z bruce.tran $	20$Date: 2009/07/06 18:12:00 $ NGS"

// ----- standard library --------------------------------------------
#include <stdio.h>

// ----- classes, structs, types -------------------------------------
#include "dataset1.h"

// ----- functions ---------------------------------------------------
int ff2out(FILE* ofp, DATASET1 vec_data,
           double xi, double eta, double lap, int poseast);

#endif //~FF2OUT_H

