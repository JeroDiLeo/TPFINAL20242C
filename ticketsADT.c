#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define MEMORYERROR "wrong memory access"
#define INVALIDARGUMENT "the argument is invalid"
#define MEMORYEXIT 1
#define totalMonths 12

typedef struct tYears{
    size_t months[totalMonths];
} tYears;

typedef struct tInfranctions{
    char * infractionName;
    size_t mountMax;
    struct tInfractions * tail;
} tInfractions;


typedef struct tAgencies{
    char * agencyName;
    size_t maxFine;
    size_t minFine;
    size_t maxMinDiff;
    tYears * arrYears;
    size_t dimYears;
    tInfractions * first;
    struct tAgencies * tail;
} tAgencies;

typedef struct ticketCDT{
   tAgencies * first;
} ticketcCDT;

