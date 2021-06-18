#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <queue>
#include <cmath>
#include "simple_barrier.hpp"

using namespace std;

class Node { 
private:
    bool flag[2];
    Node* partner;

public:
    friend class DisseminationBarrier;
    Node() : flag{true, true}, partner(NULL) {}
};

class DisseminationBarrier : public Barrier {
private:
    vector<vector<Node*>> nodes;
public:
    DisseminationBarrier(int n);
    void await(bool) {}
    void await(int, bool);
};

DisseminationBarrier::DisseminationBarrier(int n) {
    this->setSize(n);

    for(int i=0; i<n; i++) {
        nodes.push_back(vector<Node*>());
        for(int j=0; j<(int)log2(n); j++)
            nodes[i].push_back(new Node());
    }

    int d = 1;
    for(int i=0; i<n; i++) {
        d = 1;
        for(int j=0;j<(int)log2(n); j++, d*=2)
            nodes[i][j]->partner = nodes[(i+d)%n][j];
    }
}


void DisseminationBarrier::await(int thread_no, bool mySense){
    for(int r=0; r<(int)log2(this->getSize()); r++) {
        //cout << "[" << thread_no << "] " << r << ": " << nodes[thread_no][r]->partner << endl;
        nodes[thread_no][r]->partner->flag[1] = mySense;
        while(nodes[thread_no][r]->flag[1] != mySense);
    }
}


DisseminationBarrier *b;

void useDB(int i){
    b->await(i, false);
}

int main(int argc, char *argv[]) {
    b = new DisseminationBarrier(THREAD_NUM);

    vector<thread> threads;

    chrono::system_clock::time_point start = chrono::system_clock::now();
    for(int i=0; i<THREAD_NUM; i++)
        threads.push_back(thread(useDB, i));
    for(int i=0; i<THREAD_NUM; i++)
        threads[i].join();
    chrono::system_clock::time_point end = chrono::system_clock::now();

    // print out result
    chrono::microseconds us = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "Total execution time: " << us.count() << " us" << endl;

    return 0;
}
