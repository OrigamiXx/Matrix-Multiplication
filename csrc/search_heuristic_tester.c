#include <stdio.h>
#include <stdlib.h>

#include "searcher.h"
#include "constants.h"
#include "checker.h"
#include "puzzle.h"
#include "timing.h"
#include "search_heuristic_tester.h"


int time_check_heuristic(puzzle * p, bool skip[], int heuristic_num, double * time_ptr){

    struct timespec begin={0,0}, end={0,0};
    clockid_t clock_mode = CLOCK_MONOTONIC;

    clock_gettime(clock_mode, &begin);
    
    // int nullity_search(puzzle * p, bool skip[], int skip_count, int best, int which, int heuristic_type);
    // int heuristic_result = nullity_search(p, skip, skip_2d, 0, 0, 0, heuristic_num);
    int heuristic_result = generic_h(p, skip, 0, 0, heuristic_num);
    // int heuristic_result = 0;
    clock_gettime(clock_mode, &end);

    *time_ptr = ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)begin.tv_sec + 1.0e-9*begin.tv_nsec);

    return heuristic_result;

}

int main(int argc, char ** argv) {

    bool use_random_puzzles;
    if (argc == 3) {
        use_random_puzzles = false;
    } else if (argc == 4) {
        int should_use = atoi(argv[3]);
        if (should_use == 0) {
            use_random_puzzles = false;
        } else if (should_use == 1) {
            use_random_puzzles = true;
        } else {
            fprintf(stderr, "Incorrect usage - use random puzzles should be either 0 or 1\n");
            return -1;
        }
    } else {
        fprintf(stderr, "usage: search_heuristic_tester <s> <k> [<use truly random puzzles>]\n");
        fprintf(stderr, "Optional 'use truly random puzzles':\n # 1 = use truly random puzzles\n # 2 (default) = don't use truly random puzzles\n");
        return -1;
    }

    int s = atoi(argv[1]);
    int k = atoi(argv[2]);

    printf("Beginning tests on heuristics\n");

    puzzle * test_puzzle = create_puzzle(s, k);
    // randomize_puzzle(test_puzzle);
    random_usp(test_puzzle);
    // fill puzzle with random garbage


    int number_of_heuristics = 2;
    /*
    Test 0 = nullity_h
    Test 1 = clique_h
    */


    printf("Testing on puzzle\n");
    printf("------------------\n");
    print_puzzle(test_puzzle);
    printf("------------------\n");

    for (int h = 0; h < number_of_heuristics; h++){
        
        bool init_skip[test_puzzle->max_row];
        bzero(init_skip, sizeof(init_skip));

        printf("## Testing heuristic number %d:\n", h);
        double * time_ptr = (double *) malloc(sizeof(double));
        int heuristic_result = time_check_heuristic(test_puzzle, init_skip, h, time_ptr);

        printf("  Heuristic result returned for puzzle: %d\n", heuristic_result);
        printf("  Heuristic time taken: %f\n", *time_ptr);

        free(time_ptr);


    }
}