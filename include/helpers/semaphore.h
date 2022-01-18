// https://stackoverflow.com/questions/4792449/c0x-has-no-semaphores-how-to-synchronize-threads
#ifndef RELIABLE_TRANSPORT_LAYER_SEMAPHORE_H
#define RELIABLE_TRANSPORT_LAYER_SEMAPHORE_H

#include <mutex>
#include <condition_variable>

#include <mutex>
#include <condition_variable>

class Semaphore {
public:
    Semaphore (int count_ = 0)
            : count(count_) {}

    inline void up()
    {
        std::unique_lock<std::mutex> lock(mtx);
        count++;
        cv.notify_one();
    }

    inline void down()
    {
        std::unique_lock<std::mutex> lock(mtx);

        while(count == 0){
            cv.wait(lock);
        }
        count--;
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
};

#endif //RELIABLE_TRANSPORT_LAYER_SEMAPHORE_H

#pragma clang diagnostic pop