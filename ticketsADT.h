

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
size_t maxFines(ticketsADT ticket, size_t id, const char * agency);

/*@Returns total amount of fines from the beginning to a certain month -Query2
@param ticket - refers to city
@param agency - Refers to the agency name
@param year - Refers to the requested year
@param month - Refers to the requested month
*/
size_t YTDfines(ticketsADT ticket, const char * agency,size_t year,size_t month);

/*@Returns minimun amount of fine from an agency
@param ticket - refers to city
@param agency - Refers to the agency name
*/
size_t minAgencyFines(ticketsADT ticket, const char * agency);

/*@Returns maximun amount of fine from an agency
@param ticket - refers to city
@param agency - Refers to the agency name
*/
size_t maxAgencyFines(ticketsADT ticket, const char * agency);

/*@Returns difference amount of fine from an agency
@param ticket - refers to city
@param agency - Refers to the agency name
*/
size_t diffAgencyFines(ticketsADT ticket,const char * agency);

/*Set the infractions in order alphabetically*/
void sortByAlph(ticketsADT ticket);

/*Set the agencies differences in descending order*/
void sortDiffByDesc(ticketsADT ticket);