#ifndef FUNCTIONS_H
#define FUNCTIONS_H

/* ======================= INCLUDES ======================= */
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

/* ======================= TASKS ======================= */

void task_luz_normal(void *pino_GPIO);

void task_leitura_serial_receiver(void *pvParameters);

void task_leitura_uart_receiver(void *pvParameters);

void task_exibir_infos_OLED(void *pvParameters);

/* ======================= ATUADORES ======================= */

bool luz_normal(int pino_GPIO, char *acao);

/* ======================= MASTER / ORQUESTRAÇÃO ======================= */

bool orquestrador_funcionalidades(m_json_char *json_c , m_json_int *json_i);

void limpeza_dados_entrada(char *buffer);

int verificar_porta_acao_especifica(char *buffer,char *comando);

void setup_gpios(void);

void uart_init_custom(void);

bool verificar_dado_em_json_especifico(char *buffer, char *comando, char *saida);

/* ======================= JSON / RETORNO ======================= */

void retorno_requisicao_json(const char *origin, m_json_char *json_c);

void limpar_m_json_char(m_json_char *json_c);

void limpar_m_json_int(m_json_int *json_i);

void montagem_estrutura_de_envio(m_json_char *json_c,m_json_int *json_i);


#endif /* FUNCTIONS_H */
