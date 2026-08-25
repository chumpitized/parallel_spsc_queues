#include "parallel_queue.h"
#include "cached_parallel_queue.h"
#include "thread.h"
#include "time.h"

#include <stdio.h>
#include <stdlib.h>

#define MSG_TOTAL 1000000
#define QUEUE_CAPACITY 1 << 20

void produce_messages_pq(void *pq) {
	ParallelQueue *q = (ParallelQueue *)pq;

	u64 counter = 0;
	while (counter != MSG_TOTAL) {
		u64 sequence = counter;

		bool produced = parallel_queue_produce(q, &sequence);
		if (produced) {
			counter++;
		}
	}
}

void consume_messages_pq(void *pq) {
	ParallelQueue *q = (ParallelQueue *)pq;

	u64 counter = 0;
	while (counter != MSG_TOTAL) {
		u64 sequence;
		bool consumed = parallel_queue_consume(q, &sequence);
		if (consumed) {
			if (counter != sequence) {
				fprintf(stderr, "Sequence doesn't match count!"); 
				abort();
			}
			counter++;
		}
	}
}

void produce_messages_cq(void *cq) {
	CachedParallelQueue *q = (CachedParallelQueue *)cq;

	u64 counter = 0;
	while (counter != MSG_TOTAL) {
		u64 sequence = counter;

		bool produced = cached_parallel_queue_produce(q, &sequence);
		if (produced) {
			counter++;
		}
	}
}

void consume_messages_cq(void *cq) {
	CachedParallelQueue *q = (CachedParallelQueue *)cq;

	u64 counter = 0;
	while (counter != MSG_TOTAL) {
		u64 sequence;
		bool consumed = cached_parallel_queue_consume(q, &sequence);
		if (consumed) {
			if (counter != sequence) {
				fprintf(stderr, "Sequence doesn't match count!"); 
				abort();
			}
			counter++;
		}
	}
}

typedef void (*QueueFunc)(void *);

void run_queue(void *queue, QueueFunc produce, QueueFunc consume, char *name, int num_of_runs) {
	double avg = 0.0;

	double total_begin = get_current_time();

	for (int i = 0; i < num_of_runs; ++i) {
		double begin = get_current_time();
		Thread producer = create_thread(produce, queue);
		Thread consumer = create_thread(consume, queue);

		join_thread(producer);
		detach_thread(producer);

		join_thread(consumer);
		detach_thread(consumer);
		
		double end = get_current_time();
		avg += end - begin;
	}

	double total_end = get_current_time();

	double avg_total = avg / (double)num_of_runs;
	double total = total_end - total_begin;

	printf("[%s] Avg time per run: %g seconds\n", name,  avg_total);
	printf("[%s] Total time for all runs: %g seconds\n", name,  total);
}

int main(void) {
	int num_of_runs = 1000;

	////////////////////
	// Uncached Queue //
	////////////////////

	ParallelQueue pq = {0};
	parallel_queue_init(&pq, sizeof(u64), QUEUE_CAPACITY);

	run_queue(&pq, produce_messages_pq, consume_messages_pq, "Uncached",  num_of_runs);

	//////////////////
	// Cached Queue //
	//////////////////

	CachedParallelQueue cq = {0};
	cached_parallel_queue_init(&cq, sizeof(u64), QUEUE_CAPACITY);

	run_queue(&cq, produce_messages_cq, consume_messages_cq, "Cached",  num_of_runs);

	Sleep(1000000);
}