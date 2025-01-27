// GDMP Implementation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gdmp.h"
#include "hash_table.h"

#define MESSAGE_MAX_LEN 1024
#define HEADERS_MAX_COUNT 10

struct gdmp_message {
    MessageType type;
    HashTable data;
};

char **get_headers(MessageType type);
MessageType str_to_type(char *type_string);
char *type_to_str(MessageType type);

////////////////////////////////// FUNCTIONS ///////////////////////////////////

GDMPMessage GDMPNew(MessageType type) {
    GDMPMessage msg = malloc(sizeof(*msg));
    if (msg == NULL) {
        perror("malloc");
        return NULL;
    }

    msg->type = type;
    msg->data = HashTableNew(HEADERS_MAX_COUNT);

    return msg;
}

void GDMPFree(GDMPMessage msg) {
    HashTableFree(msg->data);
    free(msg);
}

void GDMPAddHeader(GDMPMessage msg, char *header, char *value) {
    HashTableInsert(msg->data, header, value);
}

char *GDMPGetValue(GDMPMessage msg, char *header) {
    if (!HashTableContains(msg->data, header)) return NULL;
    return HashTableGet(msg->data, header);
}

MessageType GDMPGetType(GDMPMessage msg) {
    return msg->type;
}

char *GDMPStringify(GDMPMessage msg) {
    char *str = malloc(MESSAGE_MAX_LEN);
    if (str == NULL) {
        perror("malloc");
        return NULL;
    }
    str[0] = '\0';

    MessageType type = GDMPGetType(msg);
    char **headers = get_headers(type);

    // Concatenate the type to the string
    char *type_str = type_to_str(type);
    strcat(str, type_str);
    strcat(str, "\n");

    for (int i = 0; i < HEADERS_MAX_COUNT && headers[i] != NULL; i++) {
        // Get a header and its value
        char *header = headers[i];
        char *value = GDMPGetValue(msg, header);
        if (value == NULL) continue;

        // Form the pair string
        char pair[MESSAGE_MAX_LEN];
        snprintf(pair, sizeof(pair), "%s: %s\n", header, value);

        // Concatenate the pair to the string
        if (strlen(str) + strlen(pair) < MESSAGE_MAX_LEN) {
            strcat(str, pair);
        } else {
            break;
        }
    }

    str[strlen(str) - 1] = '\0';
    free(headers);
    return str;
}

GDMPMessage GDMPParse(char *str) {
    // Extract message type
    char *pos = strchr(str, '\n');
    int message_type_len = pos - str;
    char type_str[MESSAGE_MAX_LEN + 1];
    strncpy(type_str, str, message_type_len);
    type_str[message_type_len] = '\0';

    // Set message type
    MessageType type = str_to_type(type_str);
    if (type == GDMP_INVALID) return NULL;
    GDMPMessage msg = GDMPNew(type);
    str = pos + 1;

    char *pair;
    char *str_copy = strdup(str);

    while ((pair = strsep(&str_copy, "\n")) != NULL) {
        // Exit condition
        if (*pair == '\0') {
            break;
        }

        // Extract header-value pair
        pos = strstr(pair, ": ");
        *pos = '\0';
        char *header = pair;
        char *value = pos + 2;

        // Set header-value pair
        GDMPAddHeader(msg, header, value);
    }

    free(str_copy);
    return msg;
}

////////////////////////////// HELPER FUNCTIONS ////////////////////////////////

/** 
 * Returns an array of headers used by the given message type (NULL terminated).
 * Returns NULL if the message type is invalid.
 */
char **get_headers(MessageType type) {
    char **headers = calloc(HEADERS_MAX_COUNT + 1, sizeof(char *)); 
    if (headers == NULL) {
        return NULL;
    }

    switch (type) {
        case GDMP_MESSAGE:
            headers[0] = "Username";
            headers[1] = "Content";
            headers[2] = "Timestamp";
            break;
        case GDMP_AUTH:
            break;
        default:
            free(headers);
            return NULL;
    }

    return headers;
}

/** 
 * Converts the given message type string into a message type enum.
 * Returns GDMP_INVALID if the given string is invalid.
 */
MessageType str_to_type(char *str) {
    if (strcmp(str, "GDMP_MESSAGE") == 0) {
        return GDMP_MESSAGE;
    } else if (strcmp(str, "GDMP_AUTH") == 0) {
        return GDMP_AUTH;
    } else {
        return GDMP_INVALID;
    }
}

/** 
 * Converts the given message type enum into a message type string.
 * Returns NULL if the message type is invalid.
 */
char *type_to_str(MessageType type) {
    if (type == GDMP_MESSAGE) {
        return "GDMP_MESSAGE";
    } else if (type == GDMP_AUTH) {
        return "GDMP_AUTH";
    } else {
        return NULL;
    }
}
