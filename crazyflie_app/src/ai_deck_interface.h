#include <stdint.h>
#include <stdbool.h>


#ifndef CF_INTERFACE_H
#define CF_INTERFACE_H

void ai_deck_interface_init(void (*callback) (uint8_t*, int));
void ai_deck_send(uint8_t *buffer, int len);

#endif