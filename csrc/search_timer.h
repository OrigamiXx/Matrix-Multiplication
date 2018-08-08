

#define TIME(c, depth, heuristic, label);

#define DISABLE_NEW_TIME(c);

#define STOP_TIMER(depth, heuristic, label);

void start_timer(int depth, heuristic_t ht, char * label);
void disable_new_time();
void stop_timer(int depth, heuristic_t ht, char * label);
