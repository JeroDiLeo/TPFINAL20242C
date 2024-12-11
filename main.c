#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ticketsADT.h"
#define OUTFILEERROR "Error in creating output file"
#define OPENINFFILEERROR "Error opening infractions file"
#define OPENFILEFINEERROR " Error opening the fine file"
#define OUTPUTERRORQ1 "Error in creating output file for Query 1"
#define OUTPUTERRORQ2 "Error in creating output file for Query 2"
#define OUTPUTERRORQ3 "Error in creating output file for Query 3"

#define MAX_LINE 1024

// Create output
FILE *createFile(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror(OUTFILEERROR);
        exit(EXIT_FAILURE);
    }
    return file;
}
void readInfractionsCsv(const char *filename, ticketsADT tickets) {
    printf("reading infractions file: %s\n", filename);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror(OPENINFFILEERROR);
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    if (!fgets(line, sizeof(line), file)) { // Read header
        fprintf(stderr, "Error: Could not read the headers of infraction file \n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), file)) {

        char *idStr = strtok(line, ";");
        char *description = strtok(NULL, "\n");

        if (!idStr || !description) {
            printf("Invalid or incomplete line: %s\n", line);
            continue;
        }

        size_t id = atoi(idStr);
        
        createInfraction(tickets,id,description);

        
    }

    fclose(file);
    printf("Infraction file read successfully.\n");
}
//Detects the format of the tickets file
static void detectTicketsCsvFormat(const char *header,int *plateIdx, int *dateIdx, int *infractionIdx, int *fineIdx, int *agencyIdx){
    if (strstr(header, "plate;issueDate;infractionId;fineAmount;issuingAgency")) {
        // Format NYC
        *plateIdx = 0;
        *dateIdx = 1;
        *infractionIdx = 2;
        *fineIdx = 3;
        *agencyIdx = 4;
    } else if (strstr(header, "issueDate;plateRedacted;unitDescription;infractionCode;fineLevel1Amount")) {
        // Format CHI
        
        *dateIdx = 0;
        *plateIdx = 1; 
        *agencyIdx = 2; 
        *infractionIdx = 3; 
        *fineIdx = 4;
    } else {
        fprintf(stderr, "Error: Unknown format for fine file.\n");
        exit(EXIT_FAILURE);
    }
}

static void readTicketsCsv(const char *filename, ticketsADT tickets) {
    printf("Reading fine file: %s\n", filename);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror(OPENFILEFINEERROR);
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    if (!fgets(line, sizeof(line), file)) {
        fprintf(stderr, "Erorr: Could not read the header of fine file.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    

    int plateIdx = -1, dateIdx = -1, infractionIdx = -1, fineIdx = -1, agencyIdx = -1;
    detectTicketsCsvFormat(line, &plateIdx, &dateIdx, &infractionIdx, &fineIdx, &agencyIdx);

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0'; // Remove any line jump

        char *fields[5];
        int idx = 0;

        char *token = strtok(line, ";");
        while (token && idx < 5) {
            fields[idx++] = token;
            token = strtok(NULL, ";");
        }

        if (idx < 5) {
            fprintf(stderr, "Invalid or incomplete line: %s\n", line);
            continue;
        }

        size_t infractionId = (size_t)atoi(fields[infractionIdx]);
        size_t fineAmount = (size_t)atoi(fields[fineIdx]);
        const char *agency = fields[agencyIdx];

        int year, month;
        if (sscanf(fields[dateIdx], "%d-%d", &year, &month) != 2 || year < 1900 || month < 1 || month > 12) {
            fprintf(stderr, "Invalid date in line: %s\n", line);
            continue;
        }

        // Add ticket to ADT
        addInfraction(tickets, infractionId, agency, fineAmount, year, month);
    }

    fclose(file);
    printf("Fine file read successfully.\n");
}
//----------Query1-------------------------------

 // Function callback to copy results in the file
    void printQuery1(const char *agency, const char *infraction, size_t count, void *param) {
        
        fprintf((FILE *)param, "%s;%s;%zu\n", agency, infraction, count);
    }
void generateQuery1(ticketsADT tickets, const char *outputFile) {
    
    
    // create output file
    FILE *file = fopen(outputFile, "w");
    if (!file) {
        perror(OUTPUTERRORQ1);
        exit(EXIT_FAILURE);
    }

    // header of file query1.CSV
    fprintf(file, "Agency;Infraction;Tickets\n");

    // Process the data
    processQuery1(tickets, printQuery1, file);

    fclose(file);
    printf("Query 1 successfully generated in: %s\n", outputFile);
}
//----------Query2-------------------------------

void printQuery2(const char *agency, size_t year, size_t month, size_t ytd, void *param) {
    FILE *file = (FILE *)param;
    fprintf(file, "%s;%zu;%zu;%zu\n", agency, year, month, ytd);
}

// Generates Query 2
void generateQuery2(ticketsADT tickets, const char *outputFile) {
    

    FILE *file = fopen(outputFile, "w");
    if (!file) {
        perror(OUTPUTERRORQ2);
        exit(EXIT_FAILURE);
    }
    // header of file query2.CSV
    fprintf(file, "Agency;Year;Month;YTD\n");

    // Process the data
    processQuery2(tickets, printQuery2, file);

    fclose(file);
    printf("Query 2 generated successfully in: %s\n", outputFile);
}
//----------Query3-------------------------------

void printQuery3(const char *agency, size_t min, size_t max, size_t diff, void *param) {
    FILE *file = (FILE *)param;
    fprintf(file, "%s;%lu;%lu;%lu\n", agency, min, max, diff);
    
}

void generateQuery3(ticketsADT tickets, const char *outputFile) {
    

    FILE *file = fopen(outputFile, "w");
    if (!file) {
        perror(OUTPUTERRORQ3);
        exit(EXIT_FAILURE);
    }
    // header of file query3.CSV
    fprintf(file, "Agency;MinAmount;MaxAmount;DiffAmount\n");

    processQuery3(tickets, printQuery3, file);

    fclose(file);
    printf("Query 3 generated successfully in: %s\n", outputFile);
}




int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Use: %s <ticketsFile> <infractionsFile>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    ticketsADT tickets = newTicket();

    // reads Infractions
    readInfractionsCsv(argv[2], tickets);

    // reads  tickets
    readTicketsCsv(argv[1], tickets);
    //sort each infraction in alphabetical order
    sortByAlph(tickets);
   
    generateQuery1(tickets, "query1.csv");
    generateQuery2(tickets, "query2.csv");
    generateQuery3(tickets, "query3.csv");
    
    freeTicket(tickets);

    printf("Successfully finished task! \n");
    return 0;
}
