#include "util.h"
#include "dynamic_array.h"

void guide(int rank, int peopleCount, int peoplePerTour)
{
    int *actualParticipants = (int *)malloc(peoplePerTour * sizeof(int));
    int participants = 0;

    while (TRUE)
    {
        int cp_state = getState();
        switch (cp_state)
        {
        case REST:
        {
            for (int i = 0; i <= willingTourists.size; ++i)
            {
                int id = dequeue(&willingTourists);
                sendPacket(0, id, CHECK_ACK);
            }

            changeState(WAITING_FOR_TOUR);
            break;
        }
        case WAITING_FOR_TOUR:
        {
            while ((participants < peoplePerTour) && queue.size != 0)
            {
                int participant = dequeue(&queue);
                actualParticipants[participants++] = participant;
                sendPacket(0, participant, REGISTER_ACK);
            }

            if (participants == peoplePerTour)
            {
                for (int i = 0; i <= peopleCount; ++i)
                {
                    if (i != rank)
                    {
                        sendPacket(0, i, START_TOUR_ACK);
                    }
                }

                changeState(IN_TOUR);
            }
            break;
        }
        case IN_TOUR:
        {
            sleep(TOUR_TIME);

            for (int i = 0; i <= peopleCount; ++i)
            {
                if (i != rank)
                {
                    sendPacket(0, i, END_TOUR_REQ);
                }
            }

            participants = 0;
            changeState(REST);
            break;
        }
        default:
            break;
        }

        sleep(SEC_IN_STATE);
    }
}
