#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdbool.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"

#include "variaveis_globais.h"
#include "ssd1306.h"

/* ============================================================
 *                      TASKS (FreeRTOS)
 * ============================================================ */

void task_leitura_serial_receiver(void *pvParameters);
void task_uart_receiver(void *pvParameters);
void task_exibir_infos_OLED(void *pvParameters);
void task_uart_transmitir(void *pvParameters);
void task_ler_sensores(void *pvParameters);

/* ============================================================
 *                  ATUADORES
 * ============================================================ */

bool luz_normal(int pino_GPIO, char *acao);

/* ============================================================
 *                  SENSORES
 * ============================================================ */

float aht10_get_temperatura(void);
float aht10_get_humidade(void);

float bh1750_get_lux_percent(void);

bool  vl53l0x_read_distance_mm(uint16_t *distance_mm);
float vl53l0x_read_distance_mm_media(void);

/* ============================================================
 *              LIMPEZA E POPULAÇÃO DE DADOS
 * ============================================================ */

void limpar_m_json_char(m_json_char *json_c);
void limpar_m_json_int(m_json_int *json_i);

void limpeza_dados_entrada(char *buffer);
void popular_campos(char *buffer);

/* ============================================================
 *          VERIFICAÇÃO DE JSON / PARSING
 * ============================================================ */

int  verificar_porta_acao_especifica(char *buffer, char *comando);
bool verificar_dado_em_json_especifico(char *buffer, char *comando, char *saida);

/* ============================================================
 *          INICIALIZAÇÃO DE PERIFÉRICOS
 * ============================================================ */

void setup_init_i2c0(void);
void setup_init_i2c1(void);

void setup_init_uart_custom(void);

/* ============================================================
 *                  AHT10
 * ============================================================ */

void aht10_init(void);
bool aht10_read(float *temperature, float *humidity);

/* ============================================================
 *                  BH1750
 * ============================================================ */

bool  bh1750_init(void);
bool  bh1750_read_raw(uint16_t *raw);
bool  bh1750_read_lux(float *lux);
float bh1750_lux_to_percent(float lux);

/* ============================================================
 *                  VL53L0X
 * ============================================================ */

bool vl53l0x_init(void);
bool vl53l0x_write_reg(uint8_t reg, uint8_t value);
bool vl53l0x_read_reg(uint8_t reg, uint8_t *value);
bool vl53l0x_read_reg16(uint8_t reg, uint16_t *value);

/* ============================================================
 *          ORQUESTRAMENTO E MENSAGERIA
 * ============================================================ */

void orquestrador_funcionalidades(m_json_char *json_c, m_json_int *json_i);

void montagem_estrutura_de_envio(m_json_char *json_c, m_json_int *json_i);
void desmontar_estrutura_de_envio(m_json_char *json_c, m_json_int *json_i);

void retorno_requisicao_json(const char *origin, m_json_char *json_c);

/* ============================================================
 *                  UART
 * ============================================================ */

void uart_read_exact(uart_inst_t *uart, uint8_t *buf, size_t len);
void receber_mensagem_uart(void);

/* ============================================================
 *                  FUNÇÕES DE TESTE
 * ============================================================ */

void ler_endereco_i2c(void);

#endif /* FUNCTIONS_H */
