#ifndef UTILH
#define UTILH
#include "main.h"
#include "queue.h"

/* typ pakietu */
typedef struct {
    int ts;       /* timestamp (zegar lamporta */
    int src;  

    int data;     /* przykładowe pole z danymi; można zmienić nazwę na bardziej pasującą */
} packet_t;
/* packet_t ma trzy pola, więc NITEMS=3. Wykorzystane w inicjuj_typ_pakietu */
#define NITEMS 3

/* Typy wiadomości */
/* TYPY PAKIETÓW */
// #define ACK     1
// #define REQUEST 2
// #define RELEASE 3
// #define APP_PKT 4
// #define FINISH  5

Queue queue;
Queue willingTourists;

int participants = 0;

enum {
    CHECK_REQ  = 1,
    CHECK_ACK = 2,
    REGISTER_REQ = 3,
    REGISTER_ACK = 4,
    START_TOUR_REQ = 5,
    START_TOUR_ACK = 6,
    HOSPITAL_INFO_REQ = 7,
    END_TOUR_REQ = 8,
    END_TOUR_ACK = 9
};

typedef enum { WAITING_FOR_SPOT, WAITING_FOR_REGISTER, WAITING_FOR_TOUR, IN_TOUR, IN_HOSPITAL, REST } state_t;

typedef enum { REGISTER_INITIAL, REGISTER_ACCEPTED, REGISTER_FAILED } register_status;

extern MPI_Datatype MPI_PAKIET_T;
void inicjuj_typ_pakietu();

/* wysyłanie pakietu, skrót: wskaźnik do pakietu (0 oznacza stwórz pusty pakiet), do kogo, z jakim typem */
void sendPacket(packet_t *pkt, int destination, int tag);

extern state_t state;

extern pthread_mutex_t stateMut;
extern pthread_mutex_t clkMut;
extern pthread_mutex_t guideIdMut;

/* zmiana stanu, obwarowana muteksem */
void changeState( state_t );
state_t getState();

void changeGuideId( int );
int getGuideId();

void changeRegisterStatus( register_status );
register_status getRegisterStatus();

void guide(int rank, int peopleCount, int peoplePerTour);
void tourist(int rank, int peopleCount);

#endif
