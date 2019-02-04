#ifndef MYTHREAD
#define MYTHREAD
#include <pthread.h>
#include <stdio.h>

class myThread {
private:
    pthread_t thread_fd;
    bool started;

public:
    myThread(): started(0) {};
    ~myThread() {
        printf("~myThread\n");
        Stop();
    }

    void Start();
    void Wait();
    void Stop();
    virtual void* thread_function() = 0;
};

#endif
