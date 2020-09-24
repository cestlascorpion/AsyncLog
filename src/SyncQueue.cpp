#include "SyncQueue.h"
#include "BufferBlock.h"
#include <unistd.h>

using namespace std;

SyncQueue::SyncQueue()
    : _size((BUFFER_SIZE + BufferBlock::MAX_BUF_LEN - 1) / BufferBlock::MAX_BUF_LEN)
    , _file(nullptr)
    , _w_idx(0)
    , _f_idx(0)
    , _array(_size) {
    for (auto &it : _array) {
        it = make_unique<BufferBlock>();
    }
}

SyncQueue *SyncQueue::instance() {
    static once_flag flag;
    static SyncQueue instance;
    call_once(flag, [&]() {
        string name = "log." + to_string(getpid()) + ".log";
        instance._file = fopen(name.c_str(), "a+");
    });
    return &instance;
}

SyncQueue::~SyncQueue() {
    Finally();
    if (_file != nullptr) {
        fclose(_file);
    }
};

void SyncQueue::Produce(const char *source, uint32_t length) {
    lock_guard<mutex> lock(_mutex);

    while (!canWrite()) {
        _write.wait(_mutex);
    }

    bool full = false;
    _array[_w_idx]->Append(source, length, full);
    if (full) {
        _w_idx = (_w_idx + 1) % _size;
        _flush.notify_one();
    }
}

void SyncQueue::Consume() {
    lock_guard<mutex> lock(_mutex);

    while (!canFlush()) {
        _flush.wait(_mutex);
    }

    _array[_f_idx]->Flush(_file);
    _f_idx = (_f_idx + 1) % _size;
    _write.notify_all();
}

bool SyncQueue::canFlush() {
    return _array[_f_idx]->Status() == BufferBlock::FULL || _array[_f_idx]->Status() == BufferBlock::FINAL;
}

bool SyncQueue::canWrite() {
    return _array[_f_idx]->Status() == BufferBlock::FREE;
}

void SyncQueue::Finally() {
    lock_guard<mutex> lock(_mutex);

    if (_array[_w_idx]->Rest() > 0) {
        _array[_w_idx]->Finally();
        _w_idx = (_w_idx + 1) % _size;
        _flush.notify_one();
    }
}