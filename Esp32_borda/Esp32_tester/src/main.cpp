#include <Arduino.h>
#include <WiFi.h>
#include "functions.h"
#include "variaveis_globais.h"

int cont = 0;

void setup() {
    Serial.begin(115200);
    Serial2.begin(UART2_BAUD, SERIAL_8N1, UART2_RX, UART2_TX);
    delay(3000);
    
    iniciando_wifi();
    

}

/* ================= LOOP ================= */

void loop() {
    execucao_wifi_em_loop();
    // Serial.printf("%i - enviado",cont++);
    // Serial2.write("{teste}");
    // uart_send_buffer("enviando teste}");

    // delay(2000);
    
    // if (Serial2.available()) {
    //     char c = Serial2.read();
    //     Serial.print(c);  // eco no USB
    // }

    uart_received_buffer();

    
}

