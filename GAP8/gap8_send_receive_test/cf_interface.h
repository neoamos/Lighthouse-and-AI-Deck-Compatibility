#include <stdint.h>
#include <stdbool.h>

#ifndef AI_DECK_INTERFACE_H
#define AI_DECK_INTERFACE_H

/*
 * Initiate the interface with the crazyflie and
 * provide a callback function for receiving messages.
 * The callback can be NULL if you don't need to receive
 * messages.
 */
void cf_interface_init(void (*callback) (uint8_t*, int));


/*
 * Send a message to the crazyflie
 */
void cf_send(uint8_t *buffer, int len);

#endif