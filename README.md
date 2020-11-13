# Lighthouse and AI Deck Compatibility

In order to use the CF2 with the AI-Deck and the Lighthouse, it is necessary to switch the communication of the GAP8 over the NINA and UART2.

The GAP8 of the AI-Deck and the FPGA of the Lighthouse-Deck share the same UART1 connection, which is why they cannot communicate with the CF2 at the same time. To fix that cutting of communication, the flow of data from the GAP8-chip must be diverted via SPI to the ESP32 based NINA-chip and then through UART2 to the core.

In order for the data transmission to work, the NINA and the Gap8 firmware must be rewritten, the new send functions must be integrated in the Gap8-program and then the new firmware with the application must be flashed on the GAP8 and the NINA-chip.