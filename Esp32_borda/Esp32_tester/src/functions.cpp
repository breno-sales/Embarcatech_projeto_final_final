#include <WiFi.h>
#include <stdlib.h>
#include <string.h>
#include <Arduino.h>

#include "variaveis_globais.h"
#include "functions.h"


// ==================================== FUNÇÕES DE UART  =======================================

void uart_send_buffer(const char *buffer) {
    if (!buffer || buffer[0] == '\0') return;

    static char buffer_final[Max_buffer_size];
    static uint16_t len = strlen(buffer);

    // limpa o buffer
    buffer_final[0] = '\0';

    // copia com segurança
    strncpy(buffer_final, buffer, Max_buffer_size - 2);
    buffer_final[Max_buffer_size - 2] = '\0';

    // envia pro console USB
    Serial.write((const uint8_t *)buffer_final, strlen(buffer_final));
    Serial.write('\n');

    // envia via UART
    Serial2.write((uint8_t *)&len, sizeof(len));
    delay(100);
    Serial2.write((uint8_t *)buffer, len);
}

bool uart_received_buffer(char *out_buffer, size_t max_len) {

    static char buffer[256];
    static int cont = 0;

    while (Serial2.available()) {

        char c = Serial2.read();
        Serial.print(c);

        if (cont < sizeof(buffer) - 1 &&
            c != ' ' && c != '\0' && c != '\r' && c != '\n') {
            buffer[cont++] = c;
        }

        if (c == '}') {
            buffer[cont] = '\0';

            limpeza_dados_entrada(buffer);

            strncpy(out_buffer, buffer, max_len - 1);
            out_buffer[max_len - 1] = '\0';

            cont = 0;
            memset(buffer, 0, sizeof(buffer));

            return true;   
        }
    }

    return false;        
}


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
    static bool client_conectado = true;

    if (client) {
        Serial.println("Cliente conectado");

        char buffer[Max_buffer_size];

        while (client.connected()) {
            if (client.available()) {
                int len = client.readBytesUntil('}', buffer, Max_buffer_size - 1);
                buffer[len] = '}';
                buffer[len +1] = '\0';

                Serial.println("\nRecebido do cliente:");
                Serial.println(buffer);

                limpeza_dados_entrada(buffer);

                Serial.printf("\nApos limpeza: %s\n",buffer);

                uart_send_buffer(buffer);

                client.print("\n{\"Status_code\":200,\"Text\":\"Mensagem completa recebida\"\n");
      
            }
            if (uart_received_buffer(msg, sizeof(msg))) {
                Serial.printf("\nMensagem enviada TCP: %s\n", msg);
                client.printf("\n%s\n",msg);

            }

            delay(10);
        }

        client.stop();
        Serial.println("Cliente desconectado");
    }
}

// ==================================== FUNÇÕES DE LIMPEZA =======================================


void limpeza_dados_entrada(char *buffer){  // func para limpar os dados de entrada e retornar eles no padrão sem espaço e tudo minusculo
    
    // aqui vai tirar todos os espaços entre as letras
    if (!buffer) return;

    char dest[Max_buffer_size];
    char dest_final[Max_buffer_size];
    int cont = 0;

    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] != ' ' && buffer[i] != '\r' && buffer[i] != '\n' ) {
            dest[cont++] = buffer[i];
        }
    }
    dest[cont] = '\0';

    // aqui vai transformar tudo para minusculo
    for (int cont2 = 0; cont2 < cont; cont2++){
        dest_final[cont2] = tolower(dest[cont2]);
    }
    dest_final[cont] = '\0';

    strcpy(buffer, dest_final);
}
