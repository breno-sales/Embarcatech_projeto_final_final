#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#include "functions.h"
#include "FreeRTOSConfig.h"

// #include "FreeRTOS.h"
// #include "task.h"


#define TCP_PORT 500
#define DEBUG_printf printf
#define BUF_SIZE 2048
#define POLL_TIME_S 5

typedef struct TCP_SERVER_T_ {
    struct tcp_pcb *server_pcb;
    struct tcp_pcb *client_pcb;
    bool complete;
    int run_count;
} TCP_SERVER_T;

static TCP_SERVER_T* tcp_server_init(void) {
    TCP_SERVER_T *state = calloc(1, sizeof(TCP_SERVER_T));
    if (!state) {
        DEBUG_printf("failed to allocate state\n");
        return NULL;
    }
    return state;
}

static err_t tcp_server_close(void *arg) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    if (state->client_pcb) {
        tcp_arg(state->client_pcb, NULL);
        tcp_poll(state->client_pcb, NULL, 0);
        tcp_sent(state->client_pcb, NULL);
        tcp_recv(state->client_pcb, NULL);
        tcp_err(state->client_pcb, NULL);
        tcp_close(state->client_pcb);
        state->client_pcb = NULL;
    }
    if (state->server_pcb) {
        tcp_arg(state->server_pcb, NULL);
        tcp_close(state->server_pcb);
        state->server_pcb = NULL;
    }
    return ERR_OK;
}

static err_t tcp_server_result(void *arg, int status) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    DEBUG_printf(status == 0 ? "success\n" : "failed %d\n", status);
    state->complete = true;
    return tcp_server_close(arg);
}

static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        return tcp_server_result(arg, -1);
    }
    cyw43_arch_lwip_check();

    DEBUG_printf("Recebido do cliente (%d bytes):\n", p->tot_len);
    char buffer[BUF_SIZE + 1];
    int len = pbuf_copy_partial(p, buffer, p->tot_len < BUF_SIZE ? p->tot_len : BUF_SIZE, 0);
    buffer[len] = '\0';
    DEBUG_printf("%s\n", buffer);

    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
    return ERR_OK;
}

static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    cyw43_arch_lwip_check();

    const char *msg = "ola mundo\n";
    err_t err = tcp_write(tpcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);
    if (err == ERR_OK) {
        DEBUG_printf("Mensagem enviada: %s", msg);
    } else {
        DEBUG_printf("Erro ao enviar mensagem: %d\n", err);
        return tcp_server_result(arg, -1);
    }
    return ERR_OK;
}

static void tcp_server_err(void *arg, err_t err) {
    if (err != ERR_ABRT) {
        DEBUG_printf("tcp_client_err_fn %d\n", err);
        tcp_server_result(arg, err);
    }
}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    if (err != ERR_OK || client_pcb == NULL) {
        DEBUG_printf("Falha no accept\n");
        tcp_server_result(arg, err);
        return ERR_VAL;
    }

    DEBUG_printf("Cliente conectado\n");
    state->client_pcb = client_pcb;

    tcp_arg(client_pcb, state);
    tcp_recv(client_pcb, tcp_server_recv);
    tcp_poll(client_pcb, tcp_server_poll, POLL_TIME_S);
    tcp_err(client_pcb, tcp_server_err);

    return ERR_OK;
}

static bool tcp_server_open(void *arg) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    DEBUG_printf("Iniciando servidor em %s:%u\n", ip4addr_ntoa(netif_ip4_addr(netif_list)), TCP_PORT);

    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!pcb) {
        DEBUG_printf("falha ao criar pcb\n");
        return false;
    }

    err_t err = tcp_bind(pcb, NULL, TCP_PORT);
    if (err) {
        DEBUG_printf("falha no bind\n");
        return false;
    }

    state->server_pcb = tcp_listen_with_backlog(pcb, 1);
    if (!state->server_pcb) {
        DEBUG_printf("falha no listen\n");
        tcp_close(pcb);
        return false;
    }

    tcp_arg(state->server_pcb, state);
    tcp_accept(state->server_pcb, tcp_server_accept);
    return true;
}

void run_tcp_server_test(void) {
    TCP_SERVER_T *state = tcp_server_init();
    if (!state || !tcp_server_open(state)) {
        tcp_server_result(state, -1);
        return;
    }

    while (!state->complete) {
#if PICO_CYW43_ARCH_POLL
        cyw43_arch_poll();
        cyw43_arch_wait_for_work_until(make_timeout_time_ms(1000));
#else
        sleep_ms(1000);
#endif
    }

    free(state);
}

int main() {
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("falha ao inicializar Wi-Fi\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();

    sleep_ms(5000);
    printf("Conectando ao Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Falha na conexão\n");
        return 1;
    }
    
    printf("Conectado ao Wi-Fi.\n");

    run_tcp_server_test();

    cyw43_arch_deinit();
    return 0;
}