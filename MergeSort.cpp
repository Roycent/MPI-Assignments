//
// Created by roycent on 2020/3/31.
//

#include <mpi.h>
#include <random>
#include <algorithm>

#define RMAX 1000

void print_list(int *numbers, int size, int rank)
{
    std::cout << rank <<": ";
    for (int i = 0; i < size; ++i)
        std:: cout << numbers[i] << " ";
    std::cout << std::endl;
}

void MergeSort(int argc, char* argv[])
{
    int rank, comm_sz;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    int n, *numbers, local_n;

    if (rank == 0)
    {
        std::cout << "Input the value of n:" << std::endl;
        std::cin >> n;
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (n % comm_sz != 0)
    {
        MPI_Finalize();
        return;
    }
    local_n = n / comm_sz;

    numbers = new int[n];
    std::random_device rd;
    std::default_random_engine e(time(0) + rd());
    std::uniform_int_distribution<int> u(0, RMAX);
    for (int i = 0; i < local_n; ++i)
        numbers[i] = u(e);

    // output unsorted numbers
    if (rank == 0)
    {
        print_list(numbers, local_n, 0);
        int *receive = new int[local_n];
        for (int i = 1; i < comm_sz; ++i)
        {
            MPI_Recv(receive, local_n, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            print_list(receive, local_n, i);
        }
        delete receive;
    }
    else
        MPI_Send(numbers, local_n, MPI_INT, 0, 0, MPI_COMM_WORLD);

    std::sort(numbers, numbers + local_n);

    int mask = 1, partner_rank;
    bool done;

    while(!done && mask < comm_sz)
    {
        partner_rank = rank ^ mask;
        if (rank < partner_rank)
        {
            int *receive = new int[local_n];
            MPI_Recv(receive, local_n, MPI_INT, partner_rank, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int *temp = new int[local_n * 2];
            int index=0, old_index=0, rec_index=0;
            while (old_index < local_n && rec_index < local_n)
            {
                if (numbers[old_index] < receive[rec_index])
                    temp[index++] = numbers[old_index++];
                else
                    temp[index++] = receive[rec_index++];
            }
            while (old_index < local_n)
                temp[index++] = numbers[old_index++];
            while (rec_index < local_n)
                temp[index++] = receive[rec_index++];
            mask <<= 1;
            local_n *= 2;
            memcpy(numbers, temp, local_n * sizeof(int));
            delete receive;
            delete temp;
        }
        else
        {
            MPI_Send(numbers, local_n, MPI_INT, partner_rank, 1, MPI_COMM_WORLD);
            delete numbers;
            done = true;
        }
    }

    int i = 0;

    if (rank == 0)
    {
        std::cout << "The global list is:" << std::endl;
        while (i < local_n)
            std::cout << numbers[i++] << " ";
        std::cout << std::endl;
    }

    MPI_Finalize();
}
