#ifndef CALLSTACK_H
#define CALLSTACK_H

#include <pthread.h>

int GetCallstack(pthread_t threadId, void **buffer, int size);

void print_backtrace(pthread_t tid);

#endif
