#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
#include <queue>
#include "simple_barrier.hpp"

using namespace std;


class CombiningBarrier : public AtomicCountBarrier {
private:
    CombiningBarrier* parent;

public:
    CombiningBarrier() : parent(NULL) {}
    CombiningBarrier(CombiningBarrier* _parent) : parent(_parent), AtomicCountBarrier(2) {}

    void await(bool);

    CombiningBarrier* getParent() { return this->parent; }

};

void CombiningBarrier::await(bool mySense) {
    if(this->decrementCounter() == 1){
        if(this->parent != NULL) {
            //cout << "await() on parent: " << this->parent << endl;
            this->parent->await(mySense);
        }
        this->setCounter(this->getSize());
        this->setSense(mySense);
    } else {
        //cout << "Waiting for loser..." << endl;
        while(this->getSense() != mySense);
    }
}

CombiningBarrier* root;
vector<CombiningBarrier*> nodes;
std::atomic<int> cur_node_num(0);

void useCBB() {
    int select_node = cur_node_num++;
    select_node/=2;
    nodes[select_node]->await(false);
}

void buildTree(CombiningBarrier* rt, int depth) {
    queue<CombiningBarrier*> temp_nodes;
    temp_nodes.push(rt);

    // Build tree structure
    CombiningBarrier *cur;
    for(int i=0; i<(depth-2); i++) {
        for(int j=0; j<(1<<i); j++) {
            cur = temp_nodes.front();
            temp_nodes.push(new CombiningBarrier(cur));
            temp_nodes.push(new CombiningBarrier(cur));
            temp_nodes.pop();
        }
    }
    
    // put leaf nodes in vector
    for(int i=0; i<(1<<(depth-2)); i++) {
        cur = temp_nodes.front();
        nodes.push_back(new CombiningBarrier(cur));
        nodes.push_back(new CombiningBarrier(cur));
        temp_nodes.pop();
    }
    //cout << "build tree done, temp_nodes emptpy? " << temp_nodes.empty() << endl << endl;
}

void printTree() {
    for(int i=0; i<nodes.size(); i++) {
        CombiningBarrier *cur = nodes[i];
        cout << i << " ]" << endl;
        while(cur != NULL) {
            cout << "(" << cur << ", " << cur->getCounter() << ", " << cur->getParent() << ")" << endl;
            cur = cur->getParent();
        }
        cout << endl;
    }
}

int main(int argc, char *argv[]) {
    root = new CombiningBarrier();
    root->setSize(2);
    root->setCounter(2);
    //cout << "Root initialized" << endl;
    buildTree(root, TREE_DEPTH);

    //printTree();

    vector<thread> threads;

    chrono::system_clock::time_point start = chrono::system_clock::now();
    for(int i=0; i<THREAD_NUM; i++)
        threads.push_back(thread(useCBB));
    for(int i=0; i<THREAD_NUM; i++)
        threads[i].join();
    chrono::system_clock::time_point end = chrono::system_clock::now();

    // print out result
    chrono::microseconds us = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "Total execution time: " << us.count() << " us" << endl;

    return 0;
}
