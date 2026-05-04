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

/* La prossima funzione è accoda_studente con politica FIFO.
    Cosa succede quando un posto si libera?
La logica speculare (che dovremo scrivere nel Check-out) farà questo:

Guarda chi c'è in head.

Gli assegna il posto.

Sposta head sul secondo della fila.

Libera la memoria del vecchio nodo con free.
*/
// Funzione di supporto per registrare gli eventi nel report


void accoda_studente(CodaAttesa* coda, char* matricola, char* data, FasciaOraria fascia) {
    // 1. Creazione del "posto" in memoria
    NodoAttesa* nuovo = (NodoAttesa*)malloc(sizeof(NodoAttesa));
    if (nuovo == NULL) {
        printf("Errore di allocazione memoria!\n");
        return;
    }

    // 2. Riempimento del nodo con i dati dello studente
    strcpy(nuovo->matricola, matricola);
    strcpy(nuovo->data, data);
    nuovo->fascia = fascia;
    nuovo->next = NULL; // Essendo l'ultimo arrivato, dopo di lui non c'è nessuno

    // 3. Posizionamento nella fila (Logica Tail-Insert)
    if (coda->head == NULL) {
        // Caso A: La coda è vuota. Lo studente è sia il primo che l'ultimo.
        coda->head = nuovo;
        coda->tail = nuovo;
    } else {
        // Caso B: C'è già qualcuno. Ci colleghiamo dopo l'ultimo e diventiamo noi la nuova fine.
        coda->tail->next = nuovo; // L'attuale ultimo ora punta al nuovo arrivato
        coda->tail = nuovo;       // Il faro 'tail' si sposta sul nuovo nodo
    }

    // 4. Aggiornamento della dimensione
    coda->dimensione++;
}

void effettua_checkin(TabellaHashStudenti* anagrafica, TurnoAula* aula, CodaAttesa* coda, char* matricola) {
    
    // 1. Verifica se lo studente esiste nell'anagrafica
    if (cerca_studente(anagrafica, matricola) == NULL) {
        printf("Errore: Matricola %s non registrata nel sistema.\n", matricola);
        return;
    }

    // 2. Cerchiamo se la matricola ha una prenotazione in questo turno
    for (int i = 0; i < 100; i++) {
        // Controlliamo se il posto è prenotato da QUESTA matricola
        if (strcmp(aula->posti[i].matricola_studente, matricola) == 0 && 
            aula->posti[i].stato == PRENOTATO) {
            
            // Lo studente è arrivato! 
            // Cambia lo stato del posto i-esimo da PRENOTATO a OCCUPATO
            aula->posti[i].stato = OCCUPATO;

            printf("Check-in completato per lo studente %s al posto %d.\n", matricola, i);
            return;
        }
    }

    // 3. SE ARRIVA QUI: lo studente non aveva una prenotazione.
    // Proviamo a vedere se c'è un posto libero per un ingresso immediato.
    printf("Nessuna prenotazione trovata. Verifico posti liberi...\n");
    
    // Chiamata corretta senza tipi, passando i dati direttamente
    effettua_prenotazione(aula, coda, matricola);
}