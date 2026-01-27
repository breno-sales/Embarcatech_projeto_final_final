#include <Arduino.h>
#include <WiFi.h>

/* ================= CONFIGURAÇÕES ================= */

#define WIFI_SSID     "Rede_Familia"
#define WIFI_PASSWORD "25021990"

#define TCP_PORT 500
#define BUF_SIZE 2048

/* ================= VARIÁVEIS ================= */

WiFiServer server(TCP_PORT);

/* ================= SETUP ================= */

void setup() {
    Serial.begin(115200);
    delay(10000);

    Serial.println();
    Serial.println("Inicializando ESP32 TCP Server");

    /* -------- Wi-Fi -------- */
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Conectando ao Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWi-Fi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    /* -------- TCP Server -------- */
    server.begin();
    server.setNoDelay(true);

    Serial.print("Servidor TCP iniciado na porta ");
    Serial.println(TCP_PORT);
}

/* ================= LOOP ================= */

void loop() {
    WiFiClient client = server.available(); // verifica conexão

    if (client) {
        Serial.println("Cliente conectado");

        char buffer[BUF_SIZE];

        while (client.connected()) {
            if (client.available()) {
                int len = client.readBytesUntil('\n', buffer, BUF_SIZE - 1);
                buffer[len] = '\0';

                Serial.println("Recebido do cliente:");
                Serial.println(buffer);

                /* Resposta */
                client.print("ola mundo\n");
                Serial.println("Mensagem enviada: ola mundo");
            }

            delay(10);
        }

        client.stop();
        Serial.println("Cliente desconectado");
    }
}
