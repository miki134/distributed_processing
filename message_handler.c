#include "main.h"
#include "watek_komunikacyjny.h"
#include "util.h"
// #include "stdlib.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

void *startMessageHandlerThread(void *ptr)
{
    MPI_Status status;
    packet_t pakiet;

    while (TRUE)
    {
        // println("%d", rank);
        MPI_Recv(&pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        pthread_mutex_lock(&clkMut);
        clk = max(pakiet.ts, clk) + 1; //?
        pthread_mutex_unlock(&clkMut);

        // println("MPI_Recv %d otrzymuje %s od %d", rank, tag2string(status.MPI_TAG), status.MPI_SOURCE);

        // return;

        switch (status.MPI_TAG)
        {
        case CHECK_REQ:
        {
            if (rank < p)
            {
                if (getState() == IN_TOUR)
                {
                    removeElement(&willingTourists, status.MPI_SOURCE);
                    enqueue(&willingTourists, status.MPI_SOURCE);

                    printQueue(&willingTourists);
                    println("rank %d kolejkuje %d", rank, status.MPI_SOURCE);
                }
                else
                    sendPacket(0, status.MPI_SOURCE, CHECK_ACK);
            }
            break;
        }
        case CHECK_ACK:
        {
            // pthread_mutex_lock(&clkMut); ???
            //            debug("Dostałem ACK od %d, mam już %d", status.MPI_SOURCE, ackCount);
            //            ackCount++;                  /* czy potrzeba tutaj muteksa? Będzie wyścig, czy nie będzie? Zastanówcie się. */
            //            pthread_mutex_lock(&clkMut); // todo: czy to nie powinno być w 25 linii?
            //            clk = max(pakiet.ts, clk) + 1;
            //            pthread_mutex_unlock(&clkMut);
            // sendPacket(0, status.MPI_SOURCE, REGISTER_ACK);
            // println("%d", getGuideId());

            if (getGuideId() == -1)
            {
                changeGuideId(status.MPI_SOURCE);
            }
            // return;

            break;
        }
        case REGISTER_REQ:
        {
            int source = status.MPI_SOURCE;
            // debug("status.MPI_TAG: %s %d %d", tag2string(status.MPI_TAG), getSize(&queue), source);
            // lockMutex(&queue);
            // printQueue(&queue);
            enqueue(&queue, source);
            // printQueue(&queue);
            // unlockMutex(&queue);
            break;
        }
        case REGISTER_ACK:
        {
            // odbiera turysta
            // jak otrzymal od tego samego przewodnika CHECK_ACK to usuwa go i przechodzi jakigoś stanu REGISTER_FAILED?
            // println("%s %s %d %d %d", tag2string(REGISTER_ACK), state2string(state), status.MPI_SOURCE, getGuideId(), getRegisterStatus());
            if (status.MPI_SOURCE == getGuideId())
            {
                // println("JEST");
                // changeRegisterStatus(REGISTER_ACCEPTED);
                changeState(WAITING_FOR_TOUR);
                // println("%d",getRegisterStatus());
            }
            break;
        }
        case START_TOUR_REQ:
        {
            // debug("status.PI_TAG: %s", tag2string(status.MPI_TAG));
            if(rank < p) {

            println("START_TOUR_REQ %d %d", rank, status.MPI_SOURCE);
            removeElement(&willingTourists, status.MPI_SOURCE);
            printQueue(&willingTourists);
            }

            if (status.MPI_SOURCE == getGuideId())
            {
                // debug("status.PI_TAG: %s", state2string(getState()));

                if (getState() == WAITING_FOR_TOUR)
                {
                    changeRegisterStatus(REGISTER_ACCEPTED);
                    sendPacket(0, status.MPI_SOURCE, START_TOUR_ACK);
                }
                else if (getState() == WAITING_FOR_REGISTER) //|| getState() == WAITING_FOR_SPOT
                {
                    println("REGISTER_FAILED %d od %d", rank, status.MPI_SOURCE);
                    changeRegisterStatus(REGISTER_FAILED);
                    changeGuideId(-1);
                }
            }
            break;
        }

        case HOSPITAL_INFO_REQ:
        {
            break;
        }

        case END_TOUR_REQ:
        {
            if (status.MPI_SOURCE == getGuideId())
            {
                changeGuideId(-1);
                sendPacket(0, status.MPI_SOURCE, END_TOUR_ACK);
            }
            break;
        }
        default:
            break;
        }
    }
}
