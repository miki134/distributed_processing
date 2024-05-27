#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"

void tourist(int rank, int peopleCount)
{
    MPI_Status status;

    int guideId = -1;

    while (true)
    {
        switch (getState())
        {
            case WAITING_FOR_SPOT:
            {
                for (int i = 0; i < peopleCount; i++)
                {
                    if (i != rank)
                    {
                        MPI_Send(NULL, 0, MPI_INT, i, CHECK_REQ, MPI_COMM_WORLD);
                    }
                }

                MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, CHECK_ACK, MPI_COMM_WORLD, &status);
                guideId = status.MPI_SOURCE;
                changeState(WAITING_FOR_REGISTER);
                MPI_Send(NULL, 0, MPI_INT, guideId, REGISTER_REQ, MPI_COMM_WORLD);
                break;
            }

            case WAITING_FOR_REGISTER:
            {
                MPI_Recv(NULL, 0, MPI_INT, guideId, ANY_TAG, MPI_COMM_WORLD, &status);
                if(status.MPI_TAG == REGISTER_ACK)
                {
                    changeState(WAITING_FOR_TOUR);
                }
                else if(status.MPI_TAG == START_TOUR_REQ && status.MPI_SOURCE == guideId)
                {
                    changeState(WAITING_FOR_SPOT);
                }
                break;
            }

            case WAITING_FOR_TOUR:
            {
                if (guideId != -1)
                {
                    MPI_Recv(NULL, 0, MPI_INT, guideId, START_TOUR_REQ, MPI_COMM_WORLD, &status);
                    changeState(IN_TOUR);
                    MPI_Send(NULL, 0, MPI_INT, status.MPI_SOURCE, START_TOUR_ACK, MPI_COMM_WORLD);
                }
                break;
            }

            case IN_TOUR:
            {
                srand(time(NULL));
                int randomNumber = rand() % 10 + 1;

                if (randomNumber == 1)
                {
                    changeState(IN_HOSPITAL); // tutaj dał bym kolejny case: IN_HOSPITAL i tam sleep - a bedzie dostawać wiadomości skolejkowane na drugim wątku
                    MPI_Send(NULL, 0, MPI_INT, MPI_ANY_SOURCE, HOSPITAL_INFO_REQ, MPI_COMM_WORLD);
                }

                // case IN_HOSPITAL:

                MPI_Recv(NULL, 0, MPI_INT, guideId, END_TOUR_REQ, MPI_COMM_WORLD, &status);
                changeState(REST);
                guideId = -1;
                MPI_Send(NULL, 0, MPI_INT, guideId, END_TOUR_ACK, MPI_COMM_WORLD);

                break;
            }

            case REST:
            {
                sleep(10);
                changeState(WAITING_FOR_REGISTER);
                break;
            }
        }
    }
}