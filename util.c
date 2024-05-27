#include "main.h"
#include "util.h"
MPI_Datatype MPI_PAKIET_T;

/*
 * w util.h extern state_t stan (czyli zapowiedź, że gdzieś tam jest definicja
 * tutaj w util.c state_t stan (czyli faktyczna definicja)
 */
state_t state = REST;

/* zamek wokół zmiennej współdzielonej między wątkami.
 * Zwróćcie uwagę, że każdy proces ma osobą pamięć, ale w ramach jednego
 * procesu wątki współdzielą zmienne - więc dostęp do nich powinien
 * być obwarowany muteksami
 */
pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t clkMut = PTHREAD_MUTEX_INITIALIZER;

struct tagNames_t
{
    const char *name;
    int tag;
} tagNames[] = {
    {"Wolne miejsca na wycieczce", CHECK_REQ},
    {"Jest wolne miejsce", CHECK_ACK},
    {"Rejestracja na wycieczke", REGISTER_REQ},
    {"Akceptacja rejestracji na wycieczke", REGISTER_ACK},
    {"Rozpoczecie wycieczki", START_TOUR_REQ},
    {"Akceptacja rozpoczecia wycieczki", START_TOUR_ACK},
    {"Pobicie przez kibiców legii", HOSPITAL_INFO_REQ},
    {"Koniec wycieczki", END_TOUR_REQ},
    {"Akceptacja konca wycieczki", END_TOUR_ACK}};
    
const char *const tag2string(int tag)
{
    for (int i = 0; i < sizeof(tagNames) / sizeof(struct tagNames_t); i++)
    {
        if (tagNames[i].tag == tag)
            return tagNames[i].name;
    }
    return "<unknown>";
}
/* tworzy typ MPI_PAKIET_T
 */
void inicjuj_typ_pakietu()
{
    /* Stworzenie typu */
    /* Poniższe (aż do MPI_Type_commit) potrzebne tylko, jeżeli
       brzydzimy się czymś w rodzaju MPI_Send(&typ, sizeof(pakiet_t), MPI_BYTE....
    */
    /* sklejone z stackoverflow */
    int blocklengths[NITEMS] = {1, 1, 1};
    MPI_Datatype typy[NITEMS] = {MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint offsets[NITEMS];
    offsets[0] = offsetof(packet_t, ts);
    offsets[1] = offsetof(packet_t, src);
    offsets[2] = offsetof(packet_t, data);

    MPI_Type_create_struct(NITEMS, blocklengths, offsets, typy, &MPI_PAKIET_T);

    MPI_Type_commit(&MPI_PAKIET_T);
}

/* opis patrz util.h */
void sendPacket(packet_t *pkt, int destination, int tag)
{
    int freepkt = 0;
    if (pkt == 0)
    {
        pkt = malloc(sizeof(packet_t));
        freepkt = 1;
    }
    pkt->src = rank;
    pkt->ts++;
    pthread_mutex_lock(&clkMut);
    clk++;
    pthread_mutex_unlock(&clkMut);
    MPI_Send(pkt, 1, MPI_PAKIET_T, destination, tag, MPI_COMM_WORLD);
    debug("Wysyłam %s do %d\n", tag2string(tag), destination);
    if (freepkt)
        free(pkt);
}

void changeState(state_t newState)
{
    pthread_mutex_lock(&stateMut);
    state = newState;
    pthread_mutex_unlock(&stateMut);
}

state_t getState()
{
    pthread_mutex_lock(&stateMut);
    state_t cp_state = state;
    pthread_mutex_unlock(&stateMut);
    return cp_state;
}
