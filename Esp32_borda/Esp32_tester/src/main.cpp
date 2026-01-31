#include <Arduino.h>
#include <WiFi.h>
#include "functions.h"
#include "variaveis_globais.h"


void setup() {
    Serial.begin(115200);
    Serial2.begin(UART2_BAUD, SERIAL_8N1, UART2_RX, UART2_TX);
    delay(3000);
    
    iniciando_wifi();

}

/* ================= LOOP ================= */

void loop() {
    execucao_wifi_em_loop();
}

