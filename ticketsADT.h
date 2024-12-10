

#include <stdio.h>

typedef struct ticketsCDT * ticketsADT;
//Create new ticket and returns the new ticketsADT empty or NULL in case of memory error
ticketsADT newTicket(void);

/*Adds a new infraction
@param ticket - refers to city
@param id - number of identification of infraction
@param name - name description of infraction
*/
void addInfraction(ticketsADT ticket, size_t id, const char * name, const char * agency, size_t fineAmount, size_t year, size_t month);

/*Generates more space for the array of infractions
@param ticket - refers to the city
*/
void resize(ticketsADT ticket);

/*@Returns amount of fines of an agency(Count)-Query1
@param ticket - refers to city
@param id - Identification of infraction
@param agency - Refers to the agency name
*/
int maxFines(ticketsADT ticket, size_t id, const char * agency);

/*@Returns total amount of fines from the beginning to a certain month -Query2
@param ticket - refers to city
@param agency - Refers to the agency name
@param year - Refers to the requested year
@param month - Refers to the requested month
*/
int YTDfines(ticketsADT ticket, const char * agency,size_t year,size_t month);

/*@Returns minimun amount of fine from an agency
@param ticket - refers to city
@param agency - Refers to the agency name
*/
int minAgencyFines(ticketsADT ticket, const char * agency);

/*@Returns maximun amount of fine from an agency
@param ticket - refers to city
@param agency - Refers to the agency name
*/
int maxAgencyFines(ticketsADT ticket, const char * agency);

/*@Returns difference amount of fine from an agency
@param ticket - refers to city
@param agency - Refers to the agency name
*/
int diffAgencyFines(ticketsADT ticket,const char * agency);

/*Set the infractions in order alphabetically*/
void sortByAlph(ticketsADT ticket);

/*Set the agencies differences in descending order*/
void sortDiffByDesc(ticketsADT ticket);

// frees the reserved space for the ticket
void freeTicket(ticketsADT ticket);

const char *getInfractionDescription(ticketsADT tickets, size_t infractionId);

void processQuery1(ticketsADT tickets, void (*callback)(const char *, const char *, size_t, void *), void *param);
// void processQuery2(ticketsADT tickets, void (*callback)(const char *, size_t, size_t, size_t, void *), void *param);
// void processQuery3(ticketsADT tickets, void (*callback)(const char *, size_t, size_t, size_t, void *), void *param);
