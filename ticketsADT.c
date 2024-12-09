#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include "ticketsADT.h"

typedef struct tFines{
    char * whichFine;
    size_t numberFine;
} tFines;

typedef struct tInfractions{
    tFines * vecFines;
    size_t possibleFines;
    size_t dimFines;
    size_t infractionID;
} tInfractions;

typedef struct tAgencies{
    struct tAgencies * tail;
    char agencyName[100];
    size_t maxMount;
    size_t minMount;
}