#include "util.h"
#include "dynamic_array.h"

void guide(int rank, int peopleCount, int peoplePerTour)
{
    Queue actualParticipants;
    initQueue(&actualParticipants, peopleCount);
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
            // if (size > 0)
            // {
            //     println("rank: %d", rank);
            //     printQueue(&willingTourists);
            // }

            for (int i = 0; i < size; ++i)
            {
                debug("dequeue: %d", i);
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
            // lockMutex(&queue);
            // printQueue(&queue);
            while ((participants < peoplePerTour) && size != 0)
            {
                debug("dequeue: %d", size);
                int participant = dequeue(&queue);
                participants++;
                enqueue(&actualParticipants, participant);
                sendPacket(0, participant, REGISTER_ACK);
                println("%d będzie na wycieczce z %d", rank, participant);
                size = getSize(&queue);
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

            // unlockMutex(&queue);

            break;
        }
        case IN_TOUR:
        {
            // println("IN_TOUR: %s", state2string(IN_TOUR));
            // lockMutex(&queue);
            // printQueue(&queue);
            // unlockMutex(&queue);
            char * str = toString(&actualParticipants);
            println("Na wycieczce z: %s", str);
            // printQueue(&actualParticipants);
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

            lockMutex(&actualParticipants);
            freeQueue(&actualParticipants);
            initQueue(&actualParticipants, peopleCount);
            unlockMutex(&actualParticipants);

            changeState(REST);
            break;
        }
        default:
            break;
        }

        sleep(SEC_IN_STATE);
    }
}
