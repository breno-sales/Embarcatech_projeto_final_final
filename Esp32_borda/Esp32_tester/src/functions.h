#include "variaveis_globais.h"
#include <Arduino.h>



bool lampada_simples(int pin, char *acao);

void uart_send_buffer(const char *buffer);

void uart_received_buffer(void);


void iniciando_wifi(void);

void execucao_wifi_em_loop(void);

void limpeza_dados_entrada(char *buffer);