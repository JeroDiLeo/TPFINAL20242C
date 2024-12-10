#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include "ticketsADT.h"

#define MAX_LINE 1024

// Crear archivo de salida
FILE *createFile(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error creating output file");
        exit(EXIT_FAILURE);
    }
    return file;
}

// Leer archivo de infracciones
void readInfractionsCsv(const char *filename, ticketsADT tickets) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening infractions file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    fgets(line, sizeof(line), file); // Saltar encabezado

    while (fgets(line, sizeof(line), file)) {
        char *idStr = strtok(line, ";");
        char *description = strtok(NULL, "\n");

        if (!idStr || !description) continue;

        size_t id = atoi(idStr);
        addInfraction(tickets, id, description, NULL, 0, 0, 0); // Agregar solo infracción
    }

    fclose(file);
}

// Leer archivo de multas
void readTicketsCsv(const char *filename, ticketsADT tickets) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening tickets file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    fgets(line, sizeof(line), file); // Saltar encabezado

    while (fgets(line, sizeof(line), file)) {
        char *plate = strtok(line, ";");
        char *date = strtok(NULL, ";");
        char *infractionIdStr = strtok(NULL, ";");
        char *fineAmountStr = strtok(NULL, ";");
        char *agency = strtok(NULL, "\n");

        if (!plate || !date || !infractionIdStr || !fineAmountStr || !agency) continue;

        size_t infractionId = atoi(infractionIdStr);
        size_t fineAmount = atoi(fineAmountStr);

        int year, month;
        sscanf(date, "%d-%d", &year, &month);

        // Buscar descripción (ya cargada previamente)
        const char *description = getInfractionDescription(tickets, infractionId);

        // Agregar multa al ADT
        addInfraction(tickets, infractionId, description, agency, fineAmount, year, month);
    }

    fclose(file);
}

// Generar Query 1
void query1(ticketsADT tickets, const char *outputFile) {
    FILE *file = createFile(outputFile);
    fprintf(file, "Agency;Infraction;Count\n");

    void printQuery1(const char *agency, const char *infraction, size_t count, void *param) {
        fprintf((FILE *)param, "%s;%s;%lu\n", agency, infraction, count);
    }

    processQuery1(tickets, printQuery1, file);
    fclose(file);
}

// Generar Query 2
void query2(ticketsADT tickets, const char *outputFile) {
    FILE *file = createFile(outputFile);
    fprintf(file, "Agency;Year;Month;YTD\n");

    void printQuery2(const char *agency, size_t year, size_t month, size_t ytd, void *param) {
        fprintf((FILE *)param, "%s;%lu;%lu;%lu\n", agency, year, month, ytd);
    }

    processQuery2(tickets, printQuery2, file);
    fclose(file);
}

// Generar Query 3
void query3(ticketsADT tickets, const char *outputFile) {
    FILE *file = createFile(outputFile);
    fprintf(file, "Agency;Min;Max;Diff\n");

    void printQuery3(const char *agency, size_t min, size_t max, size_t diff, void *param) {
        fprintf((FILE *)param, "%s;%lu;%lu;%lu\n", agency, min, max, diff);
    }

    processQuery3(tickets, printQuery3, file);
    fclose(file);
}

// Función Principal
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ticketsFile> <infractionsFile>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    ticketsADT tickets = newTicket();

    // Leer archivos de infracciones y multas
    readInfractionsCsv(argv[2], tickets);
    readTicketsCsv(argv[1], tickets);

    // Generar las consultas
    query1(tickets, "query1.csv");
    query2(tickets, "query2.csv");
    query3(tickets, "query3.csv");

    freeTicket(tickets);
    printf("Procesamiento completado.\n");
    return 0;
}
