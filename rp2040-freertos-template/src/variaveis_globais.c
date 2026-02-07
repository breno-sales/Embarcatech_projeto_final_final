#include "variaveis_globais.h"

/* ================= VARIAVEIS DE CONTROLE ================= */

bool exibir_OLED = false;
bool enviar_dados_UART = false;

m_json_char json_char;
m_json_int json_int;
const dict_chave_valor dict_remetente_destinatario[5] = {
    {"servidor", 1},
    {"master_central", 2},
    {"master_local", 3},
    {"app", 4},
    {"site", 5}
};

const dict_chave_valor dict_acao[4] = {
    {"ligar", 1},
    {"desligar", 2},
    {"aumentar", 3},
    {"reduzir", 4}
};

const dict_chave_valor dict_tipo_sensor[2] = {
    {"analogico",1},
    {"digital", 2}
};

const dict_chave_valor dict_nome_sensor[3] = {
    {"lampada_simples", 1},
    {"led", 2},
    {"ventilador", 3}
};

const dict_chave_valor dict_retorno[2] = {
    {"success",1},
    {"falha", 2}
};

const size_t n_dict_remetente_destinatario = sizeof(dict_remetente_destinatario) / sizeof(dict_remetente_destinatario[0]);
const size_t n_dict_acao = sizeof(dict_acao) / sizeof(dict_acao[0]);
const size_t n_dict_tipo_sensor = sizeof(dict_tipo_sensor) / sizeof(dict_tipo_sensor[0]);
const size_t n_dict_nome_sensor = sizeof(dict_nome_sensor) / sizeof(dict_nome_sensor[0]);
const size_t n_dict_retorno = sizeof(dict_retorno) / sizeof(dict_retorno[0]);


/* ================= JSON ================= */
int idx = 0;
int c = 0;
char buffer[Max_buffer_size] = "\0";

char *json_remetente     = "\"remetente\":";
char *json_destinatario  = "\"destinatario\":";
char *json_gpio_pino     = "\"gpio\":";
char *json_tipo_sensor   = "\"tipo_sensor\":";
char *json_nome_sensor   = "\"nome_sensor\":";
char *json_acao          = "\"acao\":";


/* ================= OLED ================= */


// ==================== SENSORES ========================

float aht10_umidade = 0;
float aht10_temperatura = 0;

float bh1750_lux = 0;
float bh1750_percent = 0;

float vl53lox_distancia = 0;

