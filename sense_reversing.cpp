#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>

#include "simple_barrier.hpp"

using namespace std;


class SenseReversingBarrier : public AtomicCountBarrier {
private:
public:
    SenseReversingBarrier(int counter) : AtomicCountBarrier(counter) { }
    ~SenseReversingBarrier() {}

    void await(bool);

};

void SenseReversingBarrier::await(bool mySense) {
    if(this->decrementCounter() == 1) {
        this->setCounter(this->getSize());
        this->setSense(mySense);
    } else {
        while (this->getSense() != mySense);
    }
}


SenseReversingBarrier *b;

void useSBB() {
    b->await(false);
}

int main(int argc, char *argv[]) {
    b = new SenseReversingBarrier(THREAD_NUM);

    vector<thread> threads;

    chrono::system_clock::time_point start = chrono::system_clock::now();
    for(int i=0; i<THREAD_NUM; i++)
        threads.push_back(thread(useSBB));
    for(int i=0; i<THREAD_NUM; i++)
        threads[i].join();
    chrono::system_clock::time_point end = chrono::system_clock::now();

    // print out result
    chrono::microseconds us = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "Total execution time: " << us.count() << " us" << endl;

    return 0;
}