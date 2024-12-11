    #include <stdio.h>
    #include <stdlib.h>
    #include <errno.h>
    #include <string.h>
    #include <ctype.h>
    #include "ticketsADT.h"

    #define MEMORYERROR "wrong memory access"
    #define INVALIDARGUMENT "the argument is invalid"
    #define MEMORYEXIT 1
    /*
#define totalMonths 12
    typedef struct tYears{
        size_t year;
        size_t months[totalMonths];
        struct tYears * tail;
    }tYears;
//Struct for the arr of Infractions for infractions.csv
    typedef struct tInfNameId{
        char * infractionName;
    }tInfNameId;
//Struct for the arr of Infractions of each agency
    typedef struct tInfranctions{
        char * infractionName; //ESTO
        size_t fineCount; //How many times appears
    }tInfractions;


    typedef struct tAgencies{
        char * agencyName;
        tYears * firstYTD;
        size_t dimYears;
        size_t minFine;
        size_t maxFine;
        tInfractions * vecInfractions; //ESTO
        size_t posibleInfraction; //Espacio reservado ESTO 
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
        tInfNameId * vecInfraction; //para infractions.csv
        size_t vecSize;
        tAgencies * first;
        tAgenDiff * firstDiff;
    }ticketsCDT;
    */

    int strcasecmp(const char *str1, const char *str2);
    // function that validates the date (defensive programming)
    static int validDate(size_t month, size_t year){
        return (year > 0 && month <= totalMonths && month >= 1);
    }

    static char * copyString(const char * source){
        if(source==NULL){
            return NULL;
        } 
        char * target = malloc(strlen(source)+1);
        if(target == NULL){
            perror(MEMORYERROR);
            return NULL;
        }
        strcpy(target, source);
        return target;
    }

    ticketsADT newTicket(void){
        ticketsADT new = calloc(1, sizeof(ticketsCDT));
        if(new == NULL){
            perror(MEMORYERROR);
            exit(MEMORYEXIT); 
        }
        return new;
    }
    //searches the agency, and returns a copy 
    static tAgencies * findAgency(tAgencies * l, const char * agency){
        int c;
        if(agency==NULL){
            perror(INVALIDARGUMENT);
            exit(MEMORYEXIT);
        }
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

    static tYears *searchFineRecYTD(tYears *l, size_t year, size_t month, size_t amount) {
        if (l == NULL||year < l->year) { // base case: create a node
            tYears *new = calloc(1, sizeof(tYears));
            if (new == NULL) {
                perror(MEMORYERROR);
                exit(MEMORYEXIT);
            }
            new->year = year;
            new->months[month - 1] = amount; // update the on corresponding date
            new->tail = l;
            return new;
        }
        if (year == l->year) {
            // if year ws found: update its month amount
            l->months[month - 1] += amount;
            return l;
        }
            // year > l->year: keep searching by tail
            l->tail = searchFineRecYTD(l->tail, year, month, amount);
            return l;
}


static tAgenDiff * changeDiff(tAgenDiff * l, const char * agency, size_t amount) {
    if (agency == NULL) {
        perror(INVALIDARGUMENT);
        exit(MEMORYEXIT);
    }

    // search or create new agency 
    tAgenDiff *current = l;
    tAgenDiff *prev = NULL;

    while (current != NULL && strcasecmp(current->agencyName, agency) < 0) {
        prev = current;
        current = current->tail;
    }

    if (current != NULL && strcasecmp(current->agencyName, agency) == 0) {
        // update existing values
        if (amount > current->max) current->max = amount;
        if (amount < current->min) current->min = amount;
        current->diff = current->max - current->min;
        return l;
    }

    tAgenDiff *newNode = malloc(sizeof(tAgenDiff));
    if (newNode == NULL) {
        perror(MEMORYERROR);
        exit(MEMORYEXIT);
    }
    newNode->agencyName = copyString(agency);
    newNode->max = amount;
    newNode->min = amount;
    newNode->diff = 0;
    newNode->tail = current;

    if (prev == NULL) {// we add in the beginning
        return newNode;  
    } else {
        prev->tail = newNode;
    }

    return l;
}

static void putInfractionInAgency(tAgencies *agency, const char *name, size_t id) {
    if (agency == NULL || name == NULL) {
        perror(MEMORYERROR);
        exit(MEMORYEXIT);
    }

    // in case the id was bigger than the memory reserved, we expand the array creating the necessary memory 
    if (id > agency->posibleInfraction) {
        size_t newSize = id;
        tInfractions *newVector = realloc(agency->vecInfractions, newSize * sizeof(tInfractions));
        if (newVector == NULL) {
            perror(MEMORYERROR);
            exit(MEMORYEXIT);
        }

        // initialize new spots
        for (size_t i = agency->posibleInfraction; i < newSize; i++) {
            newVector[i].infractionName = NULL;
            newVector[i].fineCount = 0;
        }

        agency->vecInfractions = newVector;
        agency->posibleInfraction = newSize;
    }

    // if infraction already exists, we increment the count 
    if (agency->vecInfractions[id - 1].infractionName != NULL) {
        agency->vecInfractions[id - 1].fineCount++;
    } else { // other case we create such infratcion
        agency->vecInfractions[id - 1].infractionName = copyString(name);
        agency->vecInfractions[id - 1].fineCount = 1;
    }
}


//Set the first array with all the infractions with the ids from the infractions.csv
    void createInfraction(ticketsADT ticket, size_t id, const char * id_name){
        if(id>ticket->vecSize){
            ticket->vecInfraction=realloc(ticket->vecInfraction,sizeof(tInfractions)*id);
            if(ticket->vecInfraction==NULL){
                perror(INVALIDARGUMENT);
                exit(MEMORYEXIT);
            }
            for(size_t i=ticket->vecSize;i<id;i++){
                ticket->vecInfraction[i].infractionName=NULL;
            }
            
            ticket->vecSize=id;
        }
        if(ticket->vecInfraction[id-1].infractionName==NULL){
            ticket->vecInfraction[id-1].infractionName = copyString(id_name);
        }
        
    }

    static tAgencies * findOrCreateAgency(ticketsADT ticket, tAgencies *l, size_t id, const char *agency, size_t fineAmount, size_t year, size_t month) {
    if (agency == NULL) {
        perror(INVALIDARGUMENT);
        exit(MEMORYEXIT);
    }

    if (l == NULL) {
        // create new agency 
        tAgencies *newA = calloc(1, sizeof(tAgencies));
        if (newA == NULL) {
            perror(MEMORYERROR);
            exit(MEMORYEXIT);
        }

        newA->agencyName = copyString(agency);
        const char *name = ticket->vecInfraction[id-1].infractionName;
        putInfractionInAgency(newA, name, id);

        // initialize max and min fine with the first fine 
        newA->maxFine = fineAmount;
        newA->minFine = fineAmount;

        newA->firstYTD = searchFineRecYTD(newA->firstYTD, year, month, fineAmount);

        // update firstDiff
        ticket->firstDiff = changeDiff(ticket->firstDiff, agency, fineAmount);

        return newA;
    }

    int c = strcasecmp(l->agencyName, agency);
    if (c == 0) {
        // agency was found
        const char * name = ticket->vecInfraction[id-1].infractionName;
        putInfractionInAgency(l, name, id);

        // update max and min
        if (fineAmount > l->maxFine){
            l->maxFine = fineAmount;
        }
        if (fineAmount < l->minFine){
           l->minFine = fineAmount; 
        } 

        // update the total amount in the month
        l->firstYTD=searchFineRecYTD(l->firstYTD, year, month, fineAmount);
        // update agencies diffs
        ticket->firstDiff = changeDiff(ticket->firstDiff, agency, fineAmount);
        return l;
    } else if (c < 0) {
        // in alphabetical order the agency goes before the one we are searching, we search its tail
        l->tail = findOrCreateAgency(ticket, l->tail, id, agency, fineAmount, year, month);
        return l;
    } else {
        tAgencies *newA = calloc(1, sizeof(tAgencies));
        if (newA == NULL) {
            perror(MEMORYERROR);
            exit(MEMORYEXIT);
        }
        newA->agencyName = copyString(agency);
        const char *name = ticket->vecInfraction[id-1].infractionName;
        putInfractionInAgency(newA, name, id);

        newA->maxFine = fineAmount;
        newA->minFine = fineAmount;
        newA->firstYTD = searchFineRecYTD(newA->firstYTD, year, month, fineAmount);
        ticket->firstDiff = changeDiff(ticket->firstDiff, agency, fineAmount);

        newA->tail = l;
        return newA;
    }
}

void addInfraction(ticketsADT ticket, size_t id, const char *agency, size_t fineAmount, size_t year, size_t month) {
    if (id <= 0 || !validDate(month, year)) { 
        exit(MEMORYEXIT);
    }

    ticket->first = findOrCreateAgency(ticket, ticket->first, id, agency, fineAmount, year, month);
}

    //function that returns the sum of the total fine mount from january that year until a certain month
    int YTDfines(ticketsADT ticket, const char * agency,size_t year,size_t month){
        if(ticket == NULL || !validDate(month, year) || agency == NULL){ //case of invalid parameters
            perror(INVALIDARGUMENT);
            exit(MEMORYEXIT);
        }
        tAgencies * auxAgen = findAgency(ticket->first, agency);
        if(auxAgen == NULL){
            return -1;
        }
        tYears * auxYTD= findYTD(auxAgen->firstYTD , year); //search such year
        if(auxYTD == NULL){ //in the case it doesnt exist it returns an error type parameter for the front-end
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
//Func used in sortByAlph to cmp alphabetically
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
//Func of order for query1 to order the infractions alphabetically
    void sortByAlph(ticketsADT ticket){
        if(ticket == NULL){
            return;
        }
        tAgencies * aux = ticket->first;
        while(aux!=NULL){            
            qsort(aux->vecInfractions, aux->posibleInfraction, sizeof(tInfractions),cmpInf);
            aux=aux->tail;
        }
    }
    //Func of comparison for query3
    int compareDiff(const tAgenDiff *a, const tAgenDiff *b) {
    if (a->diff != b->diff) {
        return b->diff - a->diff;  // Desc in FineDifference
    }
    return strcasecmp(a->agencyName, b->agencyName);  // Alphabetically order in case it has the same ammount of diff
    }


    // frees the agency diff array
    static void freeAgencyDiff(tAgenDiff * l){
        if(l == NULL){
            return;
        }
        tAgenDiff * aux = l->tail;
        free(l->agencyName);
        free(l);
        freeAgencyDiff(aux);
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
        for (size_t i = 0; i < ticket->vecSize; i++) {
            if (ticket->vecInfraction[i].infractionName != NULL) {
                free(ticket->vecInfraction[i].infractionName);
            }
        }
        free(ticket->vecInfraction); // free the array

        freeAgency(ticket->first);
        freeAgencyDiff(ticket->firstDiff);
        free(ticket);
    }

// func responisable to carry out query 1
void processQuery1(ticketsADT tickets, void (*callback)(const char *, const char *, size_t, void *), void *param) {
    tAgencies *agency = tickets->first;
    while (agency) {
        for (size_t i = 0; i < agency->posibleInfraction; i++) {
            if (agency->vecInfractions[i].fineCount > 0) {
                callback(agency->agencyName, agency->vecInfractions[i].infractionName, 
                agency->vecInfractions[i].fineCount, param);
            }
        }
        agency = agency->tail;
    }
}

// func responisable to carry out query 2
void processQuery2(ticketsADT tickets, void (*callback)(const char *, size_t, size_t, size_t, void *), void *param) {
    tAgencies *agency = tickets->first;

    while (agency) {
        tYears *year = agency->firstYTD;

        while (year) {
            size_t ytd = 0;

            for (int i = 0; i < totalMonths; i++) {
                if (year->months[i] > 0) {
                    ytd += year->months[i];
                    callback(agency->agencyName, year->year, i + 1, ytd, param);
                }
            }
            year = year->tail;
        }
        agency = agency->tail;
    }
}

//extra function for query3 to insert a given agency into a sorted linked list based on fine diff
static void insertSortedAgencyDiff(tAgenDiff **sortedList, tAgenDiff *current);
//extra function for query3 to iterate through the sorted list and invokes the callback function for each agency 
static void iterateAndPrintAgencyDiffs(tAgenDiff *sortedList, void (*callback)(const char *, size_t, size_t, size_t, void *), void *param);
//extra function for query3 to free all memory allocated for the sorted linked lists
static void freeSortedAgencyDiffList(tAgenDiff *sortedList);


// func responisable to carry out query 
void processQuery3(ticketsADT tickets, void (*callback)(const char *, size_t, size_t, size_t, void *), void *param) {
    tAgenDiff *sortedList = NULL;
    tAgenDiff *current = tickets->firstDiff;

    while (current != NULL) {
        insertSortedAgencyDiff(&sortedList, current);
        current = current->tail;
    }

    iterateAndPrintAgencyDiffs(sortedList, callback, param);
    freeSortedAgencyDiffList(sortedList);
}

static void insertSortedAgencyDiff(tAgenDiff **sortedList, tAgenDiff *current) {
    tAgenDiff **insertPoint = sortedList;

    while (*insertPoint != NULL && 
           ((*insertPoint)->diff > current->diff ||
            ((*insertPoint)->diff == current->diff && 
             strcasecmp((*insertPoint)->agencyName, current->agencyName) < 0))) {
        insertPoint = &(*insertPoint)->tail;
    }

    tAgenDiff *newNode = malloc(sizeof(tAgenDiff));
    if (newNode == NULL) {
        perror("Memory Allocation Error");
        exit(EXIT_FAILURE);
    }

    *newNode = *current;
    newNode->tail = *insertPoint;
    *insertPoint = newNode;
}

static void iterateAndPrintAgencyDiffs(tAgenDiff *sortedList, void (*callback)(const char *, size_t, size_t, size_t, void *), void *param) {
    tAgenDiff *current = sortedList;

    while (current != NULL) {
        callback(current->agencyName, current->min, current->max, current->diff, param);
        current = current->tail;
    }
}

static void freeSortedAgencyDiffList(tAgenDiff *sortedList) {
    while (sortedList != NULL) {
        tAgenDiff *nextNode = sortedList->tail;
        free(sortedList);
        sortedList = nextNode;
    }
}
