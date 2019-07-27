#include "BufferBlock.h"
#include <cstdio>
#include <cstring>
#include <unistd.h>

BufferBlock::BufferBlock()
    : m_id(buf_id())
    , m_used(0)
    , m_total(MAX_BUF_LEN)
    , m_data(new char[MAX_BUF_LEN]())
    , m_status(FREE) {}

BufferBlock::~BufferBlock() = default;

uint32_t BufferBlock::Rest() const {
    return m_total - m_used;
}

void BufferBlock::Append(const char *source, uint32_t length, bool &full) {
    memcpy(m_data.get() + m_used, source, length);
    m_used += length;
    if (Rest() < MAX_BUF_LINE) {
        m_status = FULL;
        full = true;
    } else {
        full = false;
    }
}

void BufferBlock::Flush(FILE *file) {
    fwrite(m_data.get(), 1, m_used, file);
    m_used = 0;
    m_status = FREE;
}

BufferBlock::STATUS BufferBlock::Status() const {
    return m_status;
}

void BufferBlock::Finally() {
    m_status = FINAL;
}

uint32_t BufferBlock::buf_id() {
    static auto id = (uint32_t)getpid();
    return ++id;
}
