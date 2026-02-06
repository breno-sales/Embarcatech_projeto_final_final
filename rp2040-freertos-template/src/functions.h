#ifndef FUNCTIONS_H
#define FUNCTIONS_H

/* ======================= INCLUDES ======================= */
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

/* ======================= TASKS ======================= */

void task_leitura_serial_receiver(void *pvParameters);

void task_leitura_uart_receiver(void *pvParameters);

void task_uart_transmitir(void *pvParameters);

void task_exibir_infos_OLED(void *pvParameters);

void task_ler_sensores(void *pvParameters);

/* ======================= ATUADORES ======================= */

bool luz_normal(int pino_GPIO, char *acao);

/* ======================= MASTER / ORQUESTRAÇÃO ======================= */

bool orquestrador_funcionalidades(m_json_char *json_c , m_json_int *json_i);

void limpeza_dados_entrada(char *buffer);

int verificar_porta_acao_especifica(char *buffer,char *comando);

void setup_init_i2c1(void);

void setup_init_uart_custom(void);

bool verificar_dado_em_json_especifico(char *buffer, char *comando, char *saida);

/* ======================= JSON / RETORNO / UART ======================= */

void retorno_requisicao_json(const char *origin, m_json_char *json_c);

void limpar_m_json_char(m_json_char *json_c);

void limpar_m_json_int(m_json_int *json_i);

void montagem_estrutura_de_envio(m_json_char *json_c,m_json_int *json_i);

void uart_read_exact(uart_inst_t *uart, uint8_t *buf, size_t len);

void receber_mensagem_uart(void);

/* ======================= FUNÇÕES DE TESTE ======================= */

void ler_endereco_i2c(void);

void aht10_init(void);

bool aht10_read(float *temperature, float *humidity);

int calcula_tempo(int tempo, int tipo);

float aht10_get_temperatura(void);

float aht10_get_humidade(void);

bool bh1750_init(void);

bool bh1750_read_raw(uint16_t *raw);

bool bh1750_read_lux(float *lux);

float bh1750_lux_to_percent(float lux);

float bh1750_get_lux_percent();

#endif /* FUNCTIONS_H */
