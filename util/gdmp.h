// GDMP Interface

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

enum message_type {
    GDMP_MESSAGE,
    GDMP_ACK,
    GDMP_AUTH,
    GDMP_INVALID,
};

typedef struct gdmp_message *GDMPMessage;
typedef enum message_type MessageType;

/**
 * Creates a GDMP message.
 */
GDMPMessage gdmp_new(MessageType type);

/**
 * Frees a GDMP message.
 */
void gdmp_free(GDMPMessage msg);

/**
 * Adds a header-value pair to a GDMP message.
 * Replaces the value if header already exists.
 */
void gdmp_add(GDMPMessage msg, char *header, char *value);

/**
 * Returns a header's value in the GDMP message.
 * Returns NULL if it doesn't exist.
 */
char *gdmp_get(GDMPMessage msg, char *header);

/**
 * Returns the message type of the gdmp message.
 */
MessageType gdmp_get_type(GDMPMessage msg);

/**
 * Serializes a GDMP message into a string.
 */
char *gdmp_stringify(GDMPMessage msg);

/**
 * Deserializes a string into a GDMP message.
 */
GDMPMessage gdmp_parse(char *str);

#endif
