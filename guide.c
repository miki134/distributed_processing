#include "util.h"
#include "dynamic_array.h"

void guide(int rank, int peopleCount)
{
    int participants = 0;

    while (true)
    {
        MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch (status.MPI_TAG)
        {
        case CHECK_REQ:
        {
            if (participants < g)
            {
                MPI_Send(NULL, 0, MPI_INT, status.MPI_SOURCE, CHECK_ACK, MPI_COMM_WORLD);
            }
            break;
        }
        case REGISTER_REQ:
        {
            if (participants < g)
            {
                participants++;
                MPI_Send(NULL, 0, MPI_INT, status.MPI_SOURCE, REGISTER_ACK, MPI_COMM_WORLD);
            }

            if (participants.size == g)
            {
                for (int i = 0; i <= peopleCount; ++i)
                {
                    MPI_Send(NULL, 0, MPI_INT, i, START_TOUR_REQ, MPI_COMM_WORLD);
                }

                state = IN_TOUR;

                sleep(TOUR_TIME);

                for (int i = 0; i <= peopleCount; ++i)
                {
                    MPI_Send(NULL, 0, MPI_INT, i, END_TOUR_REQ, MPI_COMM_WORLD);
                }

                participants = 0;
                state = REST;
            }
            break;
        }

        sleep(SEC_IN_STATE);
    }
}
