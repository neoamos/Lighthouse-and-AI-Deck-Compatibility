#include <stdint.h>
#include <stdbool.h>


#ifndef CF_INTERFACE_H
#define CF_INTERFACE_H

/*
 * Initiate a interface and provide a callback function for 
 * receiving messages.  Callback can be NULL if you don't
 * need to receive messages.
 */
void ai_deck_interface_init(void (*callback) (uint8_t*, int));

/*
 * Send a message to the AI deck
 */
void ai_deck_send(uint8_t *buffer, int len);

#endif