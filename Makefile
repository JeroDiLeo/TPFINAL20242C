CC = gcc
ALL_CFLAGS = -pedantic -Wall -std=c99 -fsanitize=address -g
all: parkingTicketsNYC parkingTicketsCHI
parkingTicketsNYC: main.c ticketsADT.c
	$(CC) $(ALL_CFLAGS) -o parkingTicketsNYC.out $^

parkingTicketsCHI: main.c ticketsADT.c
	$(CC) $(ALL_CFLAGS) -o parkingTicketsCHI.out $^

clean:
	rm -f parkingTicketsNYC.out parkingTicketsCHI.out