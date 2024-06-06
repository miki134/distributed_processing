#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"

void tourist(int rank, int peopleCount)
{
    while (TRUE)
    {
        int cp_state = getState();
        println("TURIST: %s", state2string(cp_state));
        switch (cp_state)
        {
        case REST:
        {
            for (int i = 0; i < peopleCount; i++)
            {
                if (i != rank)
                {
                    sendPacket(0, i, CHECK_REQ);
                    // MPI_Send(NULL, 0, MPI_INT, i, CHECK_REQ, MPI_COMM_WORLD);
                }
            }
            changeState(WAITING_FOR_SPOT);
            break;
        }

        case WAITING_FOR_SPOT:
        {
            int guideId = getGuideId();
            if (guideId != -1)
            {
                changeState(WAITING_FOR_REGISTER);
                sendPacket(0, guideId, REGISTER_REQ);
            }

            //                MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, CHECK_ACK, MPI_COMM_WORLD, &status);
            //                guideId = status.MPI_SOURCE;
            //
            //                MPI_Send(NULL, 0, MPI_INT, guideId, REGISTER_REQ, MPI_COMM_WORLD);
            break;
        }

        case WAITING_FOR_REGISTER:
        {
            // println("%d", getRegisterStatus());
            if (getRegisterStatus() == REGISTER_ACCEPTED)
            {
                changeState(WAITING_FOR_TOUR);
                changeRegisterStatus(REGISTER_INITIAL);
            }
            else if (getRegisterStatus() == REGISTER_FAILED)
            {
                changeState(REST);
                changeRegisterStatus(REGISTER_INITIAL);
            }
            //                MPI_Recv(NULL, 0, MPI_INT, guideId, ANY_TAG, MPI_COMM_WORLD, &status);
            //                if(status.MPI_TAG == REGISTER_ACK)
            //                {
            //                    changeState(WAITING_FOR_TOUR);
            //                }
            //                else if(status.MPI_TAG == START_TOUR_REQ && status.MPI_SOURCE == guideId)
            //                {
            //                    changeState(REST);
            //                }
            break;
        }

        case WAITING_FOR_TOUR:
        {
            if (getRegisterStatus() == REGISTER_ACCEPTED)
            {
                changeState(IN_TOUR);
                // changeRegisterStatus(REGISTER_INITIAL);
            }
            //                if (guideId != -1)
            //                {
            //                    MPI_Recv(NULL, 0, MPI_INT, guideId, START_TOUR_REQ, MPI_COMM_WORLD, &status);
            //                    changeState(IN_TOUR);
            //                    MPI_Send(NULL, 0, MPI_INT, status.MPI_SOURCE, START_TOUR_ACK, MPI_COMM_WORLD);
            //                }
            break;
        }

        case IN_HOSPITAL:
        {
            // sendPacket(0, MPI_ANY_SOURCE, HOSPITAL_INFO_REQ);
            println("Pobity %d! Jest w szpitalu!", rank);
            for (int i = 0; i < peopleCount; i++)
            {
                if (i != rank)
                {
                    sendPacket(0, i, HOSPITAL_INFO_REQ);
                }
            }
            sleep(HOSPITAL_TIME);
            changeGuideId(-1);
            changeState(REST);
            changeRegisterStatus(REGISTER_INITIAL);
            break;
        }

        case IN_TOUR:
        {
            srand(time(NULL));
            int randomNumber = rand() % 20 + 1;
            
            // println("%s %d", state2string(IN_TOUR), getGuideId());
            if (rank == randomNumber)
            {
                changeState(IN_HOSPITAL);
            }

            if (getGuideId() == -1)
            {
                changeState(REST);
                changeRegisterStatus(REGISTER_INITIAL);
            }
            break;
        }
        }
        sleep(SEC_IN_STATE);
    }
}