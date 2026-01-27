#include <string.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include <ctype.h>
#include "FreeRTOS.h"
#include "task.h"
#include "variaveis_globais.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "functions.h"

// ------------------ FUNÇÕES TASK  ----------------------------- (completa e testada)

void task_luz_normal(void *pino_GPIO){   

    const int LED_PIN = (int) pino_GPIO; 
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        gpio_put(LED_PIN, !gpio_get(LED_PIN));
        printf("deu certo\n");
    }
}

void task_leitura_serial_receiver(void *pvParameters) {

    static bool resp_orquestrador = false;
    for (;;) {
        idx = 0;
        memset(buffer, 0, sizeof(buffer));

        /* ================= ESPERA INÍCIO ================= */
        while (idx < sizeof(buffer) - 1) {
            c = getchar_timeout_us(100000);

            if (c == PICO_ERROR_TIMEOUT)
                continue;

            if (c == '\r' || c == '\n') {
                buffer[idx] = '\0';
                break;
            }            

            buffer[idx++] = (char)c;

            if (c == '}'){
                buffer[idx] = '\0';
                break;
            }
        }

        /* ================= PROCESSAMENTO ================= */
        if (idx > 0) {

            limpeza_dados_entrada(buffer);

            printf("Comando recebido: %s\n\n", buffer);
            
            verificar_dado_em_json_especifico(buffer, json_remetente, json_char.resp_remetente);
            verificar_dado_em_json_especifico(buffer, json_destinatario, json_char.resp_destinatario);
            verificar_dado_em_json_especifico(buffer, json_tipo_sensor, json_char.resp_tipo_sensor);
            verificar_dado_em_json_especifico(buffer, json_nome_sensor, json_char.resp_nome_sensor);
            verificar_dado_em_json_especifico(buffer, json_acao, json_char.resp_acao);
            verificar_dado_em_json_especifico(buffer, json_gpio_pino, json_char.resp_gpio_pino);
            json_int.resp_gpio_pino = verificar_porta_acao_especifica(buffer, json_gpio_pino);

            retorno_requisicao_json("master_central", &json_char);

            // ----------------------------print das variaveis para acompanhamento------------------
            printf(
                "\n\nJSON_CHAR:\n"
                "remetente: %s\n"
                "destinatario: %s\n"
                "tipo sensor: %s\n"
                "nome sensor: %s\n"
                "acao: %s\n"
                "GPIO: %s\n"
                "Retorno: %s\n",
                json_char.resp_remetente,
                json_char.resp_destinatario,
                json_char.resp_tipo_sensor,
                json_char.resp_nome_sensor,
                json_char.resp_acao,
                json_char.resp_gpio_pino,
                json_char.resp_retorno
            );
            
            montagem_estrutura_de_envio(&json_char, &json_int);

            printf(
                "\n\nJSON_INT:\n"
                "remetente: %i\n"
                "destinatario: %i\n"
                "tipo sensor: %i\n"
                "nome sensor: %i\n"
                "acao: %i\n"
                "GPIO: %i\n"
                "Retorno: %i\n",
                json_int.resp_remetente,
                json_int.resp_destinatario,
                json_int.resp_tipo_sensor,
                json_int.resp_nome_sensor,
                json_int.resp_acao,
                json_int.resp_gpio_pino,
                json_int.resp_retorno
            );

            //-----------------------------------------------------------------------         


            if(strstr(json_char.resp_retorno,"success")){
                resp_orquestrador = orquestrador_funcionalidades(&json_char, &json_int);
                if(resp_orquestrador){
                    // chamar função de enviar retorno
                    printf("\nMensagem de envio de retorno\n");
                } else {
                    // chamar funcao de enviar retorno com erro especifico
                    printf("\nMensagem de envio de retorno com erro especifico\n");
                }
            }
            exibir_OLED = true;
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void task_exibir_infos_OLED(void *pvParameters){
    (void) pvParameters;

    while (1) {
        if (exibir_OLED) {

            ssd1306_t disp;
            disp.external_vcc = false;

            ssd1306_init(&disp, 128, 64, 0x3C, i2c1);
            ssd1306_clear(&disp);

            char remetente[128] = "  > ";
            char nome_sensor[64] = "  > ";
            char tipo_sensor[64] = "  > ";
            char GPIO[64] = "  > ";
            char destinatario[64] = "  > ";
            char acao[64] = "  > ";
            strcat(remetente, json_char.resp_remetente);
            strcat(nome_sensor, json_char.resp_nome_sensor);
            strcat(tipo_sensor, json_char.resp_tipo_sensor);
            strcat(GPIO, json_char.resp_gpio_pino);
            strcat(destinatario, json_char.resp_destinatario);
            strcat(acao, json_char.resp_acao);
            
            ssd1306_draw_string(&disp, 0, 0, 1, "(1/2) Dados recebidos:");
            ssd1306_draw_string(&disp, 0, 16, 1, json_remetente);
            ssd1306_draw_string(&disp, 0, 24, 1, remetente);
            ssd1306_draw_string(&disp, 0, 32, 1, json_destinatario);
            ssd1306_draw_string(&disp, 0, 40, 1, destinatario);
             ssd1306_draw_string(&disp, 0, 48, 1, json_acao);
            ssd1306_draw_string(&disp, 0, 56, 1, acao);

            ssd1306_show(&disp);

            vTaskDelay(pdMS_TO_TICKS(3000));

            ssd1306_clear(&disp);

            ssd1306_draw_string(&disp, 0, 0, 1, "(2/2) Dados recebidos:");
            ssd1306_draw_string(&disp, 0, 16, 1, json_tipo_sensor);
            ssd1306_draw_string(&disp, 0, 24, 1, tipo_sensor);
            ssd1306_draw_string(&disp, 0, 32, 1, json_nome_sensor);
            ssd1306_draw_string(&disp, 0, 40, 1, nome_sensor);
            ssd1306_draw_string(&disp, 0, 48, 1, json_gpio_pino);
            ssd1306_draw_string(&disp, 0, 56, 1, GPIO);

            ssd1306_show(&disp);

            vTaskDelay(pdMS_TO_TICKS(3000));

            ssd1306_clear(&disp);
            ssd1306_show(&disp);

            limpar_m_json_char(&json_char);
            limpar_m_json_int(&json_int);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// ------------------ FUNÇÕES UNICAS DE SENSORES & ATUADORES ----------------------------- (completa e testada)

bool luz_normal(int pino_GPIO, char *acao){

    if(pino_GPIO == 0){ return false;}

    bool status_atual = gpio_get(pino_GPIO);
    
    gpio_init(pino_GPIO);
    gpio_set_dir(pino_GPIO, GPIO_OUT);

    if (strcmp(acao,"ligar") == 0){
        gpio_put(pino_GPIO, 1);  
        return true;  
    } else if (strcmp(acao,"desligar") == 0){
        gpio_put(pino_GPIO, 0);
        return true;
    } else {
        gpio_put(pino_GPIO, status_atual); 
        return false;
    }
}


// ------------------ FUNÇÕES DE LIMPEZA  ------------------------------------------ (completa e testada)

void limpar_m_json_char(m_json_char *json_c)
{
    if (json_c == NULL) {
        return;
    }

    memset(json_c, 0, sizeof(m_json_char));
    exibir_OLED = false;
}

void limpar_m_json_int(m_json_int *json_i)
{
    if (json_i == NULL) {
        return;
    }

    memset(json_i, 0, sizeof(m_json_int));
    exibir_OLED = false;
}

void limpeza_dados_entrada(char *buffer){  // func para limpar os dados de entrada e retornar eles no padrão sem espaço e tudo minusculo
    
    // aqui vai tirar todos os espaços entre as letras
    if (!buffer) return;

    char dest[Max_buffer_size];
    char dest_final[Max_buffer_size];
    int cont = 0;

    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] != ' ') {
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

// ---------------------FUNÇÕES DE VERIFICAÇÃO E INICIALIZAÇÃO ------------------------------------
int verificar_porta_acao_especifica(char *buffer, char *comando){ // func para verificar no JSON de entrada se tem a porta especificada e retornar ela
    
    if (strstr(buffer, comando) != NULL) {
        char *ptr = strstr(buffer, comando);
        int valor = 0;
        ptr += strlen(comando);

        char val[4];
        strncpy(val, ptr, 3);
        val[3] = '\0';   

        for (int i = 0; i < 4; i++){
            if (val[i] == '0'){valor = valor * 10 ;}
            else if (val[i] == '1'){valor = valor * 10 + 10 * 1;}
            else if (val[i] == '2'){valor = valor * 10 + 10 * 2;}
            else if (val[i] == '3'){valor = valor * 10 + 10 * 3;}
            else if (val[i] == '4'){valor = valor * 10 + 10 * 4;}
            else if (val[i] == '5'){valor = valor * 10 + 10 * 5;}
            else if (val[i] == '6'){valor = valor * 10 + 10 * 6;}
            else if (val[i] == '7'){valor = valor * 10 + 10 * 7;}
            else if (val[i] == '8'){valor = valor * 10 + 10 * 8;}
            else if (val[i] == '9'){valor = valor * 10 + 10 * 9;}
        }
            
        return valor/10;
    } else {
        return 0;
    }
}

bool verificar_dado_em_json_especifico(char *buffer, char *comando, char *saida){ // func para retornar automaticamente o valor encontrado para aquela "chave" especificada em comando
    
    if (strstr(buffer, comando) != NULL) {
        char *ptr = strstr(buffer, comando);
        int cont = 0;
        ptr += strlen(comando);
        
        int tamanho_ptr = strlen(ptr);
        
        char val[tamanho_ptr];
        char valor_final[tamanho_ptr];
        strncpy(val, ptr, tamanho_ptr - 1);
        val[tamanho_ptr - 1] = '\0';   

        for (int i = 1; i < tamanho_ptr; i++){ // tem que iniciar do 1 para pular o primeiro aspas
            if (val[i] != ',' && val[i] != '}' && val[i] != '\"' && val[i] != '\''){
                valor_final[cont++] = val[i];
            } else{
                break;
            }   
        }
        valor_final[cont] = '\0';
        

        strcpy(saida, valor_final); 
            
        return true;
    } else {
        strcpy(saida, "none");
        return false;
    }
}

void setup_gpios(void) 
{

    i2c_init(i2c1, 400000);
    gpio_set_function(GPIO_SDA, GPIO_FUNC_I2C);
    gpio_set_function(GPIO_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(GPIO_SDA);
    gpio_pull_up(GPIO_SCL);
}

// ------------------------------ FUNÇÕES DE ORQUESTRAMENTO E MENSAGERIA -----------------------

bool orquestrador_funcionalidades(m_json_char *json_c , m_json_int *json_i){

    static bool retorno = false;

    if(strstr(json_c->resp_nome_sensor, "luz_simples") != NULL || strstr(json_c->resp_nome_sensor, "led") != NULL){
        retorno = luz_normal(json_i->resp_gpio_pino, json_c->resp_acao);      
    } 

    if(retorno){
        printf("\nOrquestrador: tudo certo!\n");
    } else {
        printf("\nOrquestrador: Algo deu errado!\n");
    }
    return retorno;

}

void montagem_estrutura_de_envio(m_json_char *json_c,m_json_int *json_i) { // TRANSFORMAR TODO O STRUCT CHAR EM INT PARA ENVIO EM POUCOS BYTES

    
    if(json_c->resp_acao[0] != '\0'){

        size_t tamanho = sizeof(dict_acao) / sizeof(dict_acao[0]);
        for (size_t i = 0; i < tamanho; i++) {
            if(strcmp(json_c->resp_acao, dict_acao[i].key)){
                json_i->resp_acao = dict_acao[i].valor;
                break;
            }
        }        
    }

    if(json_c->resp_remetente[0] != '\0'){

        size_t tamanho = sizeof(dict_remetente_destinatario) / sizeof(dict_remetente_destinatario[0]);
        for (size_t i = 0; i < tamanho; i++) {
            if(strcmp(json_c->resp_remetente, dict_remetente_destinatario[i].key)){
                json_i->resp_remetente = dict_remetente_destinatario[i].valor;
                break;
            }
        }        
    }

    if(json_c->resp_destinatario[0] != '\0'){

        size_t tamanho = sizeof(dict_remetente_destinatario) / sizeof(dict_remetente_destinatario[0]);
        for (size_t i = 0; i < tamanho; i++) {
            if(strcmp(json_c->resp_destinatario, dict_remetente_destinatario[i].key)){
                json_i->resp_destinatario = dict_remetente_destinatario[i].valor;
                break;
            }
        }        
    }

    if(json_c->resp_tipo_sensor[0] != '\0'){

        size_t tamanho = sizeof(dict_tipo_sensor) / sizeof(dict_tipo_sensor[0]);
        for (size_t i = 0; i < tamanho; i++) {
            if(strcmp(json_c->resp_tipo_sensor, dict_tipo_sensor[i].key)){
                json_i->resp_tipo_sensor = dict_tipo_sensor[i].valor;
                break;
            }
        }        
    }

    if(json_c->resp_nome_sensor[0] != '\0'){

        size_t tamanho = sizeof(dict_nome_sensor) / sizeof(dict_nome_sensor[0]);
        for (size_t i = 0; i < tamanho; i++) {
            if(strcmp(json_c->resp_nome_sensor, dict_nome_sensor[i].key)){
                json_i->resp_nome_sensor = dict_nome_sensor[i].valor;
                break;
            }
        }        
    }

    if(json_c->resp_retorno[0] != '\0'){

        size_t tamanho = sizeof(dict_retorno) / sizeof(dict_retorno[0]);
        for (size_t i = 0; i < tamanho; i++) {
            if(strcmp(json_c->resp_retorno, dict_retorno[i].key)){
                json_i->resp_retorno = dict_retorno[i].valor;
                break;
            }
        }        
    }
}

void desmontar_estrutura_de_envio(m_json_char *json_c,m_json_int *json_i){ // TRANSFORMAR TODO O STRUCT INT EM CHAR PARA REALIZAR ATIVIDADES

    for (size_t i = 0; i < n_dict_acao; i++) {
        if(json_i->resp_acao == dict_acao[i].valor){
            snprintf(
                json_c->resp_acao,
                sizeof(json_c->resp_acao),
                "%s",
                dict_acao[i].key
            );
            break;
        }
    }
    
    for (size_t i = 0; i < n_dict_remetente_destinatario; i++) {
        if(json_i->resp_remetente == dict_remetente_destinatario[i].valor){
            snprintf(
                json_c->resp_remetente,
                sizeof(json_c->resp_remetente),
                "%s",
                dict_remetente_destinatario[i].key
            );
            break;
        }
    }

    for (size_t i = 0; i < n_dict_remetente_destinatario; i++) {
        if(json_i->resp_destinatario == dict_remetente_destinatario[i].valor){
            snprintf(
                json_c->resp_destinatario,
                sizeof(json_c->resp_destinatario),
                "%s",
                dict_remetente_destinatario[i].key
            );
            break;
        }
    }

    for (size_t i = 0; i < n_dict_nome_sensor; i++) {
        if(json_i->resp_nome_sensor == dict_nome_sensor[i].valor){
            snprintf(
                json_c->resp_nome_sensor,
                sizeof(json_c->resp_nome_sensor),
                "%s",
                dict_nome_sensor[i].key
            );
            break;
        }
    }

    for (size_t i = 0; i < n_dict_tipo_sensor; i++) {
        if(json_i->resp_tipo_sensor == dict_tipo_sensor[i].valor){
            snprintf(
                json_c->resp_tipo_sensor,
                sizeof(json_c->resp_tipo_sensor),
                "%s",
                dict_tipo_sensor[i].key
            );
            break;
        }
    }

    for (size_t i = 0; i < n_dict_retorno; i++) {
        if(json_i->resp_retorno == dict_retorno[i].valor){
            snprintf(
                json_c->resp_retorno,
                sizeof(json_c->resp_retorno),
                "%s",
                dict_retorno[i].key
            );
            break;
        }
    }

   snprintf(json_c->resp_gpio_pino, sizeof(json_c->resp_gpio_pino), "%d", json_i->resp_gpio_pino);
    
    
}

void retorno_requisicao_json(const char *origin, m_json_char *json_c){
    
    int status_code = 400;
    const char *resp_text = "erro desconhecido";

    /* =================== VALIDAÇÕES =================== */

    if (strcmp(json_c->resp_remetente, "none") == 0) {
        resp_text = "remetente invalido";
    }
    else if (strcmp(json_c->resp_destinatario, "none") == 0) {
        resp_text = "destinatario invalido";
    }
    else if (strcmp(json_c->resp_tipo_sensor, "none") == 0) {
        resp_text = "tipo de sensor invalido";
    }
    else if (strcmp(json_c->resp_nome_sensor, "none") == 0) {
        resp_text = "nome de sensor invalido";
    }
    else if (strcmp(json_c->resp_acao, "none") == 0) {
        resp_text = "acao invalida";
    }
    else if (strcmp(json_c->resp_gpio_pino, "none") == 0) {
        resp_text = "GPIO invalida";
    }
    else {
        status_code = 200;
        resp_text = "success";
    }

    /* =================== MONTAGEM DO JSON =================== */

    snprintf(
        json_c->resp_retorno,
        sizeof(json_c->resp_retorno),
        "{\"status\":%d,\"origin\":\"%s\",\"text\":\"%s\"}",
        status_code,
        origin,
        resp_text
    );
}



// ------------------ FUNÇÕES EM DESENVOLVIMENTO  ------------------------------------------
