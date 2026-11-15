#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "accounts.h"
#include "protocol.h"

static int ask(int client, char type, const char *prompt, char *answer)
{
    char reply_type;
    return send_message(client, type, prompt) == 1 && receive_message(client, &reply_type, answer) == 1 && reply_type == REPLY;
}

static int register_user(int client)
{
    char username[MESSAGE_SIZE], password[MESSAGE_SIZE] = {0};
    char confirm[MESSAGE_SIZE] = {0};
    int result = 0;

    if (send_message(client, TEXT,
                     "\n------------- ĐĂNG KÝ -------------\n"
                     "Nhập /back để quay về menu chính.\n"
                     "Tên đăng nhập: 3–20 ký tự A–Z, a–z, 0–9, _. Phân biệt hoa thường.\n") != 1)
        goto done;

    while (1)
    {
        if (!ask(client, INPUT, "Tên đăng nhập: ", username))
            goto done;
        if (strcmp(username, "/back") == 0)
        {
            result = 1;
            goto done;
        }
        if (!valid_username(username))
        {
            if (send_message(client, TEXT, "Tên đăng nhập không hợp lệ.\n") != 1)
                goto done;
            continue;
        }
        int exists = account_exists(username);
        if (exists < 0)
        {
            result = send_message(client, TEXT, "Không đọc được dữ liệu tài khoản.\n") == 1;
            goto done;
        }
        if (exists == 0)
            break;
        if (send_message(client, TEXT, "Tên đăng nhập đã tồn tại.\n") != 1)
            goto done;
    }

    if (send_message(client, TEXT,
                     "Mật khẩu: 8–64 ký tự, có chữ hoa A–Z, chữ thường a–z và số.\n"
                     "Chỉ được dùng A–Z, a–z, 0–9; không có khoảng trắng hay ký tự đặc biệt.\n") != 1)
        goto done;

    while (1)
    {
        if (!ask(client, PASSWORD, "Mật khẩu: ", password))
            goto done;
        if (strcmp(password, "/back") == 0)
        {
            result = 1;
            goto done;
        }
        if (!valid_password(password))
        {
            if (send_message(client, TEXT, "Mật khẩu không đáp ứng quy tắc.\n") != 1)
                goto done;
            continue;
        }
        if (!ask(client, PASSWORD, "Xác nhận mật khẩu: ", confirm))
            goto done;
        if (strcmp(confirm, "/back") == 0)
        {
            result = 1;
            goto done;
        }
        if (strcmp(password, confirm) == 0)
            break;
        if (send_message(client, TEXT, "Mật khẩu xác nhận không khớp. Hãy nhập lại.\n") != 1)
            goto done;
    }

    int saved = account_register(username, password);
    const char *message;
    if (saved == 1)
    {
        message = "Đăng ký thành công! Vui lòng đăng nhập.\n";
    }
    else if (saved == 0)
    {
        message = "Tên đăng nhập đã tồn tại.\n";
    }
    else
    {
        message = "Không lưu được tài khoản. Đăng ký thất bại.\n";
    }
    result = send_message(client, TEXT, message) == 1;
done:
    sodium_memzero(password, sizeof(password));
    sodium_memzero(confirm, sizeof(confirm));
    return result;
}

static int login_user(int client)
{
    char username[MESSAGE_SIZE], password[MESSAGE_SIZE] = {0};
    if (send_message(client, TEXT,
                     "\n------------ ĐĂNG NHẬP ------------\nNhập /back để quay về menu chính.\n") != 1)
        return 0;
    if (!ask(client, INPUT, "Tên đăng nhập: ", username))
        return 0;
    if (strcmp(username, "/back") == 0)
        return 1;
    int received = ask(client, PASSWORD, "Mật khẩu: ", password);
    if (!received || strcmp(password, "/back") == 0)
    {
        sodium_memzero(password, sizeof(password));
        return received;
    }
    int valid = account_login(username, password);
    sodium_memzero(password, sizeof(password));
    if (valid != 1)
    {
        return send_message(client, TEXT, valid < 0 ? "Không đọc được dữ liệu tài khoản.\n" : "Tên đăng nhập hoặc mật khẩu không đúng.\n") == 1;
    }
    char message[MESSAGE_SIZE];
    snprintf(message, sizeof(message), "Đăng nhập thành công!\nChào mừng, %.20s!\n", username);
    return send_message(client, TEXT, message) == 1 ? 2 : 0;
}

static void serve_client(int client)
{
    char choice[MESSAGE_SIZE];
    const char *menu = "\n====================================\n"
                       "       ĐẤU TRƯỜNG TRI THỨC\n"
                       "====================================\n"
                       "Chào mừng bạn đến với trò chơi!\n\n"
                       "1. Đăng ký\n2. Đăng nhập\n"
                       "------------------------------------\nLựa chọn: ";
    while (ask(client, INPUT, menu, choice))
    {
        if (strcmp(choice, "1") == 0)
        {
            if (!register_user(client))
                break;
        }
        else if (strcmp(choice, "2") == 0)
        {
            if (login_user(client) != 1)
                break;
        }
        else if (send_message(client, TEXT, "Lựa chọn không hợp lệ. Vui lòng nhập 1 hoặc 2.\n") != 1)
            break;
    }
}

int main()
{
    if (accounts_init() != 0)
    {
        fprintf(stderr, "Không khởi tạo được libsodium hoặc data/accounts.tsv.\n");
        return EXIT_FAILURE;
    }

    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) == -1 || listen(listener, 5) == -1)
    {
        perror("bind/listen");
        close(listener);
        return EXIT_FAILURE;
    }

    printf("Server is listening on port 8080...\n");

    while (1)
    {
        int client = accept(listener, NULL, NULL);
        if (client == -1)
        {
            if (errno == EINTR)
                continue;
            perror("accept");
            break;
        }
        printf("Client connected.\n");
        serve_client(client);
        close(client);
        printf("Client disconnected.\n");
    }
    close(listener);
    return EXIT_FAILURE;
}
