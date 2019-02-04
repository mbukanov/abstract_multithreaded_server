#include "myThread.hpp"

void *attached_thread(void* arg) {
    myThread * thread = (myThread*) arg;
    printf("Start thread_function\n");
    thread->thread_function();
    return NULL;
}

void myThread::Start() {
    if(started) return;
    printf("Create thread\n");
    if(pthread_create(&thread_fd, NULL, attached_thread, this) == 0) 
        started = true;
}

void myThread::Wait() {
    if(!started) return;
    pthread_join(thread_fd, NULL);
    started = false;
}

void myThread::Stop() {
    if(!started) return;
    pthread_cancel(thread_fd);
    Wait();
    started = false;
}

