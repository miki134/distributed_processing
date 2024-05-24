#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void tourist(int rank, int peopleCount) {
    MPI_Status status;

    int guideId = -1;

    while (true) {
        switch(state) {
            case WAITING_FOR_REGISTER:
                for (int i = 0; i < peopleCount; i++) {
                    if (i != rank) {
                        MPI_Send(NULL, 0, MPI_INT, i, CHECK_REQ, MPI_COMM_WORLD);
                    }
                }

                MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, CHECK_ACK, MPI_COMM_WORLD, &status);
                guideId = status.MPI_SOURCE;

                MPI_Recv(NULL, 0, MPI_INT, guideId, REGISTER_REQ, MPI_COMM_WORLD, &status);
                state = WAITING_FOR_TOUR;
                MPI_Send(NULL, 0, MPI_INT, guideId, REGISTER_ACK, MPI_COMM_WORLD);

                break;

            case WAITING_FOR_TOUR:
                if(guideId != -1) {
                    MPI_Recv(NULL, 0, MPI_INT, guideId, START_TOUR_REQ, MPI_COMM_WORLD, &status);
                    state = IN_TOUR;
                    MPI_Send(NULL, 0, MPI_INT, status.MPI_SOURCE, START_TOUR_ACK, MPI_COMM_WORLD);
                }
                break;

            case IN_TOUR:
                srand(time(NULL));
                int randomNumber = rand() % 10 + 1;

                if (randomNumber == 1) {
                    state = IN_HOSPITAL;
                    MPI_Send(NULL, 0, MPI_INT, MPI_ANY_SOURCE, HOSPITAL_INFO_REQ, MPI_COMM_WORLD);
                }

                MPI_Recv(NULL, 0, MPI_INT, guideId, END_TOUR_REQ, MPI_COMM_WORLD, &status);
                state = REST;
                guideId = -1;
                MPI_Send(NULL, 0, MPI_INT, guideId, END_TOUR_ACK, MPI_COMM_WORLD);

                break;

            case REST:
                sleep(10);
                state = WAITING_FOR_REGISTER;
                break;
        }
    }
}