#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"

#include "variaveis_globais.h"
#include "functions.h"
#include "ssd1306.h"




int main(){

    stdio_init_all();
    setup_init_i2c1();
    setup_init_uart_custom();


    limpar_m_json_char(&json_char);
    limpar_m_json_int(&json_int);

    sleep_ms(4000);
    printf("iniciando...\n");

    xTaskCreate(task_ler_sensores, "Leitura_sensores", 2048, NULL, 1, NULL);        
    // xTaskCreate(task_leitura_uart_receiver, "Leitura_uart_rx", 4096, NULL, 1, NULL);    
    // xTaskCreate(task_leitura_serial_receiver, "Leitura_Serial_usb", 4096, NULL, 1, NULL);
    // xTaskCreate(task_exibir_infos_OLED, "task_exibir_infos_OLED", 4096, NULL, 2, NULL);
    
    vTaskStartScheduler(); // Depois que o Scheduler começa, nada mais é lido no main()


}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    printf("STACK OVERFLOW na task: %s\n", pcTaskName);
    while (1);
}


