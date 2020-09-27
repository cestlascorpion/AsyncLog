#include "SyncQueue.h"

#include <chrono>
#include <thread>
#include <vector>

#define COUNT 10000

using namespace std;
using namespace scorpion;

void produce(SyncQueue *log, const string &prefix, const string &suffix);
void consume(SyncQueue *log);

int main() {
    auto *log = SyncQueue::instance();

    vector<thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back(produce, log, "[*******" + to_string(i), to_string(i + 1) + "*******]");
    }
    threads.emplace_back(consume, log);
    threads.back().detach();
    for (auto &t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    log->Finally();
    this_thread::sleep_for(chrono::seconds(1));
    return 0;
}

void produce(SyncQueue *log, const string &prefix, const string &suffix) {
    if (log == nullptr) {
        return;
    }

    for (int i = 0; i < COUNT; ++i) {
        string str = prefix + to_string(rand());
        str += suffix + "\n";
        log->Produce(str.c_str(), (uint32_t)str.size());
        this_thread::sleep_for(chrono::microseconds(10));
    }
}

void consume(SyncQueue *log) {
    if (log == nullptr) {
        return;
    }

    for (int i = 0; i < COUNT; ++i) {
        log->Consume();
    }
}