#include  "CFS.h"

#define  NICE_TO_WEIGHT(nice_val) 1024/pow(1.25, nice_val)


void run_cfs(process_info processes[], int num_processes){
    /* Arrival Buffer */
    cfs_pnode *arrival_buf[num_processes];
    memset(arrival_buf, 0, (size_t)sizeof(arrival_buf));
    int buf_size = 0;


    /* Initialize RB Tree */
    struct rb_tree *tree = rb_tree_create(my_cmp_cb, my_idcmp_cb);
    if (tree == NULL) {
		fprintf(stderr, "Error: Red Black Tree for Ready Queue unable to initialize.\n");
		exit(EXIT_FAILURE);
	}
    /* ------------------ */


	int time = 0;
	while(1){
		fprintf(stdout, "<time %d>\n", time);

		/* Linear Scan through general array to find new processes which enters */
		for(int i = 0; i < num_processes; i++){
			if(processes[i].arrival_t == time){
				fprintf(stdout, "%d entered run-queue w/ Weight %f\n", processes[i].pid, NICE_TO_WEIGHT(processes[i].priority));

				cfs_pnode *arr_process = malloc(sizeof(*arr_process));
				arr_process->pid = processes[i].pid;
				arr_process->remaining_t = processes[i].service_t;
				arr_process->weight = NICE_TO_WEIGHT(processes[i].priority);
				arr_process->v_runtime = 0;
				arr_process->slice_t = 0;

				arrival_buf[buf_size] = arr_process;
				buf_size++;
			}
		}

		/* Check if it is the start of TL Window */
		if (time % TIME_LATENCY == 0){
			fprintf(stdout, "--- TL Window ---\n");
			/* Add arrival buffer items into tree */
			for(int i = 0; i < buf_size; i++){
				printf("Adding shit in\n");
				rb_tree_insert(tree, arrival_buf[i]);
			}

			printf("size of tree: %zu\n", tree->size);
			/* Clear Arrival Buffer */
    		buf_size = 0;

			/* DFS and assign time slice */
			int sum_weight = 0;
			struct rb_iter *iter = rb_iter_create();
			if (iter) {
	    		for (cfs_pnode *v = rb_iter_first(iter, tree); v; v = rb_iter_next(iter)){ 
	    			printf("summing weight of pid %d\n",v->pid);   
	    			sum_weight += v->weight;
	    		}
	    	} else {
	    		fprintf(stderr, "Iteration Error in RB tree\n");
	    	}

	    	printf("Weight Sum: %d\n", sum_weight);
	    	if (iter) {
	    		for (cfs_pnode *v = rb_iter_first(iter, tree); v; v = rb_iter_next(iter)) {
	    			printf("creating timeslice for pid %d, %f, %d\n", v->pid, v->weight, sum_weight);
	    			v->slice_t = TIME_LATENCY * (v->weight / sum_weight);
	    			printf("Timeslice: %f, %d\n", v->slice_t, v->remaining_t);
	    			rb_tree_remove(tree, v);
	    			v->remaining_t -= v->slice_t;
	    			v->v_runtime += v->slice_t;
	    			if (v->remaining_t >= 0) {
			    		rb_tree_insert(tree, v);	
	    			}
	    		}
	    	}
	    }

		/* Iterate Time and Sleep Delay */
		time++;
		CTICK;
	}


	/* Some Freeing Subroutine */

}


int my_cmp_cb (struct rb_tree *self, struct rb_node *node_a, struct rb_node *node_b) {
	(void)self;
    cfs_pnode *a = (cfs_pnode *) node_a->value;
    cfs_pnode *b = (cfs_pnode *) node_b->value;
    return (a->weight > b->weight) - (a->weight < b->weight);
}

int my_idcmp_cb (struct rb_tree *self, struct rb_node *node_a, struct rb_node *node_b) {
	(void)self;
    cfs_pnode *a = (cfs_pnode *) node_a->value;
    cfs_pnode *b = (cfs_pnode *) node_b->value;
    return (a->pid == b->pid);
}