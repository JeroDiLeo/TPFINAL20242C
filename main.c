#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ticketsADT.h"

#define MAX_LINE 1024

// Crear archivo de salida
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
    fgets(line, sizeof(line), file); // Leer encabezado
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

        // Agregar multa directamente al ADT
        addInfraction(tickets, infractionId, agency, fineAmount, year, month);
        //printf("Multa agregada: Agencia=%s, InfractionID=%zu, Amount=%zu, Date=%s\n", agency, infractionId, fineAmount, date);
    }

    fclose(file);
    printf("Archivo de multas leído correctamente.\n");
}


 // Función callback para escribir cada resultado en el archivo
    void printQuery1(const char *agency, const char *infraction, size_t count, void *param) {
        printf("Escribiendo en Query 1: Agency=%s, Infraction=%s, Count=%zu\n", agency, infraction, count);
        fprintf((FILE *)param, "%s;%s;%zu\n", agency, infraction, count);
    }
void generateQuery1(ticketsADT tickets, const char *outputFile) {
    printf("Generando Query 1 en %s\n", outputFile);
    
    // Crear archivo de salida
    FILE *file = fopen(outputFile, "w");
    if (!file) {
        perror("Error creando archivo de salida para Query 1");
        exit(EXIT_FAILURE);
    }

    // Escribir encabezado en el archivo CSV
    fprintf(file, "Agency;Infraction;Tickets\n");

    // Procesar los datos y generar la salida usando el callback global
    processQuery1(tickets, printQuery1, file);

    fclose(file);
    printf("Query 1 generada correctamente en %s\n", outputFile);
}




int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <ticketsFile> <infractionsFile>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    ticketsADT tickets = newTicket();

    // Leer infracciones antes de procesar las multas
    readInfractionsCsv(argv[2], tickets);

    // Luego, leer las multas
    readTicketsCsv(argv[1], tickets);

    // Generar Query 1
    generateQuery1(tickets, "query1.csv");

    // Liberar memoria
    freeTicket(tickets);

    printf("Procesamiento completado.\n");
    return 0;
}
