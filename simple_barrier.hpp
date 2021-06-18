#pragma once

#include <atomic>

#define TREE_DEPTH 6
#define THREAD_NUM (1<<TREE_DEPTH)

class Barrier {
private:
    bool sense = true;
    int size;

public:
    Barrier() : size(0), sense(true) {}
    Barrier(int _size) : size(_size), sense(true) {}
    ~Barrier() {}

    bool getSense() { return this->sense; }
    int getSize() { return this->size; }

    void flipSense() { this->sense = !this->sense; }

    void setSense(bool _sense) { sense = _sense; }
    void setSize(int _size) { size = _size; }

    virtual void await(bool){}
};

class AtomicCountBarrier : public Barrier {
private:
    std::atomic<int> counter;

public:
    AtomicCountBarrier() : Barrier(0), counter(0) {}
    AtomicCountBarrier(int _counter) : Barrier(_counter), counter(_counter) {}
    
    void setCounter(int c) { counter = c; }
    int decrementCounter() { return counter--; }
    int getCounter() { return counter.load(); }
};
