#include "simulator.h"
#define INIT_P_CAP 1


void create_scheduler(FILE* file, char* algo) {
	/* Create Global Process Structure */
	int plist_capacity = INIT_P_CAP;
	int plist_size     = 0;

	process_info *processes = malloc(sizeof(*processes) * plist_size);

	while (!feof(file)){
		process_info process;
		fscanf(file, "%d %d %d %d", &process.pid, &process.arrival_t, &process.service_t, &process.priority);

		if (plist_size == plist_capacity){
			plist_capacity = plist_capacity * 2;
			processes = realloc(processes, plist_capacity * sizeof(*processes));
			assert(processes != NULL);
		}

		processes[plist_size] = process;
		plist_size++;
	}

	/* Create Scheduler */
	if (strcmp(algo, "CFS") == 0){
		fprintf(stdout, "Linux Completely Fair Scheduling Running\n");
		run_cfs(processes);
	} else if (strcmp(algo, "RR") == 0){
		fprintf(stdout, "Round Robin Scheduling Running\n");
	} else {
		fprintf(stderr, "ERROR: Scheduling Algorithm not recognized.\n");
		free(processes);
		exit(EXIT_FAILURE);
	}


	free(processes);
	return;	
}