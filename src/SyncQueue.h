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
    uint32_t m_size;
    FILE *m_file;

    uint32_t m_w_idx;
    uint32_t m_f_idx;
    std::vector<std::unique_ptr<BufferBlock>> m_array;

    std::mutex m_mutex;
    std::condition_variable_any m_write;
    std::condition_variable_any m_flush;
};
#endif // ASYNC_LOG_SYNC_QUEUE_H
