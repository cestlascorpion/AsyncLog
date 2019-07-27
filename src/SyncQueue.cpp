#include "SyncQueue.h"
#include "BufferBlock.h"
#include <unistd.h>

using namespace std;

SyncQueue::SyncQueue()
    : m_size((BUFFER_SIZE + BufferBlock::MAX_BUF_LEN - 1) / BufferBlock::MAX_BUF_LEN)
    , m_file(nullptr)
    , m_w_idx(0)
    , m_f_idx(0)
    , m_array(m_size) {
    for (auto &it : m_array) {
        it = make_unique<BufferBlock>();
    }
}

SyncQueue *SyncQueue::instance() {
    static once_flag flag;
    static SyncQueue instance;
    call_once(flag, [&]() {
        string name = "log." + to_string(getpid()) + ".log";
        instance.m_file = fopen(name.c_str(), "a+");
    });
    return &instance;
}

SyncQueue::~SyncQueue() {
    Finally();
    if (m_file != nullptr) {
        fclose(m_file);
    }
};

void SyncQueue::Produce(const char *source, uint32_t length) {
    lock_guard<mutex> lock(m_mutex);

    while (!canWrite()) {
        m_write.wait(m_mutex);
    }

    bool full = false;
    m_array[m_w_idx]->Append(source, length, full);
    if (full) {
        m_w_idx = (m_w_idx + 1) % m_size;
        m_flush.notify_one();
    }
}

void SyncQueue::Consume() {
    lock_guard<mutex> lock(m_mutex);

    while (!canFlush()) {
        m_flush.wait(m_mutex);
    }

    m_array[m_f_idx]->Flush(m_file);
    m_f_idx = (m_f_idx + 1) % m_size;
    m_write.notify_all();
}

bool SyncQueue::canFlush() {
    return m_array[m_f_idx]->Status() == BufferBlock::FULL || m_array[m_f_idx]->Status() == BufferBlock::FINAL;
}

bool SyncQueue::canWrite() {
    return m_array[m_f_idx]->Status() == BufferBlock::FREE;
}

void SyncQueue::Finally() {
    lock_guard<mutex> lock(m_mutex);

    if (m_array[m_w_idx]->Rest() > 0) {
        m_array[m_w_idx]->Finally();
        m_w_idx = (m_w_idx + 1) % m_size;
        m_flush.notify_one();
    }
}