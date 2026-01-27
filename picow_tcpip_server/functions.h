#include "string.h"
#include <ctype.h>

// ------------------ FUNÇÕES DE SENSORES & ATUADORES ----------------------------- (completa e testada)

void task_luz_normal(void *pino_GPIO){   

    const int LED_PIN = (int) pino_GPIO; 
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        gpio_put(LED_PIN, !gpio_get(LED_PIN));
        vTaskDelay(1000); // usar em caso de repetições
        printf("deu certo\n");
    }
}

// ==================== FIM FUNÇÕES DE SENSORES & ATUADORES =====================================

// ------------------ FUNÇÕES DE MASTER  ------------------------------------------ (completa e testada)

void limpeza_dados_entrada(char *buffer){  // func para limpar os dados de entrada e retornar eles no padrão sem espaço e tudo minusculo
    
    // aqui vai tirar todos os espaços entre as letras
    if (!buffer) return;

    char dest[128];
    char dest_final[128];
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

void retorno_requisicao_json(char *origin,char *remetente, char *destinatario, char *tipo_sensor, char *nome_sensor, char *acao, int gpio, char *retorno){ // func para retornar o JSON de sucesso/falha com especificações
    
    
    char *resp_status_code = "400";
    char *resp_text;
    
    printf("acao: %s\n",acao);

    if(strcmp(remetente, "none") != 0 && strcmp(destinatario, "none") != 0 && strcmp(tipo_sensor, "none") != 0 && strcmp(nome_sensor, "none") != 0 && strcmp(acao, "none") != 0 && gpio != 0){
       resp_status_code = "200";
       resp_text = "sucess";
    } else if(strcmp(remetente, "none") == 0){
        resp_text = "remetente invalido";
    } else if(strcmp(destinatario, "none") == 0){
        resp_text = "destinatario invalido";
    } else if(strcmp(tipo_sensor, "none") == 0){
        resp_text = "tipo de sensor invalido";
    } else if(strcmp(nome_sensor, "none") == 0){
        resp_text = "nome de sensor invalido";
    } else if(strcmp(acao, "none") == 0){
        resp_text = "acao invalida";
    } else if(gpio == 0){
        resp_text = "GPIO invalido";
    } else {
        resp_text = "erro desconhecido";
        resp_status_code = "404";
    }

    char resp_retorno[256] = "{\"status\":";
    strcat(resp_retorno, resp_status_code);
    strcat(resp_retorno, ", \"origin\": \"");
    strcat(resp_retorno, origin);
    strcat(resp_retorno, "\", \"text\": ");
    strcat(resp_retorno, resp_text);
    strcat(resp_retorno, "}");

    strcpy(retorno, resp_retorno);
}



// ================== FIM FUNÇÕES DE MASTER  ==========================================

static char pending_text[1024] = {0};
static bool has_pending_text = false;



// ------------------ FUNÇÕES EM DESENVOLVIMENTO  ------------------------------------------
