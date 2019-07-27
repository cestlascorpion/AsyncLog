#ifndef ASYNC_LOG_BUFFER_BLOCK_H
#define ASYNC_LOG_BUFFER_BLOCK_H

#include <cstdint>
#include <memory>

class BufferBlock {
public:
    // FREE: can write
    // FULL: can flush
    // FINAL: marked as FULL in order to end it
    enum STATUS { FREE, FULL, FINAL };

    enum {
        MAX_BUF_LEN = 1024, // block size
        MAX_BUF_LINE = 128, // max log data length
    };

public:
    BufferBlock();
    ~BufferBlock();

    BufferBlock(const BufferBlock &) = delete;
    BufferBlock &operator=(const BufferBlock &) = delete;

public:
    // free space of current block
    uint32_t Rest() const;
    // append into current block
    void Append(const char *source, uint32_t length, bool &full);
    // flush into file
    void Flush(FILE *file);
    // get current block status
    STATUS Status() const;
    // mark current status as FULL
    void Finally();

public:
    static uint32_t buf_id();

private:
    uint32_t m_id;
    uint32_t m_used;
    uint32_t m_total;
    std::unique_ptr<char[]> m_data;
    STATUS m_status;
};

#endif // ASYNC_LOG_BUFFER_BLOCK_H
