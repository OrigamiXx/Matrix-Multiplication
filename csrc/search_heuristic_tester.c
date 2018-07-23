#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>

#include "constants.h"
#include "checker.h"
#include "puzzle.h"
#include "timing.h"
#include "search_heuristic_tester.h"
#include "searcher.h"

extern cumulative_tracker * heuristic_details;
extern int number_of_heuristics;


int time_check_heuristic(puzzle * p, bool skip[], int max_s, int heuristic_num, int test_type, double * time_ptr){

    int heuristic_results[p->max_row];
    bool ** init_graph = make_graph_from_puzzle(p, skip, 0, ipow2(3, p->k)-1, ipow2(3, p->k), -1, -1);

    int found_heuristic_result;
    std::priority_queue<heuristic_result> heuristic_queue;
    struct timespec begin={0,0}, end={0,0};
    clockid_t clock_mode = CLOCK_MONOTONIC;

    clock_gettime(clock_mode, &begin);

    switch (heuristic_num) {
        case 0:
        case 1:
        case 4:
            switch (test_type) {
                case 0:
                    found_heuristic_result = generic_h(p, skip, 0, 0, heuristic_num, 999, 0);
                    break;
                case 1:
                    found_heuristic_result = search(p, skip, 0, 0, 0, heuristic_num, max_s, true, 0);
                    break;

            }
            break;

        case 2:
        case 3:
            switch (test_type) {
                case 0:
                    {
                        found_heuristic_result = -1;
                        inline_h(p, init_graph, &heuristic_queue, skip, 0, 0, heuristic_num);
                        break;
                    }
                case 1:
                    {
                        found_heuristic_result = inline_search(p, init_graph, skip, 0, 0, 0, heuristic_num, max_s, 0);
                        break;
                    }

            }
            break;
    }

    clock_gettime(clock_mode, &end);

    *time_ptr = ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)begin.tv_sec + 1.0e-9*begin.tv_nsec);

    if ((heuristic_num == 2 || heuristic_num == 3) && test_type == 0) {
        *time_ptr = *time_ptr / p->max_row;
    }

    free_2d_bool_array(init_graph, ipow2(3, p->k));

    return found_heuristic_result;

}

int main(int argc, char ** argv) {

    bool use_random_puzzles = false; // default to false
    int test_type = 0; // default
    if (argc == 5) {
        int should_use = atoi(argv[3]);
        if (should_use == 1) {
            use_random_puzzles = true;
        } else if (should_use != 0) {
            fprintf(stderr, "Incorrect usage - use random puzzles should be either 0 or 1\n");
            return -1;
        }

        test_type = atoi(argv[4]);

        if (test_type < 0 || test_type > 1) {
            fprintf(stderr, "Incorrect usage - test type should be between 0 and 1\n");
        }

    } else if (argc != 3) {
        fprintf(stderr, "usage: search_heuristic_tester <s> <k> [<use truly random puzzles>] [<test type>]\n");
        fprintf(stderr, "Optional 'use truly random puzzles':\n # 0 (default) = don't use truly random puzzles\n # 1 = use truly random puzzles\n");
        fprintf(stderr, "Optional 'test type':\n");
        fprintf(stderr, " # 0 (default) = test based on time for heuristic calculations for an s by k puzzle\n");
        fprintf(stderr, " # 1 = test based on time to reach  when searching\n");
        fprintf(stderr, " # 2 = compare heuristic calculations for puzzles size s by 0 through s by k\n");
        fprintf(stderr, " # 3 = compare time to complete search for puzzles from width 0 through width k\n");
        return -1;
    }

    int s = atoi(argv[1]);
    int k = atoi(argv[2]);

    printf("Beginning tests on heuristics\n");

    puzzle * test_puzzle;

    if (test_type == 0) {
        test_puzzle = create_puzzle(s, k);
        // randomize_puzzle(test_puzzle);
        random_usp(test_puzzle);
        sort_puzzle(test_puzzle);

        printf("Testing on puzzle\n");
        printf("------------------\n");
        print_puzzle(test_puzzle);
        printf("------------------\n");
    } else if (test_type == 1) {
        test_puzzle = create_puzzle(0, k);
    }


    int number_of_trials = 10;
    // int number_of_heuristics = 4;
    /*
    Heuristic 0 = nullity_h
    Heuristic 1 = clique_h
    Heuristic 2 = inline degree
    Heuristic 3 = inline clique h
    */

    heuristic_timed time_results[number_of_heuristics];

    switch (test_type) {
        case 0:
        case 1:
            {
                for (int n = 0; n < number_of_trials; n++) {
                    for (int h = 0; h < number_of_heuristics; h++){
                        bool init_skip[test_puzzle->max_row];
                        bzero(init_skip, sizeof(init_skip));

                        // printf("## Testing heuristic number %d:\n", h);

                        double * time_ptr = (double *) malloc(sizeof(double));
                        int heuristic_result = time_check_heuristic(test_puzzle, init_skip, s, h, test_type, time_ptr);
                        // seems we don't care about the heuristic result any more?

                        time_results[h].t_max = MAX(time_results[h].t_max, *time_ptr);
                        time_results[h].t_min = MIN(time_results[h].t_min, *time_ptr);
                        time_results[h].t_total += *time_ptr;
                        time_results[h].num_trials ++;

                        free(time_ptr);

                        // progress bar
                        double progress_percent = (double) (((n * number_of_heuristics) + h) / (double) (number_of_trials * number_of_heuristics)) * 100;
                        printf("\r%f%% complete", progress_percent);
                        fflush(stdout);

                    }
                }
                printf("\r");
                fflush(stdout);

                printf("\n");

                printf(" ## RESULTS ## \n");
                // print out results here
                for (int h = 0; h < number_of_heuristics; h++) {
                    printf("~~ Heuristic number %d:\n", h);
                    printf(" ~ total time taken: %f\n", time_results[h].t_total);
                    printf(" ~ average time taken: %f\n", (time_results[h].t_total / time_results[h].num_trials));
                    printf(" ~ max time taken: %f\n", time_results[h].t_max);
                    printf(" ~ min time taken: %f\n", time_results[h].t_min);

                    printf(" ~~ Statistics for each level:\n");
                    for (int i = 0; i < heuristic_details[h].num_details; i ++) {
                        printf(" ~~ At level %d:\n", i);
                        printf("  ~ max heuristic: %f\n", heuristic_details[h].details[i].max_heuristic);
                        printf("  ~ min heuristic: %f\n", heuristic_details[h].details[i].min_heuristic);
                        printf("  ~ average heuristic: %f\n", (heuristic_details[h].details[i].total_heuristic / (double) heuristic_details[h].details[i].num_heuristics));
                        printf("  ~ total time: %f\n", heuristic_details[h].details[i].total_time_at_level);
                        printf("  ~ graph making time: %f\n", heuristic_details[h].details[i].graph_time_at_level);
                        printf("  ~ with %d instances:\n", heuristic_details[h].details[i].total_instances_at_level);
                        printf("    ~ average time per instance of: %f\n", (heuristic_details[h].details[i].total_time_at_level / (double) heuristic_details[h].details[i].total_instances_at_level));
                        printf("    ~ average graph making time of: %f\n", (heuristic_details[h].details[i].graph_time_at_level / (double) heuristic_details[h].details[i].total_instances_at_level));
                    }
                }

            }
            break;

        case 2:
        case 3:
            {
                for (int h = 0; h < number_of_heuristics; h++) {

                }
            }
            break;

    }




}
