// GDMP Tests

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <gdmp.h>
#include <hash_table.h>

void test_GDMPNew(void);
void test_GDMPStringify(void);
void test_GDMPParse(void);

int main(void) {
    test_GDMPNew();
    test_GDMPStringify();
    test_GDMPParse();

    printf("All ThreadPool tests passed\n");
    return 0;
}

void test_GDMPNew(void) {
    GDMPMessage msg = GDMPNew(GDMP_MESSAGE);
    assert(msg != NULL);
    GDMPFree(msg);
}

void test_GDMPStringify(void) {
    GDMPMessage msg = GDMPNew(GDMP_MESSAGE);
    
    GDMPAddHeader(msg, "Username", "Will");
    GDMPAddHeader(msg, "Timestamp", "2025-01-26T12:34:56Z");
    GDMPAddHeader(msg, "Content", "G'day mate!");

    char *str = GDMPStringify(msg);
    assert(str != NULL);

    assert(strstr(str, "Username: Will\n") != NULL);
    assert(strstr(str, "Timestamp: 2025-01-26T12:34:56Z\n") != NULL);
    assert(strstr(str, "Content: G'day mate!\n") != NULL);

    GDMPFree(msg);
    free(str);
}

void test_GDMPParse(void) {
    char *str = "GDMP_MESSAGE\n"
                "Username: Will\n"
                "Timestamp: 2025-01-26T12:34:56Z\n"
                "Content: G'day mate!\n";

    GDMPMessage msg = GDMPParse(str);
    assert(msg != NULL);

    assert(GDMPGetType(msg) == GDMP_MESSAGE);

    assert(strcmp(GDMPGetValue(msg, "Username"), "Will") == 0);
    assert(strcmp(GDMPGetValue(msg, "Timestamp"), "2025-01-26T12:34:56Z") == 0);
    assert(strcmp(GDMPGetValue(msg, "Content"), "G'day mate!") == 0);

    GDMPFree(msg);
}
