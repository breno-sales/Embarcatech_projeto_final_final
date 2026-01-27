#include <Arduino.h>
#include <WiFi.h>
#include "functions.h"
#include "variaveis_globais.h"


struct m_json
{
    char resp_remetente[30];
    char resp_destinatario[30];
    char resp_gpio_pino[30];
    char resp_tipo_sensor[30];
    char resp_nome_sensor[30];
    char resp_acao[30];
    char resp_retorno[256];

    int resp_gpio_pino_int;
} modelo_json;


void setup() {
    Serial.begin(115200);
    delay(5000);
    
    iniciando_wifi();

}

/* ================= LOOP ================= */

void loop() {
    execucao_wifi_em_loop();
}
