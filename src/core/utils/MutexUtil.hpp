#pragma once

#include <mutex>

class priorityMutex
{
private:
    std::mutex dataLock;
    std::mutex high;
    std::mutex low;
public:
    void lockLow() {
        low.lock();
        high.lock();
        dataLock.lock();
        high.unlock();
    }

    void unlockLow() {
        dataLock.unlock();
        low.unlock();
    }

    void lockHigh() {
        high.lock();
        dataLock.lock();
        high.unlock();
    }

    void unlockHigh() {
        dataLock.unlock();
    }

};