# TPFINAL20242Q
## Primera Fecha 13/12

#### Autores:
- _Jerónimo Di Leo Muntaner 63051_
- _Joaquín Pelufo 63623_

## Queries

### Query1
Almacena en un archivo .csv el total de multas por agencia y por infracción de forma ordenada alfabéticamente por agencia y en caso de ser la misma, por orden alfabetico del nombre de la infracción.
### Query2
Almacena en un archivo .csv la recaudación YTD de un mes de una agencia, la cual consiste en la suma de los montos de todas
multas emitidas por esa agencia desde el primer mes del año hasta ese mes inclusive. Está ordenado alfabéticamente por agencia y en caso de ser la misma, por orden cronológico por año y mes.
### Query3
Almacena en un archivo .csv los montos mínimos y máximos de todas las multas emitidas y la diferencia entre ellos de una agencia. Se encuentra ordenado en forma descendente por diferencia entre ambos montos y en caso de ser el mismo monto, por orden alfabético de agencia.



### Etapa de Compilación
Para compilar el TP completo deberá acceder al directorio con los archivos fuentes, y luego correr el siguiente comando en la terminal:
```
make
```
Si lo que se busca es utilizar el TP para la ciudad de New York City,deberá correr el siguiente comando en la terminal:
```
make parkingTicketsNYC
```
Si se deseaba utilizar el TP solo para la ciudad de Chicago, deberá correr el siguiente comando en la terminal:
```
make parkingTicketsCHI
```

### Etapa de Ejecución
#### New York City
Luego de haber compilado correctamente el TP, para ejecutarlo correctamente se utiliza el siguiente comando:
```
./parkingTicketsNYC.out ticketsNYC.csv infractionsNYC.csv
```
_parkingTicketsNYC.out_ es el nombre del ejecutable,  _ticketsNYC.csv_ es el archivo.csv que contiene la información de las multas de NYC, _infractionsNYC.csv_ es el archivo que contiene la información de las infracciones.

#### Chicago
Luego de haber compilado correctamente el TP, para ejecutarlo correctamente se utiliza el siguiente comando:
```
./parkingTicketsCHI.out ticketsCHI.csv infractionsCHI.csv
```
_parkingTicketsCHI.out_ es el nombre del ejecutable,  _ticketsCHI.csv_ es el archivo.csv que contiene la información de las multas de NYC, _infractionsCHI.csv_ es el archivo que contiene la información de las infracciones.

