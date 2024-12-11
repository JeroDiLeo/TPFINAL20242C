#include <stdio.h>
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
        // size_t dimReal;     //Dim real
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

typedef struct ticketsCDT * ticketsADT;
//Create new ticket and returns the new ticketsADT empty or NULL in case of memory error
ticketsADT newTicket(void);

//Create a vector of Id-name for infractions (Use to fill with infractions.csv)
void createInfraction(ticketsADT ticket, size_t id, const char * id_name);

/*Adds a new infraction
@param ticket - refers to city
@param id - number of identification of infraction
@param name - name description of infraction
*/
void addInfraction(ticketsADT ticket, size_t id, const char * agency, size_t fineAmount, size_t year, size_t month);

/*@Returns total amount of fines from the beginning to a certain month -Query2
@param ticket - refers to city
@param agency - Refers to the agency name
@param year - Refers to the requested year
@param month - Refers to the requested month
*/
int YTDfines(ticketsADT ticket, const char * agency,size_t year,size_t month);

/*Set the infractions in order alphabetically for query 1*/
void sortByAlph(ticketsADT ticket);

/*Set the agencies differences in descending order ???? */
//void sortDiffByDesc(ticketsADT ticket);

// frees the reserved space for the ticket
void freeTicket(ticketsADT ticket);
// function of comparison for query 3, descending order and alphabetical order
int compareDiff(const tAgenDiff *a, const tAgenDiff *b);


const char *getInfractionDescription(ticketsADT tickets, size_t id);

void processQuery1(ticketsADT tickets, void (*callback)(const char *, const char *, size_t, void *), void *param);
void processQuery2(ticketsADT tickets, void (*callback)(const char *, size_t, size_t, size_t, void *), void *param);
void processQuery3(ticketsADT tickets, void (*callback)(const char *, size_t, size_t, size_t, void *), void *param);
