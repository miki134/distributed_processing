#include "util.h"
#include "dynamic_array.h"

void guide(int rank, int peopleCount, int peoplePerTour)
{
    while (true)
    {
        // MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        // tu zamiast recv pobrać pierwszy komunikat z listy - format w liście musi być SOURCE ID, TYP
        switch (state)
        {
        case REST:
        {
            changeState(WAITING_FOR_TOUR);
            break;
        }
        case WAITING_FOR_TOUR:
        {
            // todo: pobierz pierwszego z kolejki lub wszystkich (?) - zarejestruj tych co możesz i usuń z kolejki

            while ((participants < peoplePerTour) || queue.size == 0)
            {
                participants++;
                sendPacket(0, status.MPI_SOURCE, REGISTER_ACK); // wyslij 1 lub wszystkim (?) że akceptujesz go w wycieczce
            }

            if (participants.size == peoplePerTour)
            {
                for (int i = 0; i <= peopleCount; ++i)
                {
                    sendPacket(0, status.MPI_SOURCE, START_TOUR_ACK);
                }

                changeState(IN_TOUR);
            }
        }
        case IN_TOUR:
        {
            sleep(TOUR_TIME);

            for (int i = 0; i <= peopleCount; ++i)
            {
                sendPacket(0, status.MPI_SOURCE, END_TOUR_REQ);
            }

            participants = 0;
            changeState(WAITING_FOR_TOUR);
        }
            // case CHECK_REQ:
            // {
            //     if (participants < g)
            //     {
            //         sendPacket(0, status.MPI_SOURCE, CHECK_ACK);
            //     }
            //     break;
            // }
            // case REGISTER_REQ:
            // {
            //     if (participants < g)
            //     {
            //         participants++;
            //         sendPacket(0, status.MPI_SOURCE, REGISTER_ACK);
            //     }

            //     if (participants.size == g)
            //     {
            //         for (int i = 0; i <= peopleCount; ++i)
            //         {
            //             sendPacket(0, status.MPI_SOURCE, REGISTER_ACK);
            //         }

            //         changeState(IN_TOUR);
            //         // sleep(TOUR_TIME);

            //         // for (int i = 0; i <= peopleCount; ++i)
            //         // {
            //         //     sendPacket(0, status.MPI_SOURCE, REGISTER_ACK);
            //         // }

            //         // participants = 0;
            //         // changeState(REST)
            //     }
            //     break;
            // }
        }

        sleep(SEC_IN_STATE);
    }
