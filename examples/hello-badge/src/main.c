// hello-badge: buttons, LED and buzzer in ~100 lines.
//
// - Green debug LED (GPIO2) heartbeats once a second.
// - Each button press prints its name on the USB serial console (115200)
//   and beeps the buzzer at a button-specific pitch.
// - BOOT (SW2/GPIO0) counts as a 7th button.
//
// Buttons are active-low with external 10K pullups; debounce caps are not
// populated on the board, so we debounce in software.

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#define PIN_LED    2
#define PIN_BUZZER 9

typedef struct { gpio_num_t gpio; const char *name; int freq_hz; } button_t;

static const button_t buttons[] = {
    { GPIO_NUM_3, "LEFT",  2000 },
    { GPIO_NUM_4, "UP",    2500 },
    { GPIO_NUM_5, "DOWN",  3000 },
    { GPIO_NUM_6, "RIGHT", 3500 },
    { GPIO_NUM_7, "B",     4000 },  // 4 kHz = piezo resonance, loudest
    { GPIO_NUM_8, "A",     4500 },
    { GPIO_NUM_0, "BOOT",  5000 },
};
#define N_BUTTONS (sizeof(buttons)/sizeof(buttons[0]))

static void beep(int freq_hz, int ms)
{
    ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, freq_hz);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 512);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    vTaskDelay(pdMS_TO_TICKS(ms));
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void app_main(void)
{
    gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);

    for (int i = 0; i < N_BUTTONS; i++) {
        gpio_reset_pin(buttons[i].gpio);
        gpio_set_direction(buttons[i].gpio, GPIO_MODE_INPUT);
        gpio_set_pull_mode(buttons[i].gpio, GPIO_PULLUP_ONLY);
    }

    // Buzzer PWM (50% duty square wave when beeping)
    ledc_timer_config_t tcfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 4000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&tcfg);
    ledc_channel_config_t ccfg = {
        .gpio_num = PIN_BUZZER,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };
    ledc_channel_config(&ccfg);

    printf("\nhello-badge: press some buttons!\n");
    beep(4000, 100);

    int held[N_BUTTONS] = {0};
    int led = 0, tick = 0;

    while (1) {
        for (int i = 0; i < N_BUTTONS; i++) {
            int down = !gpio_get_level(buttons[i].gpio);
            if (down && !held[i]) {
                printf("button: %s\n", buttons[i].name);
                beep(buttons[i].freq_hz, 60);
            }
            held[i] = down;
        }
        if (++tick >= 50) {  // ~1s heartbeat
            tick = 0;
            gpio_set_level(PIN_LED, led ^= 1);
        }
        vTaskDelay(pdMS_TO_TICKS(20));  // 20ms scan = software debounce
    }
}
