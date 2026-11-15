#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <arpa/inet.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>

#define MESSAGE_SIZE 1024
#define TEXT 'T'
#define INPUT 'I'
#define PASSWORD 'P'
#define REPLY 'R'

static int transfer_all(int socket_fd, void *buffer, size_t size, int writing)
{
    char *data = buffer;
    while (size > 0) {
        ssize_t n;
        if (writing) {
            n = send(socket_fd, data, size, MSG_NOSIGNAL);
        } 
        else {
            n = recv(socket_fd, data, size, 0);
        }

        if (n < 0) {
            if (errno == EINTR) continue; 
            return -1;
        }
        if (n == 0) return 0; 

        data += n;
        size -= (size_t)n;
    }
    return 1;
}

static int send_message(int socket_fd, char type, const char *text)
{
    char frame[4 + MESSAGE_SIZE];
    size_t length = strlen(text) + 1;
    if (length > MESSAGE_SIZE) return -1;
    uint32_t header = htonl((uint32_t)length);
    memcpy(frame, &header, 4);
    frame[4] = type;
    memcpy(frame + 5, text, length - 1);
    return transfer_all(socket_fd, frame, length + 4, 1);
}

static int receive_message(int socket_fd, char *type, char *text)
{
    uint32_t header;
    int result = transfer_all(socket_fd, &header, sizeof(header), 0);
    if (result != 1) return result;
    size_t length = ntohl(header);
    if (length < 1 || length > MESSAGE_SIZE) {
        errno = EPROTO;
        return -1;
    }
    result = transfer_all(socket_fd, text, length, 0);
    if (result != 1) return result;
    *type = text[0];
    if (memchr(text + 1, '\0', length - 1) != NULL) {
        errno = EPROTO;
        return -1;
    }
    memmove(text, text + 1, length - 1);
    text[length - 1] = '\0';
    return 1;
}

#endif
