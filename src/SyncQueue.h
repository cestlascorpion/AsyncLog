#ifndef ASYNC_LOG_SYNC_QUEUE_H
#define ASYNC_LOG_SYNC_QUEUE_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <vector>

class BufferBlock;

class SyncQueue {
public:
    enum { BUFFER_SIZE = 1024 * 1024 };

public:
    static SyncQueue *instance();
    ~SyncQueue();

    SyncQueue(const SyncQueue &) = delete;
    SyncQueue &operator=(const SyncQueue) = delete;
    SyncQueue(const SyncQueue &&) = delete;
    SyncQueue &operator=(SyncQueue &&) = delete;

public:
    void Produce(const char *source, uint32_t length);
    void Consume();
    void Finally();

private:
    SyncQueue();

    bool canFlush();
    bool canWrite();

private:
    uint32_t _size;
    FILE *_file;

    uint32_t _w_idx;
    uint32_t _f_idx;
    std::vector<std::unique_ptr<BufferBlock>> _array;

    std::mutex _mutex;
    std::condition_variable_any _write;
    std::condition_variable_any _flush;
};
#endif // ASYNC_LOG_SYNC_QUEUE_H
