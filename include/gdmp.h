// GDMP Interface

#ifndef GDMP_H
#define GDMP_H

#define GDMP_MESSAGE_MAX_LEN 1024
#define GDMP_HEADERS_MAX_COUNT 10

#define GDMP_USERNAME_MAX_LEN 10
#define GDMP_CONTENT_MAX_LEN 50
#define GDMP_TIMESTAMP_MAX_LEN 10

enum message_type {
    GDMP_TEXT_MESSAGE,
    GDMP_JOIN_MESSAGE,
    GDMP_ERROR_MESSAGE,
};

typedef struct gdmp_message *GDMPMessage;
typedef enum message_type MessageType;

/**
 * Creates a GDMP message.
 */
GDMPMessage GDMPNew(MessageType type);

/**
 * Frees a GDMP message.
 */
void GDMPFree(GDMPMessage msg);

/**
 * Adds a header with the given value to a GDMP message.
 * Replaces the value if header already exists.
 */
void GDMPAddHeader(GDMPMessage msg, char *header, char *value);

/**
 * Returns the message type of the gdmp message.
 */
MessageType GDMPGetType(GDMPMessage msg);

/**
 * Returns a header's value in the GDMP message.
 * Returns NULL if it doesn't exist.
 */
char *GDMPGetValue(GDMPMessage msg, char *header);

/**
 * Serializes a GDMP message into a string.
 */
char *GDMPStringify(GDMPMessage msg);

/**
 * Deserializes a string into a GDMP message.
 * Assumes the given string is a valid GDMP message.
 */
GDMPMessage GDMPParse(char *str);

#endif
