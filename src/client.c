#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "protocol.h"

static int read_input(const char *prompt, char *input)
{
    while (1)
    {
        fputs(prompt, stdout);
        fflush(stdout);
        char *line = fgets(input, MESSAGE_SIZE, stdin);
        if (line == NULL)
            return 0;
        char *newline = strchr(input, '\n');
        if (newline != NULL)
        {
            *newline = '\0';
            return 1;
        }
        if (feof(stdin))
            return 1;
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
        {
        }
        printf("Dữ liệu quá dài. Vui lòng nhập lại.\n");
    }
}

int main()
{
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client == -1)
    {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8080);

    if (connect(client, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("connect");
        close(client);
        return EXIT_FAILURE;
    }

    char type, text[MESSAGE_SIZE], input[MESSAGE_SIZE];
    int result;
    while ((result = receive_message(client, &type, text)) == 1)
    {
        if (type == TEXT)
        {
            fputs(text, stdout);
            fflush(stdout);
        }
        else if (type == INPUT || type == PASSWORD)
        {
            if (!read_input(text, input))
                break;
            result = send_message(client, REPLY, input);
            memset(input, 0, sizeof(input));
            if (result != 1)
                break;
        }
        else
        {
            errno = EPROTO;
            result = -1;
            break;
        }
    }
    if (result < 0)
        perror("socket");
    close(client);
    return result < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
