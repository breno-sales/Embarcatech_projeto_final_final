#ifndef VARIAVEIS_GLOBAIS_H
#define VARIAVEIS_GLOBAIS_H

/* ============================================================
 *                      INCLUDES
 * ============================================================ */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

/* ============================================================
 *                      DEFINES GERAIS
 * ============================================================ */

#define Max_buffer_size 1024

/* ============================================================
 *                      I2C – PORTAS E ENDEREÇOS
 * ============================================================ */

#define I2C0_PORT            i2c0
#define I2C1_PORT_OLED       i2c1

#define GPIO_SDA             0   // 2
#define GPIO_SCL             1   // 3
#define SDA_OLED             14
#define SCL_OLED             15

#define addr_ldr             0x23
#define addr_distancia       0x29
#define addr_temp_press      0x76
#define addr_humidade_temp   0x38
#define addr_OLED            0x3C

#define BH1750_LUX_DIVISOR    1.2f
#define BH1750_MAX_LUX        65535.0f
#define VL53L0X_TIMEOUT_MS   100

/* ============================================================
 *                      UART
 * ============================================================ */

#define UART_ID          uart1
#define UART_BAUDRATE    115200
#define UART_RX_PIN      8
#define UART_TX_PIN      9
#define UART_TIMEOUT_US  3000

/* ============================================================
 *                      TIPOS DE DADOS
 * ============================================================ */

/* -------- JSON (char) -------- */
typedef struct {
    char resp_remetente[30];
    char resp_destinatario[30];
    char resp_gpio_pino[30];
    char resp_tipo_sensor[30];
    char resp_nome_sensor[30];
    char resp_acao[30];
    char resp_retorno[256];
} m_json_char;

/* -------- JSON (int) -------- */
typedef struct {
    int resp_remetente;
    int resp_destinatario;
    int resp_gpio_pino;
    int resp_tipo_sensor;
    int resp_nome_sensor;
    int resp_acao;
    int resp_retorno;
} m_json_int;

/* -------- Dicionário -------- */
typedef struct {
    char *key;
    int valor;
} dict_chave_valor;

/* ============================================================
 *                  VARIÁVEIS DE CONTROLE
 * ============================================================ */

extern bool exibir_OLED;
extern bool enviar_dados_UART;
extern ssd1306_t disp;

/* ============================================================
 *                  JSON – ESTRUTURAS
 * ============================================================ */

extern m_json_char json_char;
extern m_json_int  json_int;

/* ============================================================
 *                  DICIONÁRIOS
 * ============================================================ */

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

/* ============================================================
 *                  JSON – PARSING
 * ============================================================ */

extern int  idx;
extern int  c;
extern char buffer[Max_buffer_size];

extern char *json_remetente;
extern char *json_destinatario;
extern char *json_gpio_pino;
extern char *json_tipo_sensor;
extern char *json_nome_sensor;
extern char *json_acao;

/* ============================================================
 *                  SENSORES – VARIÁVEIS
 * ============================================================ */

extern float aht10_umidade;
extern float aht10_temperatura;

extern float bh1750_lux;
extern float bh1750_percent;

extern float vl53lox_distancia;

/* ============================================================
 *                      DEBUG
 * ============================================================ */

#define DEBUG_printf printf

#endif /* VARIAVEIS_GLOBAIS_H */
