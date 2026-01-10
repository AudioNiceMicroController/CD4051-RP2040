#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/adc.h"

#include "bsp/board.h"
#include "tusb.h"

// --------------------------------------------------
// SPI MCP3008 (SPI0 sur GP2–GP5)
// --------------------------------------------------
#define SPI_PORT spi0
#define PIN_SCK   2
#define PIN_MOSI  3
#define PIN_MISO  4
#define PIN_CS    5

// --------------------------------------------------
// MIDI configuration
// --------------------------------------------------
#define MIDI_CHANNEL     6
#define MIDI_THRESHOLD   3

#define MCP_CC_START     1   // CC 1 → 8
#define ADC_CC_START     9   // CC 9 → 12

// ADC Pico pins
uint8_t adc_pins[4] = {26, 27, 28, 29};

// --------------------------------------------------
// MCP3008 read (0–7 → 0–1023)
// --------------------------------------------------
uint16_t mcp3008_read(uint8_t channel)
{
    uint8_t tx[3] = {
        0x01,
        (0x08 | channel) << 4,
        0x00
    };
    uint8_t rx[3];

    gpio_put(PIN_CS, 0);
    spi_write_read_blocking(SPI_PORT, tx, rx, 3);
    gpio_put(PIN_CS, 1);

    return ((rx[1] & 0x03) << 8) | rx[2];
}

// --------------------------------------------------
// MIDI task
// --------------------------------------------------
void midi_task(void)
{
    static uint8_t last_cc[12] = {0};
    static uint32_t last_ms = 0;
    uint8_t msg[3];

    if (board_millis() - last_ms < 10) return;
    last_ms += 10;

    // ---------- MCP3008 (8 potars) ----------
    for (int i = 0; i < 1; i++)
    {
        uint16_t val = mcp3008_read(i);
        uint8_t cc = (val * 127) / 1023;

        if (abs(cc - last_cc[i]) > MIDI_THRESHOLD)
        {
            msg[0] = 0xB0 | (MIDI_CHANNEL & 0x0F);
            msg[1] = MCP_CC_START + i;
            msg[2] = cc;
            tud_midi_n_stream_write(0, 0, msg, 3);
            last_cc[i] = cc;
        }
    }

    // ---------- ADC Pico (4 potars) ----------
    for (int i = 0; i < 1; i++)
    {
        adc_select_input(i);
        uint16_t val = adc_read();
        uint8_t cc = (val * 127) / 4095;

        int idx = 8 + i;
        if (abs(cc - last_cc[idx]) > MIDI_THRESHOLD)
        {
            msg[0] = 0xB0 | (MIDI_CHANNEL & 0x0F);
            msg[1] = ADC_CC_START + i;
            msg[2] = cc;
            tud_midi_n_stream_write(0, 0, msg, 3);
            last_cc[idx] = cc;
        }
    }
}

// --------------------------------------------------
// Main
// --------------------------------------------------
int main(void)
{
    board_init();
    tusb_init();

    // SPI init (MCP3008)
    spi_init(SPI_PORT, 1 * 1000 * 1000);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    // ADC Pico init
    adc_init();
    for (int i = 0; i < 4; i++)
        adc_gpio_init(adc_pins[i]);

    while (1)
    {
        tud_task();
        midi_task();
    }
}
