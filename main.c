#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ticketsADT.h"

#define MAX_LINE 1024

// Creeate output
FILE *createFile(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error creando archivo de salida");
        exit(EXIT_FAILURE);
    }
    return file;
}
void readInfractionsCsv(const char *filename, ticketsADT tickets) {
    printf("Leyendo archivo de infracciones: %s\n", filename);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error abriendo archivo de infracciones");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    if (!fgets(line, sizeof(line), file)) { // Read header
        fprintf(stderr, "Error: No se pudo leer el encabezado del archivo de infracciones.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), file)) {

        char *idStr = strtok(line, ";");
        char *description = strtok(NULL, "\n");

        if (!idStr || !description) {
            printf("Línea inválida o incompleta: %s\n", line);
            continue;
        }

        size_t id = atoi(idStr);
        
        createInfraction(tickets,id,description);

        
    }

    fclose(file);
    printf("Archivo de infracciones leído correctamente.\n");
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
        fprintf(stderr, "Error: Formato de archivo de multas desconocido.\n");
        exit(EXIT_FAILURE);
    }
}

static void readTicketsCsv(const char *filename, ticketsADT tickets) {
    printf("Leyendo archivo de multas: %s\n", filename);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error abriendo archivo de multas");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    if (!fgets(line, sizeof(line), file)) {
        fprintf(stderr, "Error: No se pudo leer el encabezado del archivo de multas.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    

    int plateIdx = -1, dateIdx = -1, infractionIdx = -1, fineIdx = -1, agencyIdx = -1;
    detectTicketsCsvFormat(line, &plateIdx, &dateIdx, &infractionIdx, &fineIdx, &agencyIdx);

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0'; // Remover el salto de línea

        char *fields[5];
        int idx = 0;

        char *token = strtok(line, ";");
        while (token && idx < 5) {
            fields[idx++] = token;
            token = strtok(NULL, ";");
        }

        if (idx < 5) {
            fprintf(stderr, "Línea inválida o incompleta: %s\n", line);
            continue;
        }

        size_t infractionId = (size_t)atoi(fields[infractionIdx]);
        size_t fineAmount = (size_t)atoi(fields[fineIdx]);
        const char *agency = fields[agencyIdx];

        int year, month;
        if (sscanf(fields[dateIdx], "%d-%d", &year, &month) != 2 || year < 1900 || month < 1 || month > 12) {
            fprintf(stderr, "Fecha inválida en línea: %s\n", line);
            continue;
        }

        // Add ticket to ADT
        addInfraction(tickets, infractionId, agency, fineAmount, year, month);
    }

    fclose(file);
    printf("Archivo de multas leído correctamente.\n");
}
 // Función callback para escribir cada resultado en el archivo
    void printQuery1(const char *agency, const char *infraction, size_t count, void *param) {
        
        fprintf((FILE *)param, "%s;%s;%zu\n", agency, infraction, count);
    }
void generateQuery1(ticketsADT tickets, const char *outputFile) {
    
    
    // Crear archivo de salida
    FILE *file = fopen(outputFile, "w");
    if (!file) {
        perror("Error creando archivo de salida para Query 1");
        exit(EXIT_FAILURE);
    }

    // header of file query1.CSV
    fprintf(file, "Agency;Infraction;Tickets\n");

    // Process data
    processQuery1(tickets, printQuery1, file);

    fclose(file);
    printf("Query 1 generada correctamente en %s\n", outputFile);
}

void printQuery2(const char *agency, size_t year, size_t month, size_t ytd, void *param) {
    FILE *file = (FILE *)param;
    fprintf(file, "%s;%zu;%zu;%zu\n", agency, year, month, ytd);
}

// Generates Query 2
void generateQuery2(ticketsADT tickets, const char *outputFile) {
    

    FILE *file = fopen(outputFile, "w");
    if (!file) {
        perror("Error creando archivo de salida para Query 2");
        exit(EXIT_FAILURE);
    }
    // header of file query2.CSV
    fprintf(file, "Agency;Year;Month;YTD\n");

    // Process data
    processQuery2(tickets, printQuery2, file);

    fclose(file);
    printf("Query 2 generada correctamente en %s\n", outputFile);
}
//----------Query3-------------------------------

void printQuery3(const char *agency, size_t min, size_t max, size_t diff, void *param) {
    FILE *file = (FILE *)param;
    fprintf(file, "%s;%lu;%lu;%lu\n", agency, min, max, diff);
    
}

void generateQuery3(ticketsADT tickets, const char *outputFile) {
    

    FILE *file = fopen(outputFile, "w");
    if (!file) {
        perror("Error creando archivo de salida para Query 3");
        exit(EXIT_FAILURE);
    }
    // header of file query3.CSV
    fprintf(file, "Agency;MinAmount;MaxAmount;DiffAmount\n");

    processQuery3(tickets, printQuery3, file);

    fclose(file);
    printf("Query 3 generada correctamente en %s\n", outputFile);
}




int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <ticketsFile> <infractionsFile>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    ticketsADT tickets = newTicket();

    // read Infractions
    readInfractionsCsv(argv[2], tickets);

    // read tickets
    readTicketsCsv(argv[1], tickets);
    //Order the each Agency infractions
    sortByAlph(tickets);
   
    generateQuery1(tickets, "query1.csv");
    generateQuery2(tickets, "query2.csv");
    generateQuery3(tickets, "query3.csv");
    
    freeTicket(tickets);

    printf("Procesamiento completado.\n");
    return 0;
}
