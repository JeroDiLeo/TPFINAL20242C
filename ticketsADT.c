#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include "ticketsADT.h"

#define MEMORYERROR "wrong memory access"
#define INVALIDARGUMENT "the argument is invalid"
#define MEMORYEXIT 1
#define totalMonths 12

typedef struct tYears{
    size_t year;
    size_t months[totalMonths];
    struct tYears * tail;
}tYears;

typedef struct tInfranctions{
    char * infractionName;
    size_t fineCount; //How many times appears
}tInfractions;


typedef struct tAgencies{
    char * agencyName;
    size_t maxFine;
    size_t minFine;
    size_t maxMinDiff;
    tYears * firstYTD;
    size_t dimYears;
    tInfractions * vecInfractions;
    size_t dimReal;     //Dim real
    size_t posibleInfraction; //Espacio reservado
    struct tAgencies * tail;
}tAgencies;

typedef struct ticketsCDT{
   tAgencies * first;
}ticketsCDT;

static int validDate(size_t month, size_t year){
    return year > 0 && month <= totalMonths && month > 0;
}

static char * copyString(const char * source){
    char * target, 
    target = malloc(strlen(source)+1);
    if(target == NULL){
        exit(MEMORYEXIT); // cambair dsp 
    }
    strcpy(target, source);
    return target;
}

ticketsADT newTicket(void){
    ticketsADT new = calloc(1, sizeof(ticketsCDT));
    if(new == NULL){
        exit(MEMORYEXIT); // cambiar dsp 
    }
    return new;
}

static tYears * searchFineRecYTD(tYears * l,size_t year,size_t month,size_t amount){
    size_t c = l->year - year;
    if(l == NULL || c > 0){
        tYears * new = calloc(1,sizeof(tYears));
        if(new == NULL){
            exit(MEMORYEXIT);
        }
        new->year = year;
        new->tail = l;
        new->months[month-1] += amount;
        return new;
    }else if(c == 0){
        l->months[month-1]+= amount; 
        return l;
    }
    l->tail = searchFineRecYTD(l->tail, year, month, amount);
    return l;
}
static void createNewInfraction(tAgencies * l,const  char * name, size_t id){
    if( l == NULL){
        exit(MEMORYERROR); //mod dsp
    }
    l->vecInfractions = realloc(l->vecInfractions, id * sizeof(tInfractions));
    if(l->vecInfractions == NULL){
        exit(MEMORYEXIT); //mod dsp 
    }
    l->vecInfractions[id-1].fineCount++;
    l->vecInfractions[id-1].infractionName = copyString(name);
}

static void modifyCreateAgency(tAgencies * l, size_t year, size_t month,size_t fineAmount){
    if(fineAmount > l->maxFine){
        l->maxFine = fineAmount;
    }else if(fineAmount < l->minFine){
        l->minFine = fineAmount;
    }
    l->maxMinDiff = l->maxFine - l->minFine;
    l->firstYTD = searchFineRecYTD(l->firstYTD, year, month, fineAmount);
}

// searches if the agency exists, in the case it doesnt, it creates it in memory modifying the lists
static tAgencies * findOrCreateAgency(tAgencies * l, size_t id, const char * name, const char * agency, size_t fineAmount, size_t year, size_t month){
    int c;
    if(l == NULL || (c = strcasecmp(l->agencyName, agency)) > 0){
        tAgencies * new = calloc(1, sizeof(tAgencies));
        if(new == NULL){
            exit(MEMORYERROR); // cambiar dsp
        }
        new->agencyName = copyString(agency);
        new->tail = l;
        createNewInfraction(new, name, id);
        new->dimReal++;
        new->posibleInfraction = id;
        modifyCreateAgency(new, year, month, fineAmount);
        return new;
    }else if( c < 0){
        l->tail = findOrCreateAgency(l->tail, id,name,agency,fineAmount, year, month);
        return l;
    }
    if(id > l->posibleInfraction){
        createNewInfraction(l, name, id);
        l->dimReal++;
        l->posibleInfraction = id;
    }else{
        if(l->vecInfractions[id-1].infractionName == NULL){
            l->vecInfractions[id-1].infractionName = copyString(name);
        }
        l->vecInfractions[id-1].fineCount++;
    }
    modifyCreateAgency(l, year, month, fineAmount);
    return l;
}

void addInfraction(ticketsADT ticket, size_t id, const char * name, const char * agency, size_t fineAmount, size_t year, size_t month){
    if(id <= 0 || !validDate(month, year)){
        exit(MEMORYERROR); //check
    }
    ticket->first = findOrCreateAgency(ticket->first, id, name, agency, fineAmount, year, month);
}