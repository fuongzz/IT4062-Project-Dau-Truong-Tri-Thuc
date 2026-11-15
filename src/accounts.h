#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include <sodium.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define USERNAME_MAX 20
#define PASSWORD_MIN 8
#define PASSWORD_MAX 64

typedef struct {
    unsigned int id;
    char username[USERNAME_MAX + 1];
    char password_hash[crypto_pwhash_STRBYTES];
} Account;

#define ACCOUNT_FILE "data/accounts.tsv"

static int accounts_init(void)
{
    if (sodium_init() < 0) return -1;
    if (mkdir("data", 0700) == -1 && errno != EEXIST) return -1;
    FILE *file = fopen(ACCOUNT_FILE, "a");
    if (file == NULL) return -1;
    return fclose(file);
}

static int valid_username(const char *username)
{
    size_t length = strlen(username);
    if (length < 3 || length > USERNAME_MAX) return 0;
    for (size_t i = 0; i < length; i++) {
        char c = username[i];
        if (!(c >= 'A' && c <= 'Z') && !(c >= 'a' && c <= 'z') && !(c >= '0' && c <= '9') && c != '_') return 0;
    }
    return 1;
}

static int valid_password(const char *password)
{
    size_t length = strlen(password);
    if (length < PASSWORD_MIN || length > PASSWORD_MAX) return 0;
    int upper = 0, lower = 0, digit = 0;
    for (size_t i = 0; i < length; i++) {
        char c = password[i];
        if (c >= 'A' && c <= 'Z') upper = 1;
        else if (c >= 'a' && c <= 'z') lower = 1;
        else if (c >= '0' && c <= '9') digit = 1;
        else return 0;
    }
    return upper && lower && digit;
}

static int find_account(const char *username, Account *found, unsigned int *max_id)
{
    FILE *file = fopen(ACCOUNT_FILE, "r");
    if (file == NULL) return -1;
    char line[256], extra;
    Account account;
    int result = 0;
    *max_id = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%u\t%20[A-Za-z0-9_]\t%127s %c", &account.id,
            account.username, account.password_hash, &extra) != 3 ||
            account.id == 0 || account.id > INT_MAX ||
            !valid_username(account.username) ||
            strncmp(account.password_hash, "$argon2id$", 10) != 0) {
            result = -1;
            break;
        }
        if (account.id > *max_id) *max_id = account.id;
        if (strcmp(account.username, username) == 0) {
            *found = account;
            result = 1;
        }
    }
    if (ferror(file)) result = -1;
    fclose(file);
    return result;
}

static int account_exists(const char *username)
{
    Account account;
    unsigned int max_id;
    return find_account(username, &account, &max_id);
}

static int account_register(const char *username, const char *password)
{
    if (!valid_username(username) || !valid_password(password)) return -1;
    Account account;
    unsigned int max_id;
    int exists = find_account(username, &account, &max_id);
    if (exists == 1) return 0;
    if (exists == -1 || max_id == INT_MAX) return -1;

    if (crypto_pwhash_str_alg(account.password_hash, password, strlen(password),
                             crypto_pwhash_OPSLIMIT_INTERACTIVE,
                             crypto_pwhash_MEMLIMIT_INTERACTIVE,
                             crypto_pwhash_ALG_ARGON2ID13) != 0) return -1;

    FILE *file = fopen(ACCOUNT_FILE, "a");
    if (file == NULL) return -1;
    int written = fprintf(file, "%u\t%s\t%s\n", max_id + 1, username,
                          account.password_hash);
    int closed = fclose(file);
    if (written < 0 || closed != 0) return -1;
    return 1;
}

static int account_login(const char *username, const char *password)
{
    Account account;
    unsigned int max_id;
    int found = find_account(username, &account, &max_id);
    if (found != 1) return found;
    return crypto_pwhash_str_verify(account.password_hash, password, strlen(password)) == 0;
}

#endif
