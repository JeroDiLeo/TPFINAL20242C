#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

typedef struct ticketsCDT * ticketsADT;

//Create new ticket and returns the new ticketsADT empty or NULL in case of memory error
ticketsADT newTicket(void);

//Create a vector of Id-name for infractions (Use to fill with infractions.csv)
void createInfraction(ticketsADT ticket, size_t id, const char * id_name);

/*Adds a new infraction
@param ticket - refers to city ADT
@param id - number of identification of infraction
@param agency - name  of agency
@param fineAmount - the amount cost of the fine
@param year - year of fine
@param month - month of fine
*/
void addInfraction(ticketsADT ticket, size_t id, const char * agency, size_t fineAmount, size_t year, size_t month);

/*Returns total amount of fines from the beginning to a certain month - Query2
@param ticket - refers to city ADT
@param agency - Refers to the agency name
@param year - Refers to the requested year
@param month - Refers to the requested month
*/
int YTDfines(ticketsADT ticket, const char * agency,size_t year,size_t month);

/*Set the infractions in order alphabetically for query 1*/
void sortByAlph(ticketsADT ticket);

// frees the reserved space for the ticket
void freeTicket(ticketsADT ticket);

/*Gives the description of an infraction from its ID
@param tickets -refers to city ADT
@param id - umber of identification of infraction
*/
const char *getInfractionDescription(ticketsADT tickets, size_t id);

/* Process Query 1 by counting the number of infractions per agency.
@param tickets - The ticketsADT structure containing all ticket data.
@param callback - A function pointer that handles the result of each processed infraction.
@param param - An additional parameter passed to the callback function.
*/
void processQuery1(ticketsADT tickets, void (*callback)(const char *, const char *, size_t, void *), void *param);

/*Process Query 2 by calculating the year-to-date (YTD) accumulated fines for each agency.
@param tickets - The ticketsADT structure containing all ticket data.
@param callback - A function pointer that handles the result of each processed YTD fine.
@param param - An additional parameter passed to the callback function.
*/
void processQuery2(ticketsADT tickets, void (*callback)(const char *, size_t, size_t, size_t, void *), void *param);

/* Process Query 3 by determining the minimum, maximum, and difference in fines for each agency.
@param tickets - The ticketsADT structure containing all ticket data.
@param callback - A function pointer that handles the result of each processed agency difference.
@param param - An additional parameter passed to the callback function.
*/
void processQuery3(ticketsADT tickets, void (*callback)(const char *, size_t, size_t, size_t, void *), void *param);
