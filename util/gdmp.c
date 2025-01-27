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

MessageType map_str_to_type(char *message_type_string);
char **get_headers(MessageType type);

////////////////////////////////// FUNCTIONS ///////////////////////////////////

GDMPMessage gdmp_new(MessageType type) {
    GDMPMessage msg = malloc(sizeof(*msg));
    if (msg == NULL) {
        perror("malloc");
        return NULL;
    }

    msg->type = type;
    msg->data = HashTableNew(HEADERS_MAX_COUNT);

    return msg;
}

void gdmp_free(GDMPMessage msg) {
    HashTableFree(msg->data);
    free(msg);
}

void gdmp_add(GDMPMessage msg, char *header, char *value) {
    HashTableInsert(msg->data, header, value);
}

char *gdmp_get(GDMPMessage msg, char *header) {
    if (!HashTableContains(msg->data, header)) return NULL;
    return HashTableGet(msg->data, header);
}

MessageType gdmp_get_type(GDMPMessage msg) {
    return msg->type;
}

char *gdmp_stringify(GDMPMessage msg) {
    char *str = malloc(MESSAGE_MAX_LEN);
    if (str == NULL) {
        perror("malloc");
        return NULL;
    }
    str[0] = '\0';

    MessageType type = gdmp_get_type(msg);
    char **headers = get_headers(type);

    for (int i = 0; i < HEADERS_MAX_COUNT && headers[i] != NULL; i++) {
        // Get a header-value pair
        char *header = headers[i];
        char *value = gdmp_get(msg, header);
        if (value == NULL) continue;

        // Concatenate the pair on top of the string
        char pair[MESSAGE_MAX_LEN];
        snprintf(pair, sizeof(pair), "%s: %s\n", header, value);
        strcat(str, pair);
    }

    free(headers);
    return str;
}

GDMPMessage gdmp_parse(char *str) {
    // Extract message type
    char *pos = strchr(str, '\n');
    int message_type_len = pos - str;

    char message_type_str[MESSAGE_MAX_LEN + 1];
    strncpy(message_type_str, str, message_type_len);
    message_type_str[message_type_len] = '\0';
    
    // Set message type
    MessageType type = map_str_to_type(message_type_str);
    GDMPMessage msg = gdmp_new(type);
    str = pos + 1;

    char *pair;
    while ((pair = strsep(&str, "\n")) != NULL) {
        // Extract header-value pair
        pos = strstr(pair, ": ");
        int header_len = pos - pair;

        char header[MESSAGE_MAX_LEN];
        strncpy(header, pair, header_len);
        header[header_len] = '\0';

        char *value = pos + 2;

        // Set header-value pair
        gdmp_add(msg, header, value);
    }

    return msg;
}

////////////////////////////// HELPER FUNCTIONS ////////////////////////////////

/** 
 * Returns an array of headers used by the given message type (NULL terminated).
 */
char **get_headers(MessageType type) {
    char **headers = malloc(sizeof(char *) * HEADERS_MAX_COUNT);
    if (headers == NULL) {
        return NULL;
    }

    switch (type) {
        case GDMP_MESSAGE:
            headers[0] = "Username";
            headers[1] = "Timestamp";
            headers[2] = "Content";
            headers[3] = NULL;  // Null-terminate the array
            break;
        case GDMP_ACK:
            headers[0] = NULL;  // No headers for GDMP_ACK
            break;
        case GDMP_AUTH:
            headers[0] = NULL;  // No headers for GDMP_AUTH
            break;
        default:
            free(headers);
            return NULL;
    }

    return headers;
}

/** 
 * Convert the given message type string into a message type enum
 */
MessageType map_str_to_type(char *message_type_str) {
    if (strcmp(message_type_str, "GDMP_MESSAGE") == 0) {
        return GDMP_MESSAGE;
    } else if (strcmp(message_type_str, "GDMP_ACK") == 0) {
        return GDMP_MESSAGE;
    } else if (strcmp(message_type_str, "GDMP_AUTH") == 0) {
        return GDMP_MESSAGE;
    } else {
        return GDMP_INVALID;
    }
}
