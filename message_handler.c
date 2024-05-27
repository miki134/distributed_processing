#include "main.h"
#include "watek_komunikacyjny.h"
#include "util.h"
// #include "stdlib.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

void *startMessageHandlerThread(void *ptr)
{
    MPI_Status status;
    packet_t pakiet;

    while (true)
    {
        debug("czekam na recv");
        MPI_Recv(&pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch (status.MPI_TAG)
        {
        case CHECK_REQ:
        {
            //odbiera przewodnik
            // debug("Ktoś coś prosi. A niech ma!");
            debug("CHECK_REQ");
            
            if(getState() == IN_TOUR)
                enqueue(&willingTourists, status.MPI_SOURCE);
            else
                sendPacket(0, status.MPI_SOURCE, CHECK_ACK);
            
            
            break;
        }
        case CHECK_ACK:
        {
            // pthread_mutex_lock(&clkMut); ???
            debug("Dostałem ACK od %d, mam już %d", status.MPI_SOURCE, ackCount);
            ackCount++;                  /* czy potrzeba tutaj muteksa? Będzie wyścig, czy nie będzie? Zastanówcie się. */
            pthread_mutex_lock(&clkMut); // todo: czy to nie powinno być w 25 linii?
            clk = max(pakiet.ts, clk) + 1;
            pthread_mutex_unlock(&clkMut);
            // sendPacket(0, status.MPI_SOURCE, REGISTER_ACK);
            break;
        }
        case REGISTER_REQ:
        {
            enqueue(&queue, status.MPI_SOURCE);
            break;
        }
        case REGISTER_ACK:
        {
            // odbiera turysta
            // jak otrzymal od tego samego przewodnika CHECK_ACK to usuwa go i przechodzi jakigoś stanu REGISTER_FAILED?
            break;
        }
        case START_TOUR_REQ:
        {
            sendPacket(0, status.MPI_SOURCE, START_TOUR_ACK);
        }
        case HOSPITAL_INFO_REQ:
        {
        }
        case END_TOUR_REQ:
        {
            sendPacket(0, status.MPI_SOURCE, END_TOUR_ACK);
        }
        default:
            break;
        }
    }
}
