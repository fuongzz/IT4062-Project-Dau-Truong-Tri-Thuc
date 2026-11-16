#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

static unsigned char sent[4096], incoming[4096];
static size_t sent_size, incoming_size, incoming_offset;
static size_t send_limit, recv_limit;

static ssize_t fake_send(int fd, const void *buffer, size_t size, int flags)
{
    (void)fd;
    (void)flags;
    if (size > send_limit) size = send_limit;
    assert(sent_size + size <= sizeof(sent));
    memcpy(sent + sent_size, buffer, size);
    sent_size += size;
    return (ssize_t)size;
}

static ssize_t fake_recv(int fd, void *buffer, size_t size, int flags)
{
    (void)fd;
    (void)flags;
    if (size > recv_limit) size = recv_limit;
    if (size > incoming_size - incoming_offset) size = incoming_size - incoming_offset;
    memcpy(buffer, incoming + incoming_offset, size);
    incoming_offset += size;
    return (ssize_t)size;
}

#define send fake_send
#define recv fake_recv
#include "../src/protocol.h"
#undef send
#undef recv

static void reset_socket()
{
    memset(sent, 0, sizeof(sent));
    memset(incoming, 0, sizeof(incoming));
    sent_size = incoming_size = incoming_offset = 0;
    send_limit = recv_limit = sizeof(sent);
}

static void test_send()
{
    reset_socket();
    send_limit = 2;
    assert(send_message(1, REPLY, "A") == 1);
    const unsigned char expected[] = {0, 0, 0, 2, 'R', 'A'};
    assert(sent_size == sizeof(expected));
    assert(memcmp(sent, expected, sizeof(expected)) == 0);

    puts("ĐẠT: Gửi đúng định dạng thông điệp dù mỗi lần chỉ gửi được 2 byte.");
}

static void test_receive()
{
    reset_socket();
    const unsigned char frames[] = {
        0, 0, 0, 6, 'T', 'H', 'e', 'l', 'l', 'o',
        0, 0, 0, 1, 'I'
    };
    memcpy(incoming, frames, sizeof(frames));
    incoming_size = sizeof(frames);
    recv_limit = 1;
    char type, text[MESSAGE_SIZE];
    assert(receive_message(1, &type, text) == 1);
    assert(type == TEXT && strcmp(text, "Hello") == 0);
    assert(receive_message(1, &type, text) == 1);
    assert(type == INPUT && strcmp(text, "") == 0);
    assert(receive_message(1, &type, text) == 0);

    puts("ĐẠT: Nhận từng byte, tách các thông điệp liên tiếp, nội dung rỗng và kết thúc luồng.");
}

static void test_invalid_frames()
{
    char type, text[MESSAGE_SIZE];
    const uint32_t lengths[] = {0, MESSAGE_SIZE + 1};
    for (size_t i = 0; i < sizeof(lengths) / sizeof(lengths[0]); i++)
    {
        reset_socket();
        uint32_t header = htonl(lengths[i]);
        memcpy(incoming, &header, 4);
        incoming_size = 4;
        errno = 0;
        assert(receive_message(1, &type, text) == -1);
        assert(errno == EPROTO);
    }

    puts("ĐẠT: Từ chối độ dài payload bằng 0 hoặc vượt giới hạn 1024 byte.");
}

int main()
{
    test_send();
    test_receive();
    test_invalid_frames();
    puts("TẤT CẢ KIỂM THỬ GIAO THỨC ĐỀU ĐẠT.");
    return 0;
}
