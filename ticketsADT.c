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
            exit(MEMORYEXIT); // cambiar dsp 
        }
        return new;
    }
    //searches agency, and returns a copy 
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
        if (l == NULL||year < l->year) { // Caso base: crear un nuevo nodo
            tYears *new = calloc(1, sizeof(tYears));
            if (new == NULL) {
                perror(MEMORYERROR);
                exit(MEMORYEXIT);
            }
            new->year = year;
            new->months[month - 1] = amount; // Actualizar el mes correspondiente
            new->tail = l;
            return new;
        }
        if (year == l->year) {
            // Año encontrado: actualizar el mes
            l->months[month - 1] += amount;
            return l;
        }
            // year > l->year: continuar buscando en la cola
            l->tail = searchFineRecYTD(l->tail, year, month, amount);
            return l;
}


static tAgenDiff * changeDiff(tAgenDiff * l, const char * agency, size_t amount) {
    if (agency == NULL) {
        perror(INVALIDARGUMENT);
        exit(MEMORYEXIT);
    }

    // Buscar la agencia o crear una nueva
    tAgenDiff *current = l:
    tAgenDiff *prev = NULL;

    while (current != NULL && strcasecmp(current->agencyName, agency) < 0) {
        prev = current;
        current = current->tail;
    }

    if (current != NULL && strcasecmp(current->agencyName, agency) == 0) {
        // Actualizar valores existentes
        if (amount > current->max) current->max = amount;
        if (amount < current->min) current->min = amount;
        current->diff = current->max - current->min;
        return l;
    }

    // Crear nueva entrada
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

    if (prev == NULL) {// Insertar al inicio
        return newNode;  
    } else {
        prev->tail = newNode;
    }

    return l;
}

/*
Old version
static tAgenDiff * changeDiff(tAgenDiff * l, const char * agency, size_t amount) {
    if (agency == NULL) {
        perror(INVALIDARGUMENT);
        exit(MEMORYEXIT);
    }

    int c = 0;

    // Caso base: lista vacía o encontramos la agencia
    if (l == NULL || (c = strcasecmp(l->agencyName, agency)) == 0) {
        if (l == NULL) { // Crear nueva agencia si no existe
            tAgenDiff *newNode = malloc(sizeof(tAgenDiff));
            if (newNode == NULL) { // Verificar malloc
                perror("Error al asignar memoria");
                exit(MEMORYEXIT);
            }
            newNode->max = amount;
            newNode->min = amount;
            newNode->diff = 0;
            newNode->agencyName = copyString(agency);
            if (newNode->agencyName == NULL) { // Verificar copyString
                free(newNode);
                perror("Error al copiar cadena");
                exit(MEMORYEXIT);
            }
            newNode->tail = l;
            return newNode;
        } else { // Actualizar valores si la agencia ya existe
            if (amount > l->max) {
                l->max = amount;
            }
            if (amount < l->min) {
                l->min = amount;
            }
            l->diff = l->max - l->min;
        }
        return l;
    }

    // Llamada recursiva para buscar la agencia
    tAgenDiff *newTail = changeDiff(l->tail, agency, amount);

    // Verificar si hay cambio en la lista
    if (l->tail != newTail) {
        l->tail = newTail;
    }

    // Reordenar nodos según diferencia y orden alfabético
    tAgenDiff *aux = l->tail;
    if (aux != NULL) {
        size_t diffComparison = aux->diff - l->diff;

        // Ordenar por diff descendente
        if (diffComparison > 0) {
            l->tail = aux->tail;
            aux->tail = l;
            return aux;
        } else if (diffComparison == 0) {
            // Si diffs son iguales, ordenar alfabéticamente
            if (strcasecmp(l->agencyName, aux->agencyName) > 0) {
                l->tail = aux->tail;
                aux->tail = l;
                return aux;
            }
        }
    }

    return l;
}
*/

static void putInfractionInAgency(tAgencies *agency, const char *name, size_t id) {
    if (agency == NULL || name == NULL) {
        perror(MEMORYERROR);
        exit(MEMORYEXIT);
    }

    // Redimensionar el vector si el ID es mayor que el espacio reservado
    if (id > agency->posibleInfraction) {
        size_t newSize = id;
        tInfractions *newVector = realloc(agency->vecInfractions, newSize * sizeof(tInfractions));
        if (newVector == NULL) {
            perror(MEMORYERROR);
            exit(MEMORYEXIT);
        }

        // Inicializar nuevas posiciones
        for (size_t i = agency->posibleInfraction; i < newSize; i++) {
            newVector[i].infractionName = NULL;
            newVector[i].fineCount = 0;
        }

        agency->vecInfractions = newVector;
        agency->posibleInfraction = newSize;
    }

    // Incrementar el contador si la infracción ya existe
    if (agency->vecInfractions[id - 1].infractionName != NULL) {
        agency->vecInfractions[id - 1].fineCount++;
    } else { // Crear una nueva infracción
        agency->vecInfractions[id - 1].infractionName = copyString(name);
        agency->vecInfractions[id - 1].fineCount = 1;
    }
}

    static int checkDiff(size_t max, size_t min, size_t amount){
        return amount > max || amount < min;
    }

//Set the first vec with all the infractions with ids from the infractions.csv
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
        // Crear una nueva agencia
        tAgencies *newA = calloc(1, sizeof(tAgencies));
        if (newA == NULL) {
            perror(MEMORYERROR);
            exit(MEMORYEXIT);
        }

        newA->agencyName = copyString(agency);
        // Obtener la descripción de la infracción desde el ticket
        const char *name = ticket->vecInfraction[id-1].infractionName;
        putInfractionInAgency(newA, name, id);

        // Inicializar maxFine, minFine con el primer fineAmount
        newA->maxFine = fineAmount;
        newA->minFine = fineAmount;

        newA->firstYTD = searchFineRecYTD(newA->firstYTD, year, month, fineAmount);

        // Actualizar firstDiff
        ticket->firstDiff = changeDiff(ticket->firstDiff, agency, fineAmount);

        return newA;
    }

    int c = strcasecmp(l->agencyName, agency);
    if (c == 0) {
        // Agencia encontrada
        const char * name = ticket->vecInfraction[id-1].infractionName;
        putInfractionInAgency(l, name, id);

        // Actualizar max y min
        if (fineAmount > l->maxFine){
            l->maxFine = fineAmount;
        }
        if (fineAmount < l->minFine){
           l->minFine = fineAmount; 
        } 

        // Actualizar recaudación
        l->firstYTD=searchFineRecYTD(l->firstYTD, year, month, fineAmount);
        // Actualizar diffs
        ticket->firstDiff = changeDiff(ticket->firstDiff, agency, fineAmount);
        return l;
    } else if (c < 0) {
        // La agencia actual es menor alfabéticamente, buscamos en la cola
        l->tail = findOrCreateAgency(ticket, l->tail, id, agency, fineAmount, year, month);
        return l;
    } else {
        // c > 0, insertar nueva agencia antes de l
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

    // Llamar a findOrCreateAgency con ticket, ya no necesitamos vecInf
    ticket->first = findOrCreateAgency(ticket, ticket->first, id, agency, fineAmount, year, month);
}

    // access the max price of a fine in a certain agency
    int maxFines(ticketsADT ticket, size_t id, const char * agency){
        if(ticket == NULL || agency == NULL){
            perror(INVALIDARGUMENT);
            exit(MEMORYEXIT); 
        }
        tAgencies * aux = findAgency(ticket->first,agency);
        if( aux == NULL){ // we check what the recursive call returns, to not dereference a NULL pointer
            return -1;
        }
        if( id < aux->posibleInfraction && aux->vecInfractions[id-1].infractionName != NULL){ // we check by infraction id if such infraction exists in this agency
            return aux->vecInfractions[id-1].fineCount;
        }
        return -1; // case the infractions doesnt exist
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
    // recursive funtion that returns the agency in search or NULL if not found
    static tAgenDiff * findAgencyDiff(tAgenDiff * l, const char * agency){
        int c;
        if(agency==NULL){ // if the l provided was already NULL (this funtion does NOT add agencies) 
            perror(INVALIDARGUMENT);
            exit(MEMORYEXIT);
        }
        if(l == NULL ||(c = strcasecmp(l->agencyName, agency)) == 0){ //recursion will stop if agency not found, return NULL, or if agency was found 
            return l;
        }
        return findAgencyDiff(l->tail, agency);
    }
    // access the min fine amount from an agency
    int minAgencyFines(ticketsADT ticket, const char * agency){
        if(ticket == NULL || agency == NULL){
            perror(INVALIDARGUMENT);
            exit(MEMORYEXIT); 
        }

        tAgenDiff * aux = findAgencyDiff(ticket->firstDiff, agency);
        if(aux == NULL){ //if the agency wasnt found, return -1 which would be a not found indicator
            return -1;
        }
        return aux->min;
    }

    // access the max fine amount from an agency
    int maxAgencyFines(ticketsADT ticket, const char * agency){
        if(ticket == NULL || agency == NULL){
            perror(INVALIDARGUMENT);
            exit(MEMORYEXIT); 
        }

        tAgenDiff * aux = findAgencyDiff(ticket->firstDiff, agency);
        if(aux == NULL){ //if the agency wasnt found, return -1 which would be a not found indicator
            return -1;
        }
        return aux->max;
    }
    //access the difference between the max fine and the mine fine
    int diffAgencyFines(ticketsADT ticket,const char * agency){
        if(ticket == NULL || agency == NULL){
            perror(INVALIDARGUMENT);
            exit(MEMORYEXIT); 
        }

        tAgenDiff * aux = findAgencyDiff(ticket->firstDiff, agency);
        if(aux == NULL){ //if the agency wasnt found, return -1 which would be a not found indicator
            return -1;
        }
        return aux->diff;
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
    return strcasecmp(a->agencyName, b->agencyName);  // Alphabetically in case its the same ammount
    }



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
        // Liberar el arreglo de infractions.csv (vecInfraction)
        for (size_t i = 0; i < ticket->vecSize; i++) {
            if (ticket->vecInfraction[i].infractionName != NULL) {
                free(ticket->vecInfraction[i].infractionName);
            }
        }
        free(ticket->vecInfraction); // Liberar el arreglo completo

        freeAgency(ticket->first);
        freeAgencyDiff(ticket->firstDiff);
        free(ticket);
    }

void processQuery1(ticketsADT tickets, void (*callback)(const char *, const char *, size_t, void *), void *param) {
    tAgencies *agency = tickets->first;
    while (agency) {
       // printf("Procesando agencia: %s\n", agency->agencyName);
        for (size_t i = 0; i < agency->posibleInfraction; i++) {
            if (agency->vecInfractions[i].fineCount > 0) {
               // printf("  Infracción: %s, Count: %zu\n", agency->vecInfractions[i].infractionName, agency->vecInfractions[i].fineCount);
                callback(agency->agencyName, agency->vecInfractions[i].infractionName, 
                agency->vecInfractions[i].fineCount, param);
            }
        }
        agency = agency->tail;
    }
}
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
void processQuery3(ticketsADT tickets, void (*callback)(const char *, size_t, size_t, size_t, void *), void *param) {
    tAgenDiff *sortedList = NULL;
    tAgenDiff *current = tickets->firstDiff;

    // Crear una lista temporal ordenada
    while (current != NULL) {
        tAgenDiff **insertPoint = &sortedList;
        while (*insertPoint != NULL && 
              ((*insertPoint)->diff > current->diff || 
              ((*insertPoint)->diff == current->diff && strcasecmp((*insertPoint)->agencyName, current->agencyName) < 0))) {
            insertPoint = &(*insertPoint)->tail;
        }

        // Crear nueva referencia SIN duplicar memoria
        tAgenDiff *newNode = malloc(sizeof(tAgenDiff));
        if (newNode == NULL) {
            perror(MEMORYERROR);
            exit(MEMORYEXIT);
        }
        *newNode = *current;  // Copiar estructura
        newNode->tail = *insertPoint;
        *insertPoint = newNode;

        current = current->tail;
    }

    // Imprimir datos
    current = sortedList;
    while (current != NULL) {
        callback(current->agencyName, current->min, current->max, current->diff, param);
        current = current->tail;
    }

    // Liberar memoria temporal
    while (sortedList != NULL) {    //Without this, generates memory leaks
        tAgenDiff *aux = sortedList->tail;
        free(sortedList);  // No liberar agencyName, solo estructura
        sortedList = aux;
    }
}





