#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ticketsADT.h"

#define MAX_LINE 1024

// Creates exit file
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
    if (!fgets(line, sizeof(line), file)) { // Leer encabezado
        fprintf(stderr, "Error: No se pudo leer el encabezado del archivo de infracciones.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    printf("Encabezado leído: %s", line);

    if (strncmp(line, "id;description", 14) != 0) {
        fprintf(stderr, "Error: El archivo de infracciones no tiene el formato esperado.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), file)) {
        printf("Línea leída: %s", line);

        char *idStr = strtok(line, ";");
        char *description = strtok(NULL, "\n");

        if (!idStr || !description) {
            printf("Línea inválida o incompleta: %s\n", line);
            continue;
        }

        size_t id = atoi(idStr);
        printf("Creando infracción: ID=%zu, Description=%s\n", id, description);
        createInfraction(tickets,id,description);

        printf("Infracción agregada: ID=%zu, Description=%s\n", id, description);
    }

    fclose(file);
    printf("Archivo de infracciones leído correctamente.\n");
}

void readTicketsCsv(const char *filename, ticketsADT tickets) {
    printf("Leyendo archivo de multas: %s\n", filename);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error abriendo archivo de multas");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    fgets(line, sizeof(line), file); // reads header
    printf("Encabezado: %s\n", line);

    while (fgets(line, sizeof(line), file)) {
        //printf("Línea leída: %s", line);

        char *plate = strtok(line, ";");
        char *date = strtok(NULL, ";");
        char *infractionIdStr = strtok(NULL, ";");
        char *fineAmountStr = strtok(NULL, ";");
        char *agency = strtok(NULL, "\n");

        if (!plate || !date || !infractionIdStr || !fineAmountStr || !agency) {
            printf("Línea inválida o incompleta: %s\n", line);
            continue;
        }

        size_t infractionId = atoi(infractionIdStr);
        size_t fineAmount = atoi(fineAmountStr);

        int year, month;
        sscanf(date, "%d-%d", &year, &month);

        // Add ticket on the ADT
        addInfraction(tickets, infractionId, agency, fineAmount, year, month);
        //printf("Multa agregada: Agencia=%s, InfractionID=%zu, Amount=%zu, Date=%s\n", agency, infractionId, fineAmount, date);
    }

    fclose(file);
    printf("Archivo de multas leído correctamente.\n");
}


void printQuery1(const char *agency, const char *infraction, size_t count, void *param) {
    printf("Escribiendo en Query 1: Agency=%s, Infraction=%s, Count=%zu\n", agency, infraction, count);
    fprintf((FILE *)param, "%s;%s;%zu\n", agency, infraction, count);
}

void generateQuery1(ticketsADT tickets, const char *outputFile) {
    printf("Generando Query 1 en %s\n", outputFile);
    
    // Creates output file
    FILE *file = fopen(outputFile, "w");
    if (!file) {
        perror("Error creando archivo de salida para Query 1");
        exit(EXIT_FAILURE);
    }

    // Writes header on the CSV file
    fprintf(file, "Agency;Infraction;Tickets\n");

    // Process the data and generates the output
    processQuery1(tickets, printQuery1, file);

    fclose(file);
    printf("Query 1 generada correctamente en %s\n", outputFile);
}
// Callback for Query 2
void printQuery2(const char *agency, size_t year, size_t month, size_t ytd, void *param) {
    FILE *file = (FILE *)param;
    printf("Escribiendo en Query 2: %s, Año=%zu, Mes=%zu, Multas Acumuladas=%zu\n", agency, year, month, ytd);
    fprintf(file, "%s;%zu;%zu;%zu\n", agency, year, month, ytd);
}

// Generates Query 2
void generateQuery2(ticketsADT tickets, const char *outputFile) {
    printf("Generando Query 2 en %s\n", outputFile);

    FILE *file = fopen(outputFile, "w");
    if (!file) {
        perror("Error creando archivo de salida para Query 2");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "Agency;Year;Month;AccumulatedFines\n");

    // Process the data and creates the csv for Query 2
    processQuery2(tickets, printQuery2, file);

    fclose(file);
    printf("Query 2 generada correctamente en %s\n", outputFile);
}




int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <ticketsFile> <infractionsFile>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    ticketsADT tickets = newTicket();

    // reads the infraction file
    readInfractionsCsv(argv[2], tickets);

    // Reads the tickets file
    readTicketsCsv(argv[1], tickets);
    //Order each Agency infractions
    sortByAlph(tickets);
    // Generates Query 1
    generateQuery1(tickets, "query1.csv");

    // Generates Query 2
    generateQuery2(tickets, "query2.csv");
    
    freeTicket(tickets);

    printf("Procesamiento completado.\n");
    return 0;
}
