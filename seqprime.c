#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void find_primes_sequential(int limit) {
    bool *prime_flags = malloc((limit + 1) * sizeof(bool));
    
    for (int i = 2; i <= limit; i++) prime_flags[i] = true;

    for (int factor = 2; factor * factor <= limit; factor++) {
        if (prime_flags[factor]) {
            for (int multiple = factor * factor; multiple <= limit; multiple += factor) {
                prime_flags[multiple] = false;
            }
        }
    }

    for (int i = 2; i <= limit; i++) {
        if (prime_flags[i]) printf("%d ", i);
    }
    printf("\n");

    free(prime_flags);
}

int main() {
    int prime_limit;
    scanf("%d", &prime_limit);

    clock_t exec_start = clock();
    find_primes_sequential(prime_limit);
    clock_t exec_end = clock();
    
    printf("Execution Time: %f seconds\n", (double)(exec_end - exec_start) / CLOCKS_PER_SEC);

    return 0;
}
