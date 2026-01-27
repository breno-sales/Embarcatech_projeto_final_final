#include <Arduino.h>
#include <WiFi.h>
#include "variaveis_globais.h"
#include <stdlib.h>
#include <string.h>



// ================================= FUNÇOES DE ATUADORES =====================================

bool lampada_simples(int pin, char *acao){
    
    pinMode(pin, OUTPUT);

    if(pin == 0){ return false;}

    bool status_atual = digitalRead(pin);

    if (strcmp(acao,"ligar") == 0){
        digitalWrite(pin, 1);  
        return true;  
    } else if (strcmp(acao,"desligar") == 0){
        digitalWrite(pin, 0);
        return true;
    } else {
        return false;
    }


}

// ==================================== FUNÇÕES DE REDE =======================================

WiFiServer server(TCP_PORT);

void iniciando_wifi(void){
    
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

void execucao_wifi_em_loop(void){

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

                // /* Resposta */
                if(buffer[len -1] == '}'){
                    client.print("\nMensagem completa recebida");
                    Serial.println("Mensagem enviada: Mensagem completa recebida");  
                }
                
            }

            delay(10);
        }

        client.stop();
        Serial.println("Cliente desconectado");
    }
}