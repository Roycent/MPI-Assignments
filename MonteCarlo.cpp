//
// Created by roycent on 2020/4/2.
//

//
// Created by roycent on 2020/3/24.
//

#include <mpi.h>
#include <random>

int main(int argc, char* argv[])
{
    long long local_number_in_cycle, total_number_in_cycle;
    long long total_times, local_times;
    int rank, comm_sz;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    //Get input
    if(rank == 0)
    {
        std::cout << "Enter the total number of tosses" << std::endl;
        std::cin >> total_times;
    }

    //Broadcast the number
    MPI_Bcast(&total_times, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    //Processes' work
    local_times = total_times / comm_sz;
    double x, y, distance_squared;
    //Init the random engine
    std::random_device rd;
    std::default_random_engine e(time(0) + rd());
    std::uniform_real_distribution<double> u(-1.0, 1.0);
    local_number_in_cycle = 0;
    //Calculate
    for(long long i = 0; i < local_times; ++i)
    {
        x = u(e);
        y = u(e);
        distance_squared = x * x + y * y;
        if(distance_squared <= 1)
            local_number_in_cycle ++;
    }

    //Reduce the results
    MPI_Reduce(&local_number_in_cycle, &total_number_in_cycle, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    //Output the result
    if(rank == 0)
        std::cout << (double)total_number_in_cycle/(double)total_times * 4 << std::endl;

    MPI_Finalize();
}