#include "BufferBlock.h"
#include <cstdio>
#include <cstring>
#include <unistd.h>

BufferBlock::BufferBlock()
    : _id(buf_id())
    , _used(0)
    , _total(MAX_BUF_LEN)
    , _data(new char[MAX_BUF_LEN]())
    , _status(FREE) {}

BufferBlock::~BufferBlock() = default;

uint32_t BufferBlock::Rest() const {
    return _total - _used;
}

void BufferBlock::Append(const char *source, uint32_t length, bool &full) {
    memcpy(_data.get() + _used, source, length);
    _used += length;
    if (Rest() < MAX_BUF_LINE) {
        _status = FULL;
        full = true;
    } else {
        full = false;
    }
}

void BufferBlock::Flush(FILE *file) {
    fwrite(_data.get(), 1, _used, file);
    _used = 0;
    _status = FREE;
}

BufferBlock::STATUS BufferBlock::Status() const {
    return _status;
}

void BufferBlock::Finally() {
    _status = FINAL;
}

uint32_t BufferBlock::buf_id() {
    static auto id = (uint32_t)getpid();
    return ++id;
}
