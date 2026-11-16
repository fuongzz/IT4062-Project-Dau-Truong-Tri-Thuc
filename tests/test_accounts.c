#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "../src/accounts.h"

static void clear_accounts()
{
    FILE *file = fopen(ACCOUNT_FILE, "w");
    assert(file != NULL);
    assert(fclose(file) == 0);
}

static void test_username()
{
    assert(valid_username("An_123") == 1);
    assert(valid_username("abc") == 1);
    assert(valid_username("abcdefghijklmnopqrst") == 1);
    const char *invalid[] = {"", "ab", "abcdefghijklmnopqrstu", "An 123", "An-123", "An.123", "An\t123", "Nguyễn"};
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); i++)
        assert(valid_username(invalid[i]) == 0);
    puts("ĐẠT: Quy tắc tên đăng nhập và giới hạn độ dài.");
}

static void test_password()
{
    assert(valid_password("Abcd1234") == 1);
    const char *invalid[] = {"", "Abc1234", "abcd1234", "ABCD1234", "Abcdefgh", " Abcd1234", "Abcd1234 ", "Abcd 1234", "Abcd_1234", "Abcd1234!", "Abcd1234é"};
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); i++)
        assert(valid_password(invalid[i]) == 0);

    char boundary[66];
    memset(boundary, 'a', sizeof(boundary));
    memcpy(boundary, "Aa1", 3);
    boundary[64] = '\0';
    assert(valid_password(boundary) == 1);
    boundary[64] = 'a';
    boundary[65] = '\0';
    assert(valid_password(boundary) == 0);
    puts("ĐẠT: Ký tự mật khẩu hợp lệ, thành phần bắt buộc và giới hạn độ dài.");
}

static void test_register_and_login()
{
    clear_accounts();
    assert(account_register("An123", "Abcd1234") == 1);
    assert(account_register("An123", "Different123") == 0);
    assert(account_register("an123", "Abcd1234") == 1);
    assert(account_register("bad name", "Abcd1234") == -1);
    assert(account_register("Invalid123", "abcd1234") == -1);
    assert(account_exists("An123") == 1);
    assert(account_exists("AN123") == 0);
    assert(account_exists("Invalid123") == 0);
    assert(account_login("An123", "Abcd1234") == 1);
    assert(account_login("An123", "Wrong123") == 0);
    assert(account_login("AN123", "Abcd1234") == 0);
    assert(account_login("Unknown123", "Abcd1234") == 0);

    Account first, second;
    unsigned int max_id;
    assert(find_account("An123", &first, &max_id) == 1);
    assert(first.id == 1 && max_id == 2);
    assert(find_account("an123", &second, &max_id) == 1);
    assert(second.id == 2);
    assert(strncmp(first.password_hash, "$argon2id$", 10) == 0);
    assert(strstr(first.password_hash, "Abcd1234") == NULL);
    assert(strcmp(first.password_hash, second.password_hash) != 0);

    assert(accounts_init() == 0);
    assert(account_login("An123", "Abcd1234") == 1);
    assert(account_login("an123", "Abcd1234") == 1);
    puts("ĐẠT: Đăng ký, tên trùng, phân biệt hoa thường, ID, băm mật khẩu, đăng nhập và lưu tài khoản.");
}


int main()
{
    char directory[] = "/tmp/dttt-test-accounts-XXXXXX";
    int original_cwd = open(".", O_RDONLY);
    assert(original_cwd >= 0);
    assert(mkdtemp(directory) != NULL);
    assert(chdir(directory) == 0);
    assert(accounts_init() == 0);

    test_username();
    test_password();
    test_register_and_login();

    assert(unlink(ACCOUNT_FILE) == 0);
    assert(rmdir("data") == 0);
    assert(fchdir(original_cwd) == 0);
    assert(close(original_cwd) == 0);
    assert(rmdir(directory) == 0);
    puts("TẤT CẢ KIỂM THỬ TÀI KHOẢN ĐỀU ĐẠT.");
    return 0;
}
