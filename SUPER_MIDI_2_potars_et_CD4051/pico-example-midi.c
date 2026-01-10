#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "bsp/board.h"
#include "tusb.h"

// =====================================================
// MIDI CONFIG
// =====================================================
#define MIDI_CHANNEL    8   // 0 = Channel 1
#define MIDI_THRESHOLD  1
#define MIDI_INTERVAL_MS 10

// =====================================================
// DIRECT POTENTIOMETERS (A1, A2)
// =====================================================
#define NUM_DIRECT_POTS 2

typedef struct {
    uint8_t adc_input;
    uint8_t cc_number;
    uint8_t last_cc;
} pot_t;

pot_t direct_pots[NUM_DIRECT_POTS] = {
    {1, 1, 0},   // A0 -> CC1
    {2, 2, 0},   // A1 -> CC2
};

// =====================================================
// CD4051 CONFIG
// =====================================================
#define S0_PIN   0
#define S1_PIN   1
#define S2_PIN   2
#define INH_PIN  3
#define ADC_MUX_INPUT 0   // ADC0 / GPIO26

#define NUM_MUX_POTS 8
uint8_t mux_cc_nums[NUM_MUX_POTS] = {10,11,12,13,14,15,16,17};
uint8_t mux_last_cc[NUM_MUX_POTS] = {0};

// =====================================================
// CD4051 FUNCTIONS
// =====================================================
void cd4051_init(void) {
    gpio_init(S0_PIN);
    gpio_init(S1_PIN);
    gpio_init(S2_PIN);
    gpio_init(INH_PIN);

    gpio_set_dir(S0_PIN, true);
    gpio_set_dir(S1_PIN, true);
    gpio_set_dir(S2_PIN, true);
    gpio_set_dir(INH_PIN, true);

    gpio_put(INH_PIN, true);
}

void cd4051_select(uint8_t ch) {
    gpio_put(INH_PIN, true);
    gpio_put(S0_PIN, ch & 0x01);
    gpio_put(S1_PIN, ch & 0x02);
    gpio_put(S2_PIN, ch & 0x04);
    gpio_put(INH_PIN, false);
    sleep_us(10);
}

uint16_t cd4051_read(uint8_t ch) {
    cd4051_select(ch);
    return adc_read();
}

// =====================================================
// MIDI SEND
// =====================================================
void send_cc(uint8_t cc, uint8_t value) {
    uint8_t msg[3];
    msg[0] = 0xB0 | (MIDI_CHANNEL & 0x0F);
    msg[1] = cc;
    msg[2] = value;
    tud_midi_n_stream_write(0, 0, msg, 3);
}

// =====================================================
// MIDI TASK
// =====================================================
void midi_task(void) {
    static uint32_t last_ms = 0;
    if (board_millis() - last_ms < MIDI_INTERVAL_MS) return;
    last_ms += MIDI_INTERVAL_MS;

    // -------- Direct pots --------
    for (int i = 0; i < NUM_DIRECT_POTS; i++) {
        adc_select_input(direct_pots[i].adc_input);
        uint16_t raw = adc_read();
        uint8_t cc = (raw * 127) / 4095;

        if (abs(cc - direct_pots[i].last_cc) > MIDI_THRESHOLD) {
            send_cc(direct_pots[i].cc_number, cc);
            direct_pots[i].last_cc = cc;
        }
    }

    // -------- CD4051 pots --------
    adc_select_input(ADC_MUX_INPUT);
    for (uint8_t ch = 0; ch < NUM_MUX_POTS; ch++) {
        if (ch != 0)
            break;
        uint16_t raw = cd4051_read(ch);
        uint8_t cc = (raw * 127) / 4095;

        if (abs(cc - mux_last_cc[ch]) > MIDI_THRESHOLD) {
            send_cc(mux_cc_nums[ch], cc);
            mux_last_cc[ch] = cc;
        }
    }
}

// =====================================================
// MAIN
// =====================================================
int main(void) {
    board_init();
    tusb_init();

    adc_init();
    adc_gpio_init(26); // ADC0
    adc_gpio_init(27); // ADC1

    cd4051_init();

    while (1) {
        tud_task();
        midi_task();
    }
}
