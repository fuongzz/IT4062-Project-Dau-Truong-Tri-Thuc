#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static unsigned char incoming[4096], sent[4096];
static size_t incoming_size, incoming_offset, sent_size;
static int closed_count;

static int fake_socket(int domain, int type, int protocol)
{
    assert(domain == AF_INET && type == SOCK_STREAM && protocol == IPPROTO_TCP);
    return 42;
}

static int fake_connect(int fd, const struct sockaddr *address, socklen_t size)
{
    assert(fd == 42 && size == sizeof(struct sockaddr_in));
    const struct sockaddr_in *addr = (const struct sockaddr_in *)address;
    assert(addr->sin_family == AF_INET);
    assert(ntohs(addr->sin_port) == 8080);
    assert(addr->sin_addr.s_addr == htonl(INADDR_LOOPBACK));
    return 0;
}

static int fake_close(int fd)
{
    assert(fd == 42);
    closed_count++;
    return 0;
}

static ssize_t fake_send(int fd, const void *buffer, size_t size, int flags)
{
    assert(fd == 42);
    (void)flags;
    assert(sent_size + size <= sizeof(sent));
    memcpy(sent + sent_size, buffer, size);
    sent_size += size;
    return (ssize_t)size;
}

static ssize_t fake_recv(int fd, void *buffer, size_t size, int flags)
{
    assert(fd == 42);
    (void)flags;
    if (size > incoming_size - incoming_offset) size = incoming_size - incoming_offset;
    memcpy(buffer, incoming + incoming_offset, size);
    incoming_offset += size;
    return (ssize_t)size;
}

#define main client_main
#define socket fake_socket
#define connect fake_connect
#define close fake_close
#define send fake_send
#define recv fake_recv
#include "../src/client.c"
#undef main
#undef socket
#undef connect
#undef close
#undef send
#undef recv

static FILE *keyboard, *screen;
static int saved_stdin, saved_stdout;

static void begin_console(const char *input)
{
    keyboard = tmpfile();
    screen = tmpfile();
    assert(keyboard != NULL && screen != NULL);
    assert(fputs(input, keyboard) >= 0);
    rewind(keyboard);
    assert(fflush(stdout) == 0);
    saved_stdin = dup(STDIN_FILENO);
    saved_stdout = dup(STDOUT_FILENO);
    assert(saved_stdin >= 0 && saved_stdout >= 0);
    assert(dup2(fileno(keyboard), STDIN_FILENO) >= 0);
    assert(dup2(fileno(screen), STDOUT_FILENO) >= 0);
    clearerr(stdin);
    incoming_size = incoming_offset = sent_size = 0;
    closed_count = 0;
}

static void end_console(char *text, size_t size)
{
    assert(fflush(stdout) == 0);
    rewind(screen);
    size_t n = fread(text, 1, size - 1, screen);
    text[n] = '\0';
    assert(dup2(saved_stdin, STDIN_FILENO) >= 0);
    assert(dup2(saved_stdout, STDOUT_FILENO) >= 0);
    assert(close(saved_stdin) == 0);
    assert(close(saved_stdout) == 0);
    assert(fclose(keyboard) == 0);
    assert(fclose(screen) == 0);
    clearerr(stdin);
}

static void queue_message(char type, const char *text)
{
    size_t length = strlen(text) + 1;
    assert(incoming_size + length + 4 <= sizeof(incoming));
    uint32_t header = htonl((uint32_t)length);
    memcpy(incoming + incoming_size, &header, 4);
    incoming[incoming_size + 4] = (unsigned char)type;
    memcpy(incoming + incoming_size + 5, text, length - 1);
    incoming_size += length + 4;
}

static void test_messages_and_replies(void)
{
    const char *inputs[] = {"1\nAbcd1234\n", "2\nAbcd1234\n"};
    for (size_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++)
    {
        char text[4096];
        begin_console(inputs[i]);
        queue_message(TEXT, "Chào bạn!\n");
        queue_message(INPUT, "Lựa chọn: ");
        queue_message(PASSWORD, "Mật khẩu: ");
        queue_message(TEXT, "Xong!\n");
        int result = client_main();
        end_console(text, sizeof(text));
        assert(result == EXIT_SUCCESS && closed_count == 1);
        assert(incoming_offset == incoming_size);
        assert(strstr(text, "Chào bạn!") != NULL);
        assert(strstr(text, "Lựa chọn: ") != NULL);
        assert(strstr(text, "Mật khẩu: ") != NULL);
        assert(strstr(text, "Xong!") != NULL);
        const unsigned char expected[] = {
            0, 0, 0, 2, 'R', inputs[i][0],
            0, 0, 0, 9, 'R', 'A', 'b', 'c', 'd', '1', '2', '3', '4'
        };
        assert(sent_size == sizeof(expected));
        assert(memcmp(sent, expected, sizeof(expected)) == 0);
        printf("ĐẠT: Chọn %c, hiển thị thông báo, gửi đúng dữ liệu và đóng kết nối.\n",
               inputs[i][0]);
    }
}

int main(void)
{
    test_messages_and_replies();
    puts("TẤT CẢ KIỂM THỬ MÁY KHÁCH ĐỀU ĐẠT.");
    return 0;
}
