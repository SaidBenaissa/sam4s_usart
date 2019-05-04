/*
 * main.c
 * 
 * Created on: April 22, 2019
 * Author: John Cobb
 */

#include <asf.h>
#include <stdio.h>
#include <string.h>
#include "cph_config.h"
#include "cph_millis.h"
#include "cph_util.h"
#include "cph_console.h"
#include "cph_cli.h"
#include "cph_uart.h"

void app_init(void);
bool app_timeout(clock_time_t);
// void app_settimer(clock_time_t *, clock_time_t);
void app_settimer(clock_time_t *, clock_time_t);
void load_uart_config(void);
void app_message(uart_cfg_t *, char *);

typedef struct {
    clock_time_t log_timer;
    clock_time_t led_timer;
} app_timer_t;

volatile app_timer_t app_timer;


void uart_ondatareceived(uint8_t *buffer, uint32_t len);

void uart_ondatareceived(uint8_t *buffer, uint32_t len) {
    // ioport_toggle_pin_level(LED0_GPIO);
    // delay_ms(50);
    // ioport_toggle_pin_level(LED0_GPIO);

}

void app_init(void) {
    // initialize timers to zero
    app_timer.log_timer = 0;
    app_timer.led_timer = 0;
}


int main(void)
{
    sysclk_init();
    board_init();
    delay_init();
    config_init();
    cph_millis_init();

    const sam_uart_opt_t sam_uart_opt = {
        sysclk_get_cpu_hz(),
        UART1_SERIAL_BAUDRATE, 
        UART1_SERIAL_MODE
    };

    const uart_cfg_t u_cfg = {
        sam_uart_opt,
        UART1,
        ID_UART1,
        PINS_UART1_PIO,
        PINS_UART1_TYPE,
        PINS_UART1_MASK,
        PINS_UART1_ATTR,
        UART1_IRQn,
        UART_IER_RXRDY,
        uart_ondatareceived
    };

    /* configure uart */
    lib_uart_cfg(&u_cfg);

    /* initialize variables and callbacks for our task processor */
    app_init();

    app_message(&u_cfg, "Welcome to sam4s_uart example.\r\n");
    while(1) {

        if ((app_timeout(app_timer.log_timer))) {
            // send a message every second
            // app_message(&u_cfg);
            // reset timer

            app_message(&u_cfg, "Hello World, how are you?\r\n");
            app_settimer(&app_timer.log_timer, 1000);
        }

        if ((app_timeout(app_timer.led_timer))) {
            // toggle led
            ioport_toggle_pin_level(LED0_GPIO);
            app_settimer(&app_timer.led_timer, 100);
        }
        
        /* process any data on uart */
        lib_uart_tick(&u_cfg);

        delay_ms(50);
    }
}

void app_message(uart_cfg_t *uart_cfg, char *msg) {
    // buffer used to sprintf timestamp into message
    char buffer[64] = {0};
    // data used to store the message
    char data[] = "%lu    %s\r\n";

    // copy the timstamp into data and store into buffer
    sprintf(buffer, data, g_cph_millis, msg);

    // write to uart1
    // uart_uart1_write_bytes(buffer, sizeof(data));
    lib_uart_writebytes(uart_cfg, buffer, sizeof(buffer));   
}

void app_settimer(clock_time_t *timer, clock_time_t timeout) {
    *timer = g_cph_millis + timeout;
}

bool app_timeout(clock_time_t timer) {
    if (g_cph_millis >= timer) {
        return true;
    }

    return false;
}

