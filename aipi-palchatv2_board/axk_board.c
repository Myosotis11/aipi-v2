#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>

#include "bflb_gpio.h"
#include "axk_board.h"

#define BOARD_PWR_EN_PIN 13
#define BOARD_PWR_DN_PIN 14

static struct bflb_device_s *gpio;

void gpio0_isr(uint8_t pin)
{
    if (pin == BOARD_PWR_DN_PIN) {
        if (xTaskGetTickCountFromISR() > 5000) {
            bflb_gpio_reset(gpio, BOARD_PWR_EN_PIN);
        }
    }
}

int axk_board_init(void)
{
    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, BOARD_PWR_EN_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_set(gpio, BOARD_PWR_EN_PIN);
    bflb_gpio_init(gpio, BOARD_PWR_DN_PIN, GPIO_INPUT | GPIO_SMT_EN);
    bflb_gpio_int_init(gpio, BOARD_PWR_DN_PIN, GPIO_INT_TRIG_MODE_SYNC_FALLING_EDGE);
    bflb_gpio_irq_attach(BOARD_PWR_DN_PIN, gpio0_isr);
    bflb_irq_enable(gpio->irq_num);

    return 0;
}
