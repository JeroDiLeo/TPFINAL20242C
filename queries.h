


/*Creates a .csv file which contains the total amount of fines per agency and for infraction.
The information is shown alphabetically in order per agency, in case of the same agency,
 it will be in order alphabetically per infraction*/
void query1(ticketsADT ticket);

/*Creates a .csv file, that contains the total amount of currency made from fines, adding each month up in alphabetical order, in tha case of working in the same agency, then it will be in chronologicl order. Example the month july,
 will have the sum of the months from january that year until july.*/
void query2(ticketsADT ticket);

/*Creates a .csv file, that contains the maximum and minmum fine cost from that agency and its according difference (max-min). This will be in descending order comparing such difference with different agency,
 in case of having the same difference, it we be in alphabetical order*/
void query3(ticketsADT ticket);