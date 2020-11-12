# Lighthouse and AI Deck Compatibility

In order to use the CF2 with the AI-Deck and the Lighthouse, it is necessary to switch the communication of the GAP8 over the NINA and UART2.

The GAP8 of the AI-Deck and the FPGA of the Lighthouse-Deck share the same UART1 connection, which is why they cannot communicate with the CF2 at the same time. To fix that cutting of communication, the flow of data from the GAP8-chip must be diverted via SPI to the ESP32 based NINA-chip and then through UART2 to the core.

![Screenshot-from-2020-06-03-11-12-51-768x529](/uploads/a345b6f9cb094068d9abeb76adbc0e1b/Screenshot-from-2020-06-03-11-12-51-768x529.png)