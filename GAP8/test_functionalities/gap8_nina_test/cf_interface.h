#include <stdint.h>
#include <stdbool.h>

#ifndef AI_DECK_INTERFACE_H
#define AI_DECK_INTERFACE_H

void cf_interface_init();
void cf_send(uint8_t *buffer, int len);

#endif