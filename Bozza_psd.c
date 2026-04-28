#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    char matricola[12];
    char nome[60];
    char corso_di_laurea[60];
}studente;

typedef enum{
    MATTINA, //es 09:00 - 13:00
    POMERIGGIO, //es 13:00 - 18:00
    SERA //es 18:00 - 22:00
}FasciaOraria;

typedef enum{
    LIBERO, // POSTO NON OCCUPATO
    PRENOTATO, // LO STUDENTE HA PRENOTATO IL POSTO, MA NON E ANCORA ARRIVATO
    OCCUPATO // LO STUDENTE HA FATTO IL CHECK-IN 
}StatoPosto;

typedef struct{
    int numero_posto;
    StatoPosto stato;
    char matricola_studente [12];
}Posto;

typedef struct{
    char data[11]; //formato "DD/MM/YYYY"
    FasciaOraria fascia;
    Posto posti[100];
    int posti_disponibili; //contatore da incrementare e decrementare   
}TurnoAula;

typedef struct NodoAttesa{
    char matricola[12];
    char data[11];
    FasciaOraria fascia;
    struct NodoAttesa *next; //putatore al prossimo studente in fila
}NodoAttesa;

typedef  struct{
    NodoAttesa *heade; //estrazione
    NodoAttesa *tail; // inserimento
    int dimensione;
}CodaAttesa;

typedef enum{
    evento_prenotazione,
    evento_checkin,
    evento_checkout,
    evento_noshow
}TipoEvento;

typedef struct NodoStorico{
    char data[11];
    FasciaOraria fascai;
    char matricola[12];
    TipoEvento tipo;
    struct NodoStorico *next;
}nodostorico;
