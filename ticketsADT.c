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
    tYears * firstYTD;
    size_t dimYears;
    size_t minFine;
    size_t maxFine;
    tInfractions * vecInfractions;
    size_t dimReal;     //Dim real
    size_t posibleInfraction; //Espacio reservado
    struct tAgencies * tail;
}tAgencies;

typedef struct tAgenDiff{ // agency in alphabetical order and descending order of diff
    char * agencyName;
    size_t diff;
    size_t max;
    size_t min;
    struct tAgenDiff * tail;
}tAgenDiff;

typedef struct ticketsCDT{
   tAgencies * first;
   tAgenDiff * firstDiff;

}ticketsCDT;

// function that validates the date (defensive programming)
static int validDate(size_t month, size_t year){
    return year > 0 && month <= totalMonths && month > 0;
}

static char * copyString(const char * source){
    char * target, 
    target = malloc(strlen(source)+1);
    if(target == NULL){
        perror(MEMORYERROR);
        exit(MEMORYEXIT); // cambair dsp 
    }
    strcpy(target, source);
    return target;
}

ticketsADT newTicket(void){
    ticketsADT new = calloc(1, sizeof(ticketsCDT));
    if(new == NULL){
        perror(MEMORYERROR);
        exit(MEMORYEXIT); // cambiar dsp 
    }
    return new;
}
//searches agency, and returns a copy 
static tAgencies * findAgency(tAgencies * l, const char * agency){
    int c;
    if(l == NULL || (c = strcasecmp(l->agencyName, agency)) > 0){
        return NULL;
    }else if( c == 0){
        return l;
    }
    return findAgency(l->tail, agency);
}
// searches the year
static tYears * findYTD(tYears * l, size_t year){
    int c = l->year - year;
    if(l == NULL || c > 0){
        return NULL;
    }
    if(c == 0){
        return l;
    }
    return findYTD(l->tail, year);
}

static tYears * searchFineRecYTD(tYears * l,size_t year,size_t month,size_t amount){
    size_t c = l->year - year;
    if(l == NULL || c > 0){ // case year doesnt exist
        tYears * new = calloc(1,sizeof(tYears));
        if(new == NULL){
            perror(MEMORYERROR);
            exit(MEMORYEXIT);
        }
        new->year = year;
        new->tail = l;
        new->months[month-1] += amount;
        return new;
    }else if(c == 0){ // case we found the year 
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
    l->vecInfractions = realloc(l->vecInfractions, id * sizeof(tInfractions)); //expand the array
    if(l->vecInfractions == NULL){
        perror(MEMORYERROR);
        exit(MEMORYEXIT); //mod dsp 
    }
    l->vecInfractions[id-1].fineCount++; //update fine count
    l->vecInfractions[id-1].infractionName = copyString(name); //add new infraction
}

static int checkDiff(size_t max, size_t min, size_t amount){
    return amount > max || amount < min;
}

static tAgenDiff * changeDiff(tAgenDiff * l, char * agency, size_t amount){ // changes the list of diff agency, same func as exercise 3 in the second exam this semester
    int c;
    if( l == NULL || (c = strcasecmp(l->agencyName, agency)) == 0){ // stops when it found the agency or it reached the end
        if(l == NULL){ // case it reached the end, we have to add the agency 
            tAgenDiff * new = malloc(sizeof(tAgenDiff));
            new->max = amount;
            new->min = amount;
            new->diff = 0;
            new->agencyName = copyString(agency);
            new->tail = l;
            return new;
        }else{ // case it found the agency, we modify the max, min and diff
            if(amount > l->max){
                l->max = amount;
            }
            l->min = amount;
            l->diff = l->max - l->min;
        }
        return l;
    }
    l->tail = checkDiff(l->tail, agency, amount); // recursive call (stack type recursion)

    tAgenDiff * aux = l->tail; // aux node to compare current node with its tail
    size_t b = aux->diff - l->diff; 
    if(b > 0){ // if aux diff is bigger than we swap positions
        l->tail = aux->tail;
        aux->tail = l;
        return aux;
    }else if(b == 0){ //case they have same amount of diff
        if(c > 0){ //since we got here, the agency provided is the same as its tail why we use the same c (responsible for modifications), we swap in alphabetical order
            l->tail = aux->tail;
            aux->tail = l;
            return aux;
        }
    }
    return l;
}

// searches if the agency exists, in the case it doesnt, it creates it in memory modifying the lists
static tAgencies * findOrCreateAgency(tAgencies * l, size_t id, const char * name, const char * agency, size_t fineAmount, size_t year, size_t month,tAgenDiff * firstDiff){
    int c;
    if(l == NULL || (c = strcasecmp(l->agencyName, agency)) > 0){  // case the agency doesnt exist
        tAgencies * new = calloc(1, sizeof(tAgencies));
        if(new == NULL){
            perror(MEMORYERROR);
            exit(MEMORYERROR); // cambiar dsp
        }
        new->agencyName = copyString(agency);
        new->tail = l;
        createNewInfraction(new, name, id); // function that creates the new infraction
        new->dimReal++;
        new->posibleInfraction = id;
        searchFineRecYTD(new->firstYTD, year, month, fineAmount); // updates the YTD data
        firstDiff = changeDiff(firstDiff, agency, fineAmount); // CHANGE DIF
        return new;
    }else if( c < 0){  // case the agency is lower than the one we want to fine (in alphabetical terms)
        l->tail = findOrCreateAgency(l->tail, id,name,agency,fineAmount, year, month, firstDiff);
        return l;
    } // case we found the agency
    if(id > l->posibleInfraction){  // if the id of infractions is higher than the dimension we expand the array
        createNewInfraction(l, name, id); 
        l->dimReal++;
        l->posibleInfraction = id;
    }else{
        if(l->vecInfractions[id-1].infractionName == NULL){ //we check if that infractions exists
            l->vecInfractions[id-1].infractionName = copyString(name);
        }
        l->vecInfractions[id-1].fineCount++;
    }
    if(checkDiff(l->maxFine, l->minFine,fineAmount)){
    firstDiff = changeDiff(firstDiff, agency, fineAmount);
    } // checks 
    searchFineRecYTD(l->firstYTD, year, month, fineAmount); // either the caseses the YTD needs to be updated 
    return l;
}

void addInfraction(ticketsADT ticket, size_t id, const char * name, const char * agency, size_t fineAmount, size_t year, size_t month){
    if(id <= 0 || !validDate(month, year)){ 
        exit(MEMORYERROR); //check
    }
    ticket->first = findOrCreateAgency(ticket->first, id, name, agency, fineAmount, year, month, ticket->firstDiff); // recursive call to find agency
}


int maxFines(ticketsADT ticket, size_t id, const char * agency){
    if(ticket == NULL || agency == NULL){
        exit(MEMORYEXIT); //check
    }
    tAgencies * aux = findAgency(ticket->first,agency);
    if( aux == NULL){ // we check what the recursive call returns, to no dereference a NULL pointer
        return -1;
    }
     if( id < aux->posibleInfraction && aux->vecInfractions[id-1].infractionName != NULL){ // we check by infraction id if such infraction exists in this agency
        return aux->vecInfractions[id-1].fineCount;
     }
     return -1; // case the infractions doesnt exist
}


int YTDfines(ticketsADT ticket, const char * agency,size_t year,size_t month){
    if(ticket == NULL || !validDate(month, year || agency == NULL)){ //case of invalid parameters
        exit(MEMORYEXIT);
    }
    tAgencies * auxAgen = findAgency(ticket->first, agency);
    if(auxAgen == NULL){
        return -1;
    }
    tYears * auxYTD= findYTD(auxAgen->firstYTD , year); //serach such year
    if(auxYTD == NULL){ //if not exist returns an error type parameter for the front-end
        return -1;
    }
    size_t sum = 0;
    for(size_t i = 0; i < month-1; i++){  // we return the sum from january till that month 
        sum += auxYTD->months[i];
    }
    if(sum == 0){
        return -1;
    }
    return sum; // we return the sum
}

static tAgenDiff * findAgencyDiff(tAgenDiff * l, char * agency){
    int c;
    if(l == NULL || (c = strcasecmp(l->agencyName, agency)) == 0){
        return l;
    }
    return findAgencyDiff(l->tail, agency);
}

int minAgencyFines(ticketsADT ticket, const char * agency){
    if(ticket == NULL || agency == NULL){
        perror(MEMORYEXIT);
        exit(MEMORYEXIT); //check
    }

    tAgenDiff * aux = findAgencyDiff(ticket->firstDiff, agency);
    if(aux == NULL){
        return -1;
    }
    return aux->min;
}


int maxAgencyFines(ticketsADT ticket, const char * agency){
    if(ticket == NULL || agency == NULL){
        perror(MEMORYEXIT);
        exit(MEMORYEXIT); //check
    }

    tAgenDiff * aux = findAgencyDiff(ticket->firstDiff, agency);
    if(aux == NULL){
        return -1;
    }
    return aux->max;
}

int diffAgencyFines(ticketsADT ticket,const char * agency){
    if(ticket == NULL || agency == NULL){
        perror(MEMORYEXIT);
        exit(MEMORYEXIT); //check
    }

    tAgenDiff * aux = findAgencyDiff(ticket->firstDiff, agency);
    if(aux == NULL){
        return -1;
    }
    return aux->min;
}

static int cmpInf(const void *a, const void *b) {
    const tInfractions *infractionA = (const tInfractions *)a;
    const tInfractions *infractionB = (const tInfractions *)b;

    if(infractionA->infractionName == NULL && infractionB->infractionName == NULL){
        return 0;
    }else if(infractionA->infractionName == NULL){
        return -1;
    }else if(infractionB->infractionName == NULL){
        return 1;
    }
    return strcmp(infractionA->infractionName, infractionB->infractionName);
}

void sortByAlph(ticketsADT ticket){
    if(ticket == NULL){
        return;
    }
    for(; ticket->first != NULL; ticket->first = ticket->first->tail){
        tAgencies * aux = ticket->first;
        qsort(aux->vecInfractions, aux->posibleInfraction, sizeof(tInfractions), &cmpInf);
    }
}


static void freeAgencyDiff(tAgenDiff * l){
    if(l == NULL){
        return;
    }
    tAgenDiff * aux = l->tail;
    free(l->agencyName);
    free(l);
    freeAgency(aux);
}

// frees YTD list
static void freeYTD(tYears * l){
    if(l == NULL){
        return;
    }
    tYears * aux = l->tail;
    free(l);
    freeYTD(aux);
}

// frees agency list
static void freeAgency(tAgencies * l){
    if(l == NULL){
        return;
    }
    tAgencies * aux = l->tail;
    free(l->agencyName);
    freeYTD(l->firstYTD);
    for(size_t i = 0; i < l->posibleInfraction; i++){
        free(l->vecInfractions[i].infractionName);
    }
    free(l->vecInfractions);
    free(l);
    freeAgency(aux);
}

void freeTicket(ticketsADT ticket){
    freeAgency(ticket->first);
    freeAgencyDiff(ticket->firstDiff);
    free(ticket);
}