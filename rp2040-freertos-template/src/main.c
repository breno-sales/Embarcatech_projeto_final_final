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


void loop_uart_rx(void) {

    char buffer[Max_buffer_size];
    int idx = 0;

    while (1) {

        // espera dado chegar
        // printf("aguardando...\n");
        if (uart_is_readable(UART_ID)) {

            char c = uart_getc(UART_ID);
            printf("aqui: %c",c);

            // evita overflow
            if (idx < Max_buffer_size - 1) {
                buffer[idx++] = c;
            }

            // fim do JSON
            if (c == '}') {
                buffer[idx] = '\0';

                printf("\nMensagem recebida completa:\n%s\n", buffer);

                // ================= PROCESSAMENTO =================
                limpeza_dados_entrada(buffer);

                verificar_dado_em_json_especifico(buffer, json_remetente, json_char.resp_remetente);
                verificar_dado_em_json_especifico(buffer, json_destinatario, json_char.resp_destinatario);
                verificar_dado_em_json_especifico(buffer, json_tipo_sensor, json_char.resp_tipo_sensor);
                verificar_dado_em_json_especifico(buffer, json_nome_sensor, json_char.resp_nome_sensor);
                verificar_dado_em_json_especifico(buffer, json_acao, json_char.resp_acao);
                verificar_dado_em_json_especifico(buffer, json_gpio_pino, json_char.resp_gpio_pino);
                json_int.resp_gpio_pino = verificar_porta_acao_especifica(buffer, json_gpio_pino);

                retorno_requisicao_json("master_central", &json_char);

                printf("Processado com sucesso!\n");

                // sinaliza outras partes do sistema
                exibir_OLED = true;

                // reseta buffer para próxima mensagem
                idx = 0;
                memset(buffer, 0, sizeof(buffer));
            }
        }

        // evita busy-wait agressivo
        sleep_ms(1);
    }
}



int main(){

    stdio_init_all();
    setup_init_i2c0();
    setup_init_i2c1();
    setup_init_uart_custom();


    limpar_m_json_char(&json_char);
    limpar_m_json_int(&json_int);

    sleep_ms(5000);
    printf("iniciando...\n");

    // xTaskCreate(task_ler_sensores, "Leitura_sensores", 2048, NULL, 1, NULL);        
    // xTaskCreate(task_leitura_uart_receiver, "Leitura_uart_rx", 4096, NULL, 1, NULL);    
    xTaskCreate(task_leitura_serial_receiver, "Leitura_Serial_usb", 4096, NULL, 1, NULL);
    xTaskCreate(task_exibir_infos_OLED, "task_exibir_infos_OLED", 4096, NULL, 2, NULL);
    
    vTaskStartScheduler(); // Depois que o Scheduler começa, nada mais é lido no main()
    // loop_uart_rx();


    // while (1) {

    // const char *mensagem =
    //     "{\"remetente\":\"bitdoglab\","
    //     "\"destinatario\":\"master_central\","
    //     "\"tipo_sensor\":\"digital\","
    //     "\"nome_sensor\":\"led\","
    //     "\"acao\":\"ligar\","
    //     "\"gpio\":11}";

    // uart_puts(UART_ID, mensagem);
    // uart_puts(UART_ID, "\n");   // delimitador (importante)

    // printf("Mensagem enviada via UART\n");

    // sleep_ms(3000);  // envia a cada 1 segundo
    // }

    // ler_endereco_i2c();
}

