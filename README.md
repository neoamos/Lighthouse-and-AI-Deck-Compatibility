# Lighthouse and AI Deck Compatibility

In order to use the CF2 with the AI-Deck and the Lighthouse, it is necessary to switch the communication of the GAP8 over the NINA and UART2.

The GAP8 of the AI-Deck and the FPGA of the Lighthouse-Deck share the same UART1 connection, which is why they cannot communicate with the CF2 at the same time. To fix that cutting of communication, the flow of data from the GAP8-chip must be diverted via SPI to the ESP32 based NINA-chip and then through UART2 to the core.

![Screenshot-from-2020-06-03-11-12-51-768x529](https://s3.rwth-aachen.de/gitrwthuploads/%40hashed/ed/2e/ed2e3dda0b53826ab85a68f18f5f19f98142c96a80622375fa1aab6ff3d51b20/a345b6f9cb094068d9abeb76adbc0e1b/Screenshot-from-2020-06-03-11-12-51-768x529.png?response-content-disposition=inline%3B%20filename%3D%22Screenshot-from-2020-06-03-11-12-51-768x529.png%22%3B%20filename%2A%3DUTF-8%27%27Screenshot-from-2020-06-03-11-12-51-768x529.png&response-content-type=image%2Fpng&AWSAccessKeyId=5RLMCXNCZREE21JQNG0H&Signature=uHJscyFZhq0pJCTuQAfV9gHh6N8%3D&Expires=1605173610)

Overview of the scheme taken from [blog post](https://www.bitcraze.io/2020/06/ai-deck-is-available-in-early-access/)

In order for the data transmission to work, the NINA and the Gap8 firmware must be rewritten, the new send functions must be integrated in the Gap8-program and then the new firmware with the appllication must be flashed on the GAP8 and the NINA-chip.