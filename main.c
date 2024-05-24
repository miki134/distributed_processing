/* w main.h także makra println oraz debug -  z kolorkami! */
#include "main.h"
#include "watek_glowny.h"
#include "watek_komunikacyjny.h"
#include "util.h"
/*
 * W main.h extern int rank (zapowiedź) w main.c int rank (definicja)
 * Zwróćcie uwagę, że każdy proces ma osobą pamięć, ale w ramach jednego
 * procesu wątki współdzielą zmienne - więc dostęp do nich powinien
 * być obwarowany muteksami. Rank i size akurat są write-once, więc nie trzeba,
 * ale zob util.c oraz util.h - zmienną state_t state i funkcję changeState
 *
 */
int rank, size;
int ackCount = 0;
int clk = 0;
/* 
 * Każdy proces ma dwa wątki - główny i komunikacyjny
 * w plikach, odpowiednio, watek_glowny.c oraz (siurpryza) watek_komunikacyjny.c
 *
 *
 */

pthread_t threadKom;

void finalizuj()
{
    pthread_mutex_destroy( &stateMut);
    /* Czekamy, aż wątek potomny się zakończy */
    println("czekam na wątek \"komunikacyjny\"\n" );
    pthread_join(threadKom,NULL);
    MPI_Type_free(&MPI_PAKIET_T);
    MPI_Finalize();
}

void check_thread_support(int provided)
{
    printf("THREAD SUPPORT: chcemy %d. Co otrzymamy?\n", provided);
    switch (provided) {
        case MPI_THREAD_SINGLE: 
            printf("Brak wsparcia dla wątków, kończę\n");
            /* Nie ma co, trzeba wychodzić */
	    fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
	    MPI_Finalize();
	    exit(-1);
	    break;
        case MPI_THREAD_FUNNELED: 
            printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
	    break;
        case MPI_THREAD_SERIALIZED: 
            /* Potrzebne zamki wokół wywołań biblioteki MPI */
            printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
	    break;
        case MPI_THREAD_MULTIPLE: printf("Pełne wsparcie dla wątków\n"); /* tego chcemy. Wszystkie inne powodują problemy */
	    break;
        default: printf("Nikt nic nie wie\n");
    }
}

int main(int argc, char **argv)
{
    int rank, size, provided;
    MPI_Status status;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    check_thread_support(provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    pthread_create( &threadKom, NULL, startKomWatek , 0);

    int p = 3; // liczba przewodników
    int g = 5; // limit uczestników obsługiwanych przez przewodnika
    int n = 10; // ilość uczestników chętnych na udział w wycieczce
    State state = WAITING_FOR_REGISTER;

    if (rank < p) { // Przewodnicy
        guide(rank, n+p);
    } else { // Turyści
        tourist(rank, n+p);
    }

    MPI_Finalize();
}

// int main(int argc, char **argv)
// {
//     MPI_Status status;
//     int provided;
//     MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
//     check_thread_support(provided);
//     srand(rank);
//     /* zob. util.c oraz util.h */
//     inicjuj_typ_pakietu(); // tworzy typ pakietu
//     MPI_Comm_size(MPI_COMM_WORLD, &size);
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     /* startKomWatek w watek_komunikacyjny.c 
//      * w vi najedź kursorem na nazwę pliku i wciśnij klawisze gf
//      * powrót po wciśnięciu ctrl+6
//      * */
//     pthread_create( &threadKom, NULL, startKomWatek , 0);

//     /* mainLoop w watek_glowny.c 
//      * w vi najedź kursorem na nazwę pliku i wciśnij klawisze gf
//      * powrót po wciśnięciu ctrl+6
//      * */
//     mainLoop(); // możesz także wcisnąć ctrl-] na nazwie funkcji
// 		// działa, bo używamy ctags (zob Makefile)
// 		// jak nie działa, wpisz set tags=./tags :)
    
//     finalizuj();
//     return 0;
// }

