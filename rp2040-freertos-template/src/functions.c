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
#include "hardware/uart.h"

// ------------------ FUNÇÕES TASK  ----------------------------- 

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

            popular_campos(buffer);

            orquestrador_funcionalidades(&json_char, &json_int);

            exibir_OLED = true;
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void task_leitura_uart_receiver(void *pvParameters) {

    for (;;) {

        // só chama quando houver algo na UART
        printf("esta rodando\n");
        if (uart_is_readable(UART_ID)) {
            printf("recebeu algo\n");
            receber_mensagem_uart();
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void task_exibir_infos_OLED(void *pvParameters){
    (void) pvParameters;

    while (1) {
        if (exibir_OLED) {

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

void task_uart_transmitir(void *pvParameters){ // ENVIAR DADOS DE RETORNO DE JSON_CHAR para UART
    
    if(enviar_dados_UART){
        uart_puts(UART_ID,json_char.resp_retorno);
        printf("\nmsg enviada pela UART: %s\n",json_char.resp_retorno);

        enviar_dados_UART = false;
    }
}

void task_ler_sensores(void *pvParameters){
    (void) pvParameters;

    for (;;) {
        aht10_temperatura = aht10_get_temperatura();
        aht10_umidade = aht10_get_humidade();
        bh1750_percent = bh1750_get_lux_percent();
        vl53lox_distancia = vl53l0x_read_distance_mm_media();

        printf("\nLeitura dos sensores:\n"
            "AHT10 = Temperatura: %0.2fºC & Umidade: %0.2f%%\n"
            "BH1750 = Lux: %0.2f & Percentagem lux: %0.2f%%\n"
            "VL53LOX = Distancia media: %fmm (milimetros)\n",
            aht10_temperatura,
            aht10_umidade,
            bh1750_lux,
            bh1750_percent,
            vl53lox_distancia
        );

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}



// ------------------ FUNÇÕES UNICAS DE ATUADORES ----------------------------- (completa e testada)

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


// ------------------ FUNÇÕES UNICAS DE SENSORES ----------------------------- (completa e testada)

float aht10_get_temperatura(){
    static float temp = 0;
    static float hum = 0;
        if (aht10_read(&temp, &hum)) {
            return temp;
        }
    return 0;
}

float aht10_get_humidade(){
    static float temp = 0;
    static float hum = 0;
        if (aht10_read(&temp, &hum)) {
            return hum;
        }
    return 0;
}

float bh1750_get_lux_percent(){

    if (bh1750_read_lux(&bh1750_lux)) {
        bh1750_percent = bh1750_lux_to_percent(bh1750_lux);
        return bh1750_percent;
    } else {
        printf("Erro na leitura do BH1750\n");
    }
    return 0;
}

bool vl53l0x_read_distance_mm(uint16_t *distance_mm){
    
    if (!distance_mm)
        return false;

    // Start single-shot ranging
    if (!vl53l0x_write_reg(0x00, 0x01))
        return false;

    // Tempo REAL de medição (obrigatório)
    sleep_ms(100);

    // Lê distância (RESULT_RANGE_STATUS + 10)
    if (!vl53l0x_read_reg16(0x1E, distance_mm))
        return false;

    // Limpa interrupção interna
    vl53l0x_write_reg(0x0B, 0x01);

    return true;
}

float vl53l0x_read_distance_mm_media(){ // sensor funciona bem em distancias entra 2cm ~ 30cm

    uint32_t sum = 0;
    uint16_t d;
    uint8_t valid = 0;
    float ret = 0;

    for (uint8_t i = 0; i < 5; i++) {
        if (vl53l0x_read_distance_mm(&d)) {
            sum += d;
            valid++;
        }
        sleep_ms(20);
    }

    if (valid == 0)
        return 0;

    ret = sum / valid;
    return ret;
}


// ------------------ FUNÇÕES DE LIMPEZA & populacao ------------------------------------------ (completa e testada)

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

void popular_campos(char* buffer){

    limpeza_dados_entrada(buffer);

    printf("\nComando recebido: %s\n\n", buffer);
    
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

void setup_init_i2c1(void){
    i2c_init(I2C1_PORT_OLED, 400000);
    gpio_set_function(SDA_OLED, GPIO_FUNC_I2C);
    gpio_set_function(SCL_OLED, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_OLED);
    gpio_pull_up(SCL_OLED);

    disp.external_vcc = false;

    ssd1306_init(&disp, 128, 64, addr_OLED, I2C1_PORT_OLED);
    ssd1306_clear(&disp);
    ssd1306_show(&disp);

}

void setup_init_i2c0(void) {

    i2c_init(I2C0_PORT, 400000);
    gpio_set_function(GPIO_SDA, GPIO_FUNC_I2C);
    gpio_set_function(GPIO_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(GPIO_SDA);
    gpio_pull_up(GPIO_SCL);
    
    bh1750_init();
    aht10_init();
    vl53l0x_init();
}

void setup_init_uart_custom(void){
    uart_init(UART_ID, UART_BAUDRATE);

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(UART_ID, true);
}

void aht10_init() {
    uint8_t init_cmd[3] = {0xE1, 0x08, 0x00};
    i2c_write_blocking(I2C0_PORT, addr_humidade_temp, init_cmd, 3, false);
    sleep_ms(20);
}

bool aht10_read(float *temperature, float *humidity) {
    uint8_t trigger_cmd[3] = {0xAC, 0x33, 0x00};
    uint8_t data[6];
    i2c_write_blocking(I2C0_PORT, addr_humidade_temp, trigger_cmd, 3, false);
    sleep_ms(80);
    int ret = i2c_read_blocking(I2C0_PORT, addr_humidade_temp, data, 6, false);
    if (ret != 6 || (data[0] & 0x80)) return false;

    uint32_t raw_humi = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
    uint32_t raw_temp = (((uint32_t)(data[3] & 0x0F)) << 16) | ((uint32_t)data[4] << 8) | data[5];

    *humidity = ((float)raw_humi / 1048576.0f) * 100.0f;
    *temperature = ((float)raw_temp / 1048576.0f) * 200.0f - 50.0f;
    return true;
}

int calcula_tempo(int tempo, int tipo) {
    int h = tempo / 3600;
    int m = (tempo % 3600) / 60;
    int s = tempo % 60;
    if (tipo == 0) return h;
    if (tipo == 1) return m;
    return s;
}

bool bh1750_init(void){

    uint8_t cmd;
    
    // Power ON
    cmd = 0x01;
    if (i2c_write_blocking(I2C0_PORT, addr_ldr, &cmd, 1, false) < 0)
        return false;

    sleep_ms(100);

    // Reset
    cmd = 0x07;
    if (i2c_write_blocking(I2C0_PORT, addr_ldr, &cmd, 1, false) < 0)
        return false;

    sleep_ms(100);

    // Modo contínuo de alta resolução
    cmd = 0x10;
    if (i2c_write_blocking(I2C0_PORT, addr_ldr, &cmd, 1, false) < 0)
        return false;

    sleep_ms(180); // tempo seguro para primeira leitura

    return true;
}

bool bh1750_read_raw(uint16_t *raw){
    uint8_t data[2];

    if (!raw)
        return false;

    int ret = i2c_read_blocking(I2C0_PORT, addr_ldr, data, 2, false);
    if (ret != 2)
        return false;

    *raw = ((uint16_t)data[0] << 8) | data[1];
    return true;
}

bool bh1750_read_lux(float *lux){
    uint16_t raw;

    if (!lux)
        return false;

    if (!bh1750_read_raw(&raw))
        return false;

    *lux = raw / BH1750_LUX_DIVISOR;
    return true;
}

float bh1750_lux_to_percent(float lux){
    if (lux < 0.0f)
        lux = 0.0f;

    if (lux > BH1750_MAX_LUX)
        lux = BH1750_MAX_LUX;

    return (lux / BH1750_MAX_LUX) * 100.0f;
}

bool vl53l0x_write_reg(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};
    return i2c_write_blocking(I2C0_PORT, addr_distancia, buf, 2, false) == 2;
}

bool vl53l0x_read_reg(uint8_t reg, uint8_t *value) {
    if (i2c_write_blocking(I2C0_PORT, addr_distancia, &reg, 1, true) != 1)
        return false;
    return i2c_read_blocking(I2C0_PORT, addr_distancia, value, 1, false) == 1;
}

bool vl53l0x_read_reg16(uint8_t reg, uint16_t *value) {
    uint8_t buf[2];
    if (i2c_write_blocking(I2C0_PORT, addr_distancia, &reg, 1, true) != 1)
        return false;
    if (i2c_read_blocking(I2C0_PORT, addr_distancia, buf, 2, false) != 2)
        return false;

    *value = (buf[0] << 8) | buf[1];
    return true;
}

bool vl53l0x_init(void){
    uint8_t id;

    vl53l0x_write_reg(0x88, 0x00);
    vl53l0x_write_reg(0x80, 0x01);
    vl53l0x_write_reg(0xFF, 0x01);
    vl53l0x_write_reg(0x00, 0x00);
    vl53l0x_write_reg(0x91, 0x3C);
    vl53l0x_write_reg(0x00, 0x01);
    vl53l0x_write_reg(0xFF, 0x00);
    vl53l0x_write_reg(0x80, 0x00);

    vl53l0x_write_reg(0x01, 0xFF);   
    vl53l0x_write_reg(0x04, 0x00);   
    vl53l0x_write_reg(0x05, 0x00);

    sleep_ms(50);
    return true;
}


// ------------------------------ FUNÇÕES DE ORQUESTRAMENTO E MENSAGERIA -----------------------

void orquestrador_funcionalidades(m_json_char *json_c , m_json_int *json_i){

    static bool retorno = false;

    if(strstr(json_char.resp_retorno,"success")){

        if(strstr(json_c->resp_nome_sensor, "luz_simples") != NULL || strstr(json_c->resp_nome_sensor, "led") != NULL){
            retorno = luz_normal(json_i->resp_gpio_pino, json_c->resp_acao);      
        } 

        if(retorno){
            printf("\nOrquestrador: tudo certo!\n");
            printf("\nMensagem de envio de retorno\n");
        } else {
            printf("\nOrquestrador: Algo deu errado!\n");
            printf("\nMensagem de envio de retorno com erro especifico\n");
        }
    }

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


// ------------------ FUNÇÕES UART  ------------------------------------------

void uart_read_exact(uart_inst_t *uart, uint8_t *buf, size_t len) {
    size_t received = 0;

    while (received < len) {
        if (uart_is_readable(uart)) {
            buf[received++] = uart_getc(uart);
            printf("aqui> %c",buf[received-1]);
        } else {
            tight_loop_contents(); // evita busy-wait agressivo
        }
    }
}

void receber_mensagem_uart(void) {

    uint16_t len = 0;

    // 1️⃣ Lê o tamanho (2 bytes)
    uart_read_exact(UART_ID, (uint8_t *)&len, sizeof(len));

    // proteção básica
    if (len == 0 || len >= Max_buffer_size) {
        // printf("Erro: tamanho inválido (%u)\n", len);
        return;
    }

    // 2️⃣ Lê o payload
    static char buffer[Max_buffer_size];
    uart_read_exact(UART_ID, (uint8_t *)buffer, len);

    // transforma em string
    buffer[len] = '\0';

    /* ================= PROCESSAMENTO ================= */
    printf("string completa: %s\n", buffer);

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

    exibir_OLED = true;
}

// ------------------ FUNÇÕES DE TESTE  ------------------------------------------

void ler_endereco_i2c(){

    uint8_t dummy = 0x00;
    
    printf("Buscando dispositivos...\n");

        for (uint8_t addr = 0x08; addr <= 0x77; addr++) {
            int ret = i2c_write_blocking(
                I2C0_PORT,
                addr,
                &dummy,
                1,
                false
            );

            if (ret == 1) {
                printf("✔ Dispositivo I2C0 encontrado em: 0x%02X\n", addr);
            }
        }

        for (uint8_t addr = 0x08; addr <= 0x77; addr++) {
            int ret = i2c_write_blocking(
                I2C1_PORT_OLED,
                addr,
                &dummy,
                1,
                false
            );

            if (ret == 1) {
                printf("✔ Dispositivo I2C1 encontrado em: 0x%02X\n", addr);
            }
        }

        printf("Scan finalizado.\n\n");
        sleep_ms(3000);
}

