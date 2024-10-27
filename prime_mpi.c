#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void send_terminator(int dest_proc) {
    int end_signal = -1;
    MPI_Send(&end_signal, 1, MPI_INT, dest_proc, 0, MPI_COMM_WORLD);
}

int main(int argc, char **argv) {
    int proc_rank, total_procs, limit;
    double exec_start, exec_end;
    int *prime_candidates = (int *)malloc((limit / 2) * sizeof(int));
    int candidate_count = 0;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &total_procs);

    if (proc_rank == 0) {
        fflush(stdout);
        scanf("%d", &limit);
    }

    
    MPI_Bcast(&limit, 1, MPI_INT, 0, MPI_COMM_WORLD);
    

    if (proc_rank == 0) {
        exec_start = MPI_Wtime();
    }


    if (proc_rank == 0) {
        for (int num = 2; num <= limit; num++) {
            bool is_prime = true;
            for (int j = 0; j < candidate_count; j++) {
                if (num % prime_candidates[j] == 0) {
                    is_prime = false;
                    break;
                }
            }
            if (is_prime) {
                prime_candidates[candidate_count++] = num;
                if (proc_rank + 1 < total_procs) {
                    MPI_Send(&num, 1, MPI_INT, proc_rank + 1, 0, MPI_COMM_WORLD);
                }
            }
        }
        if (proc_rank + 1 < total_procs) {
            send_terminator(proc_rank + 1);
        }
    } else {
        int recv_num;
        while (1) {
            MPI_Recv(&recv_num, 1, MPI_INT, proc_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (recv_num == -1) {
                if (proc_rank + 1 < total_procs) {
                    send_terminator(proc_rank + 1);
                }
                break;
            }
            bool is_prime = true;
            for (int j = 0; j < candidate_count; j++) {
                if (recv_num % prime_candidates[j] == 0) {
                    is_prime = false;
                    break;
                }
            }
            if (is_prime) {
                prime_candidates[candidate_count++] = recv_num;
                if (proc_rank + 1 < total_procs) {
                    MPI_Send(&recv_num, 1, MPI_INT, proc_rank + 1, 0, MPI_COMM_WORLD);
                }
            }
        }
    }

    int *all_prime_numbers = NULL;
    int *recv_counts = NULL;
    int *displacements = NULL;

    if (proc_rank == 0) {
        all_prime_numbers = (int *)malloc((limit / 2) * total_procs * sizeof(int));
        recv_counts = (int *)malloc(total_procs * sizeof(int));
        displacements = (int *)malloc(total_procs * sizeof(int));
    }

    MPI_Gather(&candidate_count, 1, MPI_INT, recv_counts, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (proc_rank == 0) {
        displacements[0] = 0;
        for (int i = 1; i < total_procs; i++) {
            displacements[i] = displacements[i - 1] + recv_counts[i - 1];
        }
    }

    MPI_Gatherv(prime_candidates, candidate_count, MPI_INT, all_prime_numbers, recv_counts, displacements, MPI_INT, 0, MPI_COMM_WORLD);

    if (proc_rank == 0) {
        exec_end = MPI_Wtime();
        for (int i = 0; i < displacements[total_procs - 1] + recv_counts[total_procs - 1]; i++) {
            bool is_unique = true;
            for (int j = 0; j < i; j++) {
                if (all_prime_numbers[i] == all_prime_numbers[j]) {
                    is_unique = false;
                    break;
                }
            }
            if (is_unique) {
                printf("%d ", all_prime_numbers[i]);
            }
        }
        double time=exec_end - exec_start;
        printf("\nExecution Time: %f seconds\n", time);

        free(all_prime_numbers);
        free(recv_counts);
        free(displacements);
    }

    free(prime_candidates);
    MPI_Finalize();
    return 0;
}
