// GDMP Tests

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <gdmp.h>
#include <hash_table.h>

void test_gdmp_new(void);
void test_gdmp_stringify(void);
void test_gdmp_parse(void);

int main(void) {
    test_gdmp_new();
    test_gdmp_stringify();
    test_gdmp_parse();

    printf("All ThreadPool tests passed\n");
    return 0;
}

void test_gdmp_new(void) {
    GDMPMessage msg = gdmp_new(GDMP_MESSAGE);
    assert(msg != NULL);
    gdmp_free(msg);
}

void test_gdmp_stringify(void) {
    GDMPMessage msg = gdmp_new(GDMP_MESSAGE);
    
    gdmp_add(msg, "Username", "Will");
    gdmp_add(msg, "Timestamp", "2025-01-26T12:34:56Z");
    gdmp_add(msg, "Content", "G'day mate!");

    char *str = gdmp_stringify(msg);
    assert(str != NULL);

    assert(strstr(str, "Username: Will\n") != NULL);
    assert(strstr(str, "Timestamp: 2025-01-26T12:34:56Z\n") != NULL);
    assert(strstr(str, "Content: G'day mate!\n") != NULL);

    gdmp_free(msg);
    free(str);
}

void test_gdmp_parse(void) {
    char *str = "GDMP_MESSAGE\n"
                "Username: Will\n"
                "Timestamp: 2025-01-26T12:34:56Z\n"
                "Content: G'day mate!\n";

    GDMPMessage msg = gdmp_parse(str);
    assert(msg != NULL);

    assert(gdmp_get_type(msg) == GDMP_MESSAGE);

    assert(strcmp(gdmp_get(msg, "Username"), "Will") == 0);
    assert(strcmp(gdmp_get(msg, "Timestamp"), "2025-01-26T12:34:56Z") == 0);
    assert(strcmp(gdmp_get(msg, "Content"), "G'day mate!") == 0);

    gdmp_free(msg);
}
