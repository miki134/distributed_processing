#include "util.h"
#include "dynamic_array.h"

void guide(int rank, int peopleCount, int peoplePerTour)
{
    int *actualParticipants = (int *)malloc(peoplePerTour * sizeof(int));
    int participants = 0;

    while (TRUE)
    {
        int cp_state = getState();
        println("GUIDE: %s", state2string(cp_state));
        switch (cp_state)
        {
        case REST:
        {
            // debug("willingTourists: %d", willingTourists.size);
            int size = getSize(&willingTourists);
            for (int i = 0; i < size; ++i)
            {
                int id = dequeue(&willingTourists);
                sendPacket(0, id, CHECK_ACK);
            }

            changeState(WAITING_FOR_TOUR);
            break;
        }
        case WAITING_FOR_TOUR:
        {
            int size = getSize(&queue);
            debug("WAITING_FOR_TOUR: %d %d %d", size, participants, peoplePerTour);
            lockMutex(&queue);
            // printQueue(&queue);
            while ((participants < peoplePerTour) && size != 0)
            {
                debug("dequeue: %d", size);
                int participant = dequeue(&queue);
                actualParticipants[participants++] = participant;
                sendPacket(0, participant, REGISTER_ACK);
                println("%d jest na wycieczce z %d", rank, participant);
            }

            if (participants == peoplePerTour)
            {
                debug("participants == peoplePerTour: %d %d %d", size, participants, peoplePerTour);
                // printQueue(&queue);
                for (int i = 0; i < peopleCount; ++i)
                {
                    if (i != rank)
                    {
                        sendPacket(0, i, START_TOUR_REQ);
                    }
                }

                changeState(IN_TOUR);
            }

            unlockMutex(&queue);

            break;
        }
        case IN_TOUR:
        {
            // println("IN_TOUR: %s", state2string(IN_TOUR));
            // lockMutex(&queue);
            // printQueue(&queue);
            // unlockMutex(&queue);


            sleep(TOUR_TIME);

            for (int i = 0; i < peopleCount; ++i)
            {
                if (i != rank)
                {
                    sendPacket(0, i, END_TOUR_REQ);
                }
            }

            participants = 0;
            lockMutex(&queue);
            freeQueue(&queue);
            initQueue(&queue, peopleCount);
            unlockMutex(&queue);

            changeState(REST);
            break;
        }
        default:
            break;
        }

        sleep(SEC_IN_STATE);
    }
}
