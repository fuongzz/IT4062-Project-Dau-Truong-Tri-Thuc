#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static unsigned char replies[16384], output[32768];
static size_t reply_size, reply_offset, output_size;

static ssize_t fake_send(int fd, const void *buffer, size_t size, int flags)
{
    (void)fd;
    (void)flags;
    assert(output_size + size <= sizeof(output));
    memcpy(output + output_size, buffer, size);
    output_size += size;
    return (ssize_t)size;
}

static ssize_t fake_recv(int fd, void *buffer, size_t size, int flags)
{
    (void)fd;
    (void)flags;
    if (size > reply_size - reply_offset) size = reply_size - reply_offset;
    memcpy(buffer, replies + reply_offset, size);
    reply_offset += size;
    return (ssize_t)size;
}

#define main server_main
#define send fake_send
#define recv fake_recv
#include "../src/server.c"
#undef main
#undef send
#undef recv

static void queue_reply(const char *text)
{
    size_t length = strlen(text) + 1;
    assert(reply_size + length + 4 <= sizeof(replies));
    uint32_t header = htonl((uint32_t)length);
    memcpy(replies + reply_size, &header, 4);
    replies[reply_size + 4] = REPLY;
    memcpy(replies + reply_size + 5, text, length - 1);
    reply_size += length + 4;
}

static int has_message(char type, const char *text)
{
    for (size_t offset = 0; offset < output_size;)
    {
        uint32_t header;
        assert(offset + 4 <= output_size);
        memcpy(&header, output + offset, 4);
        size_t length = ntohl(header);
        assert(length >= 1 && length <= MESSAGE_SIZE);
        assert(offset + 4 + length <= output_size);
        char content[MESSAGE_SIZE];
        memcpy(content, output + offset + 5, length - 1);
        content[length - 1] = '\0';
        if (output[offset + 4] == type && strstr(content, text) != NULL) return 1;
        offset += length + 4;
    }
    return 0;
}

static void test_menu(void)
{
    const char *answers[] = {
        "9",                                    
        "1", "Demo123", "Abcd1234", "Abcd1234",     
        "2", "Demo123", "Wrong123",               
        "2", "Demo123", "Abcd1234"           
    };
    for (size_t i = 0; i < sizeof(answers) / sizeof(answers[0]); i++)
        queue_reply(answers[i]);

    serve_client(1);

    assert(reply_offset == reply_size);
    assert(has_message(INPUT, "1. Đăng ký\n2. Đăng nhập"));
    assert(has_message(TEXT, "Lựa chọn không hợp lệ"));
    assert(has_message(TEXT, "Đăng ký thành công"));
    assert(account_login("Demo123", "Abcd1234") == 1);
    assert(has_message(TEXT, "Tên đăng nhập hoặc mật khẩu không đúng"));
    assert(has_message(TEXT, "Chào mừng, Demo123!"));
    puts("ĐẠT: Menu, lựa chọn sai, đăng ký thành công, đăng nhập sai và đăng nhập đúng.");
}

int main(void)
{
    char directory[] = "/tmp/dttt-test-server-XXXXXX";
    int original_cwd = open(".", O_RDONLY);
    assert(original_cwd >= 0);
    assert(mkdtemp(directory) != NULL);
    assert(chdir(directory) == 0);
    assert(accounts_init() == 0);

    test_menu();

    assert(unlink(ACCOUNT_FILE) == 0);
    assert(rmdir("data") == 0);
    assert(fchdir(original_cwd) == 0);
    assert(close(original_cwd) == 0);
    assert(rmdir(directory) == 0);
    puts("TẤT CẢ KIỂM THỬ MÁY CHỦ ĐỀU ĐẠT.");
    return 0;
}
