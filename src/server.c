#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(listener);
        exit(EXIT_FAILURE);
    }

    if (listen(listener, 5) == -1) {
        perror("listen");
        close(listener);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port 8080...\n");

    int client_socket = accept(listener, NULL, NULL);
    if (client_socket == -1) {
        perror("accept");
        close(listener);
        exit(EXIT_FAILURE);
    }

    printf("Client connected.\n");

    close(client_socket);
    close(listener);
    return 0;
}
