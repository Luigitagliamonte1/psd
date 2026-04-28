#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BUCKETS 101
#define MAX_POSTI 100

// --- ANAGRAFICA STUDENTI (TABELLA HASH) ---
typedef struct {
    char matricola[12];
    char nome[60];
    char corso_di_laurea[60];
} Studente;

typedef struct NodoStudente {
    Studente dati;
    struct NodoStudente *next;
} NodoStudente;

typedef struct {
    NodoStudente *tabella[BUCKETS];
} TabellaHashStudenti;

// --- GESTIONE AULA E PRENOTAZIONI ---
typedef enum {
    MATTINA,    // 09:00 - 13:00
    POMERIGGIO, // 13:00 - 18:00
    SERA        // 18:00 - 22:00
} FasciaOraria;

typedef enum {
    LIBERO,      // Posto non occupato
    PRENOTATO,   // Prenotato, ma studente non ancora arrivato
    OCCUPATO     // Studente presente (Check-in fatto)
} StatoPosto;

typedef struct {
    int numero_posto;
    StatoPosto stato;
    char matricola_studente[12];
} Posto;

typedef struct {
    char data[11]; // "DD/MM/YYYY"
    FasciaOraria fascia;
    Posto posti[MAX_POSTI];
    int posti_occupati; // Contatore per gestire la disponibilità rapidamente
} TurnoAula;

// --- LISTA D'ATTESA (CODA FIFO) ---
typedef struct NodoAttesa {
    char matricola[12];
    char data[11];
    FasciaOraria fascia;
    struct NodoAttesa *next;
} NodoAttesa;

typedef struct {
    NodoAttesa *head; // Estrazione
    NodoAttesa *tail; // Inserimento
    int dimensione;
} CodaAttesa;

// --- STORICO E REPORT (LISTA CONCATENATA) ---
typedef enum {
    EVENTO_PRENOTAZIONE,
    EVENTO_CHECKIN,
    EVENTO_CHECKOUT,
    EVENTO_NOSHOW
} TipoEvento;

typedef struct NodoStorico {
    char data[11];
    FasciaOraria fascia;
    char matricola[12];
    TipoEvento tipo;
    struct NodoStorico *next;
} NodoStorico;

typedef struct {
    NodoStorico *head;
    int totale_eventi;
} Storico;

int calcola_hash(char* matricola) {
    unsigned long hash = 5381;
    int c;

    // Scorri la stringa carattere per carattere
    while ((c = *matricola++)) {
        // Logica: hash * 33 + c 
        // (lo shift a sinistra di 5 posizioni equivale a moltiplicare per 32)
        hash = ((hash << 5) + hash) + c; 
    }

    return (int)(hash % BUCKETS);
}

void inserisci_studente(TabellaHashStudenti* t, Studente s) {
    // 1. Calcoliamo l'indice usando la funzione hash 
    int indice = calcola_hash(s.matricola);
    
    // 2. Creiamo un nuovo nodo (un "vagoncino" della lista)
    NodoStudente* nuovo = (NodoStudente*)malloc(sizeof(NodoStudente));
    if (nuovo == NULL) {
        printf("Errore: memoria esaurita!\n");
        return;
    }

    // 3. Copiamo i dati dello studente nel nuovo nodo
    nuovo->dati = s;

    // --- COMPLETA TU ---
    // Adesso dobbiamo inserire 'nuovo' in TESTA alla lista che si trova in t->tabella[indice]
   
    
    // a. Il 'next' del nuovo nodo deve puntare a quello che attualmente è il primo 
    //    elemento del bucket (anche se è NULL)
    // nuovo->next = ... ;
    nuovo->next=t->tabella[indice];
    // b. Ora la testa del bucket deve diventare il nostro 'nuovo' nodo
    // t->tabella[indice] = ... ;
    t->tabella[indice] = nuovo;
}

void effettua_prenotazione(TurnoAula* aula, CodaAttesa* coda, char* matricola) {
    
    if (aula->posti_occupati == MAX_POSTI) { 
        // AULA PIENA: Chiamiamo l'aiutante per la coda
        printf("Aula piena! ");
        accoda_studente(coda, matricola, aula->data, aula->fascia);
        
    } else {
        // AULA DISPONIBILE: Cerchiamo il posto con il solito ciclo for
        for (int i = 0; i < MAX_POSTI; i++) {
            if (aula->posti[i].stato == LIBERO) {
                aula->posti[i].stato = PRENOTATO;
                strcpy(aula->posti[i].matricola_studente, matricola);
                aula->posti_occupati++; // Incrementiamo perché un posto è stato preso

                printf("Prenotazione confermata! Posto assegnato: %d\n", i);
                return; 
            }
        }
    }
}


