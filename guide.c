#include "util.h"
#include "dynamic_array.h"

void guide(int rank, int peopleCount, int peoplePerTour)
{
    int *actualParticipants = (int *)malloc(peoplePerTour * sizeof(int));

    while (true)
    {
        switch (getState())
        {
        case REST:
        {
            changeState(WAITING_FOR_TOUR);
            break;
        }
        case WAITING_FOR_TOUR:
        {
            while ((participants < peoplePerTour) && queue.size != 0)
            {
                int participant = enqueue(queue);
                actualParticipants[participants++] = participant;
                sendPacket(0, participant, REGISTER_ACK);
            }

            if (participants.size == peoplePerTour)
            {
                for (int i = 0; i <= peoplePerTour; ++i)
                {
                    sendPacket(0, actualParticipants[i], START_TOUR_ACK);
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
        }

        sleep(SEC_IN_STATE);
    }
