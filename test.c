#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define CHECK_REQ 1
#define CHECK_ACK 2
#define REGISTER_REQ 3
#define REGISTER_ACK 4
#define START_TOUR_REQ 5
#define START_TOUR_ACK 6
#define HOSPITAL_INFO_REQ 7
#define END_TOUR_REQ 8
#define END_TOUR_ACK 9

#define MAX_GUIDES 10

typedef enum { WAITING_FOR_REGISTER, WAITING_FOR_TOUR, IN_TOUR, IN_HOSPITAL, REST } State;

int main(int argc, char** argv) {
    int rank, size, provided;
    MPI_Status status;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int p = 3; // liczba przewodników
    int g = 5; // limit uczestników obsługiwanych przez przewodnika
    int n = 10; // ilość uczestników chętnych na udział w wycieczce
    State state = WAITING_FOR_REGISTER;

    if (rank < p) { // Przewodnicy
        int participants = 0;
        bool tour_active = false;

       
    } else { // Turyści
        while (true) {
            if (state == WAITING_FOR_REGISTER) {
                for (int i = 0; i < p; i++) {
                    MPI_Send(NULL, 0, MPI_INT, i, CHECK_REQ, MPI_COMM_WORLD);
                }
                MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, CHECK_ACK, MPI_COMM_WORLD, &status);
                MPI_Send(NULL, 0, MPI_INT, status.MPI_SOURCE, REGISTER_REQ, MPI_COMM_WORLD);
                MPI_Recv(NULL, 0, MPI_INT, status.MPI_SOURCE, REGISTER_ACK, MPI_COMM_WORLD, &status);
                state = WAITING_FOR_TOUR;
            } else if (state == WAITING_FOR_TOUR) {
                MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, START_TOUR_REQ, MPI_COMM_WORLD, &status);
                state = IN_TOUR;
                MPI_Send(NULL, 0, MPI_INT, status.MPI_SOURCE, START_TOUR_ACK, MPI_COMM_WORLD);
            } else if (state == IN_TOUR) {
                // Simulate being beaten and going to hospital
                state = IN_HOSPITAL;
                MPI_Send(NULL, 0, MPI_INT, MPI_ANY_SOURCE, HOSPITAL_INFO_REQ, MPI_COMM_WORLD);
            } else if (state == REST) {
                MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, END_TOUR_REQ, MPI_COMM_WORLD, &status);
                state = REST;
                MPI_Send(NULL, 0, MPI_INT, status.MPI_SOURCE, END_TOUR_ACK, MPI_COMM_WORLD);
            }
        }
    }

    MPI_Finalize();
    return 0;
}
