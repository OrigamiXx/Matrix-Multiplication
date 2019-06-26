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
#include "canonization.h"

extern cumulative_tracker * heuristic_details;
extern int number_of_heuristics;
extern const char * heuristic_names[];


int time_check_heuristic(puzzle * p, bool skip[], int max_s, int heuristic_num, int test_type, double * time_ptr){

    int heuristic_results[p->max_row];
    puzzle_row max_index = ipow2(3, p->k);
    bool ** init_graph = make_graph_from_puzzle(p, skip, 0, max_index, -1, -1);

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


    int number_of_trials = 1;
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
                int time_log_buffer_size = 75;
                char time_log_name[time_log_buffer_size];
                snprintf(time_log_name, time_log_buffer_size, "search_logs/time_results--max_s=%d-k=%d", s, k);


                FILE * time_log;
                time_log = fopen(time_log_name, "w");

                time_t t = time(NULL);
                struct tm tm = *localtime(&t);

                fprintf(time_log, "Time log started at time %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                fprintf(time_log, "heuristic_type,k,time_taken,s_result\n");
                fclose(time_log);


                for (int n = 0; n < number_of_trials; n++) {

                    int log_buffer_size = 75;
                    char log_name[log_buffer_size];
                    snprintf(log_name, log_buffer_size, "search_logs/t#%d--search_run--max_s=%d-k=%d", n, s, k);
                    init_log(log_name);

                    for (int h = 1; h < number_of_heuristics; h++){

                        printf("----- BEGINNING SEARCH WITH HEURISTIC %s FOR WIDTH %d -----\n", heuristic_names[h], k);

                        bool init_skip[test_puzzle->max_row];
                        bzero(init_skip, sizeof(init_skip));

                        // printf("## Testing heuristic number %d:\n", h);

                        double * time_ptr = (double *) malloc(sizeof(double));
                        int heuristic_result = time_check_heuristic(test_puzzle, init_skip, s, h, test_type, time_ptr);
                        // seems we don't care about the heuristic result any more?

                        // time_results[h].t_max = MAX(time_results[h].t_max, *time_ptr);
                        // time_results[h].t_min = MIN(time_results[h].t_min, *time_ptr);
                        // time_results[h].t_total += *time_ptr;
                        // time_results[h].num_trials ++;

                        // progress bar
                        // double progress_percent = (double) (((n * number_of_heuristics) + h) / (double) (number_of_trials * number_of_heuristics)) * 100;
                        // printf("\r%f%% complete", progress_percent);
                        // fflush(stdout);

                        printf("----- COMPLETED SEARCH WITH HEURISTIC %s FOR WIDTH %d -----\n", heuristic_names[h], k);

                        // h_type,k|total_time ==> TO LOG FILE

                        time_log = fopen(time_log_name, "a");
                        fprintf(time_log, "heuristic_type,k,time_taken,heuristic_result_returned");
                        fprintf(time_log, "%d,%d,%e,%d\n", h, k, *time_ptr, heuristic_result);
                        fclose(time_log);

                        free(time_ptr);

                        log_current_results(true);

                        reset_isomorphs();
                    }

                    log_current_results(true);
                    wipe_statistics();

                }
                // printf("\r");
                // fflush(stdout);

                printf("\n");


                // STILL NEED SOME SHIT FROM THE TIME

                // printf(" ## RESULTS ## \n");
                // // print out results here
                // for (int h = 0; h < number_of_heuristics; h++) {
                //     printf("~~ Heuristic number %d:\n", h);
                //     printf(" ~ total time taken: %f\n", time_results[h].t_total);
                //     printf(" ~ average time taken: %f\n", (time_results[h].t_total / time_results[h].num_trials));
                //     printf(" ~ max time taken: %f\n", time_results[h].t_max);
                //     printf(" ~ min time taken: %f\n", time_results[h].t_min);

                //     printf(" ~~ Statistics for each level:\n");
                //     for (int i = 0; i < heuristic_details[h].num_details; i ++) {
                //         printf(" ~~ At level %d:\n", i);
                //         printf("  ~ max heuristic: %e\n", heuristic_details[h].details[i].max_heuristic);
                //         printf("  ~ min heuristic: %e\n", heuristic_details[h].details[i].min_heuristic);
                //         printf("  ~ average heuristic: %e\n", (heuristic_details[h].details[i].total_heuristic / (double) heuristic_details[h].details[i].num_heuristics));
                //         printf("  ~ total time: %e\n", heuristic_details[h].details[i].total_time_at_level);
                //         printf("  ~ graph making time: %e\n", heuristic_details[h].details[i].graph_time_at_level);
                //         printf("  ~ with %d instances:\n", heuristic_details[h].details[i].total_instances_at_level);
                //         printf("    ~ average time per instance of: %e\n", (heuristic_details[h].details[i].total_time_at_level / (double) heuristic_details[h].details[i].total_instances_at_level));
                //         printf("    ~ average graph making time of: %e\n", (heuristic_details[h].details[i].graph_time_at_level / (double) heuristic_details[h].details[i].total_instances_at_level));
                //     }
                // }

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
