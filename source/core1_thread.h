#ifndef CORE1_THREAD_H
#define CORE1_THREAD_H
#include <pico/util/queue.h>

extern queue_t results_queue;
extern queue_t data_queue;

void core1_run(void);
void core1_entry(void);

#endif /* CORE1_THREAD_H */