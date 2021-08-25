#ifndef FF4OUT_H
#define FF4OUT_H

// %P%
// ----- constants ---------------------------------------------------
#pragma ident "$Id: ff4out.h 44065 2010-07-12 20:16:35Z bruce.tran $	20$Date: 2009/07/06 18:12:11 $ NGS"

// ----- standard library --------------------------------------------
#include <stdio.h>

// ----- classes, structs, types -------------------------------------
#include "dataset1.h"

// ----- functions ---------------------------------------------------
int ff4out(DATASET1 vec_data, double xi, double eta, double lap, int poseast);

#endif //~FF4OUT_H

