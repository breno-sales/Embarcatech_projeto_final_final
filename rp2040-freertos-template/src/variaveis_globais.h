#ifndef VARIAVEIS_GLOBAIS_H
#define VARIAVEIS_GLOBAIS_H

#include <stdbool.h>
#include <stdio.h>
#include "hardware/uart.h"
#include "hardware/i2c.h"

#define Max_buffer_size 1024

// ======================= I2C E ENDEREÇOS ===============================================
#define I2C_PORT i2c1
#define GPIO_SDA 2
#define GPIO_SCL 3

#define addr_ldr            0x23
#define addr_distancia      0x29
#define addr_temp_press     0x76
#define addr_humidade_temp  0x38

#define BH1750_LUX_DIVISOR       1.2f
#define BH1750_MAX_LUX           65535.0f


// ======================= UART ===============================================
#define UART_ID uart0
#define UART_BAUDRATE 115200
#define UART_RX_PIN 0  // RECEBEDOR 
#define UART_TX_PIN 1 // TRANSMISSOR  
#define UART_TIMEOUT_US 3000

/* ================= VARIAVEIS DE CONTROLE =================a */

extern bool exibir_OLED;
extern bool enviar_dados_UART;

typedef struct {
    char resp_remetente[30];
    char resp_destinatario[30];
    char resp_gpio_pino[30];
    char resp_tipo_sensor[30];
    char resp_nome_sensor[30];
    char resp_acao[30];
    char resp_retorno[256];

}m_json_char;

extern m_json_char json_char;

typedef struct {
    int resp_remetente;            // MAPEAR OS REMETENTES [ 1 - SERVIDOR ; 2 - MASTER CENTRAL ; 3 - MASTER LOCAL 1 ....]
    int resp_destinatario;         // MAPEAR OS DESTINATARIOS [ 1 - SERVIDOR ; 2 - MASTER CENTRAL ; 3 - MASTER LOCAL 1 ....]
    int resp_gpio_pino;            // PINO SIMPLES
    int resp_tipo_sensor;          // MAPEAR TIPOS DE SENSORES [ 1 - ANALOGICO ; 2 - DIGITAL]
    int resp_nome_sensor;          // MAPEAR NOMES DE SENSORES CONHECIDOS [ 1 - LUZ/LED ; 2 - VENTILADOR ; .... ]
    int resp_acao;                 // MAPEAR AÇÕES ESPERADAS [ 1- LIGAR ; 2- DESLIGAR ; .... ]
    int resp_retorno;              // MAPEAR TIPOS DE RETORNO SIMPLIFICADO [ 1 - SUCESSO ; 2 - FALHA ]

}m_json_int;

extern m_json_int json_int;

typedef struct {
    char *key;
    int valor;
}dict_chave_valor;

extern const dict_chave_valor dict_remetente_destinatario[5];
extern const dict_chave_valor dict_acao[4];
extern const dict_chave_valor dict_tipo_sensor[2];
extern const dict_chave_valor dict_nome_sensor[3];
extern const dict_chave_valor dict_retorno[2];

extern const size_t n_dict_remetente_destinatario;
extern const size_t n_dict_acao;
extern const size_t n_dict_tipo_sensor;
extern const size_t n_dict_nome_sensor;
extern const size_t n_dict_retorno;



/* ================= JSON ================= */
extern int idx;
extern int c;
extern char buffer[Max_buffer_size];

extern char *json_remetente;
extern char *json_destinatario;
extern char *json_gpio_pino;
extern char *json_tipo_sensor;
extern char *json_nome_sensor;
extern char *json_acao;



// ==================== SENSORES ========================

extern float aht10_umidade;
extern float aht10_temperatura;

extern float bh1750_lux;
extern float bh1750_percent;

/* ================= Debug ================= */
#define DEBUG_printf printf

#endif /* VARIAVEIS_GLOBAIS_H */
