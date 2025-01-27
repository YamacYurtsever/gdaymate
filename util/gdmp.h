// GDMP Interface

#ifndef GDMP_H
#define GDMP_H

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
 */
GDMPMessage GDMPParse(char *str);

#endif
