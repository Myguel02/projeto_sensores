#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>

#include "common.h"
#include "utils.h"

#define NUM_LEITURAS_POR_SENSOR 2000
// #define MAX_SENSORS 10 // Removido, já definido em common.h

data_type string_para_tipo_dado(const char *tipo_str) {
    char lower_str[50];
    size_t len = strlen(tipo_str);
    for (size_t i = 0; i < len && i < sizeof(lower_str) - 1; i++) {
        lower_str[i] = tolower((unsigned char)tipo_str[i]);
    }
    lower_str[len < sizeof(lower_str) ? len : sizeof(lower_str) - 1] = '\0';

    if (strcmp(lower_str, "conj_z") == 0) return TYPE_INT;
    if (strcmp(lower_str, "conj_q") == 0) return TYPE_FLOAT;
    if (strcmp(lower_str, "texto") == 0) return TYPE_STRING;
    if (strcmp(lower_str, "binario") == 0) return TYPE_BOOL;
    return TYPE_UNKNOWN;
}

void gerar_valor_aleatorio(data_type tipo, char *buffer, size_t buffer_size) {
    switch (tipo) {
        case TYPE_INT:
            snprintf(buffer, buffer_size, "%lld", (long long)(rand() % 100000 - 50000));
            break;
        case TYPE_FLOAT:
            snprintf(buffer, buffer_size, "%.2f", (float)(rand() % 100000) / 100.0 - 500.0);
            break;
        case TYPE_STRING: {
            const char *palavras[] = {"medidaA", "dadoX", "valorY", "estadoZ", "temp_alta", "temp_baixa", "ok", "alerta"};
            int num_palavras = sizeof(palavras) / sizeof(palavras[0]);
            snprintf(buffer, buffer_size, "%s", palavras[rand() % num_palavras]);
            break;
        }
        case TYPE_BOOL:
            snprintf(buffer, buffer_size, "%s", (rand() % 2 == 0) ? "true" : "false");
            break;
        default:
            snprintf(buffer, buffer_size, "INVALID_TYPE");
            break;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4 || (argc - 3) % 2 != 0) {
        fprintf(stderr, "Uso: %s \"<data_inicio DD/MM/YYYY HH:MM:SS>\" \"<data_fim DD/MM/YYYY HH:MM:SS>\" <id_sensor1>:<tipo1> [<id_sensor2>:<tipo2> ...]\n", argv[0]);
        fprintf(stderr, "Tipos de dados validos: CONJ_Z (inteiro), CONJ_Q (float), TEXTO (string), BINARIO (booleano).\n");
        return 1;
    }

    int d_ini, m_ini, a_ini, h_ini, min_ini, s_ini;
    int d_fim, m_fim, a_fim, h_fim, min_fim, s_fim;

    if (sscanf(argv[1], "%d/%d/%d %d:%d:%d", &d_ini, &m_ini, &a_ini, &h_ini, &min_ini, &s_ini) != 6) {
        fprintf(stderr, "Erro: Formato da data e hora de inicio invalido. Use DD/MM/YYYY HH:MM:SS\n");
        return 1;
    }
    if (sscanf(argv[2], "%d/%d/%d %d:%d:%d", &d_fim, &m_fim, &a_fim, &h_fim, &min_fim, &s_fim) != 6) {
        fprintf(stderr, "Erro: Formato da data e hora de fim invalido. Use DD/MM/YYYY HH:MM:SS\n");
        return 1;
    }

    long long timestamp_inicio = converter_para_timestamp(d_ini, m_ini, a_ini, h_ini, min_ini, s_ini);
    long long timestamp_fim = converter_para_timestamp(d_fim, m_fim, a_fim, h_fim, min_fim, s_fim);

    if (timestamp_inicio == -1 || timestamp_fim == -1) {
        fprintf(stderr, "Erro: Uma das datas/horas fornecidas eh invalida ou fora do range de representacao.\n");
        return 1;
    }
    if (timestamp_inicio >= timestamp_fim) {
        fprintf(stderr, "Erro: A data/hora de inicio deve ser anterior a data/hora de fim.\n");
        return 1;
    }

    SensorInfo sensores[MAX_SENSORS]; // MAX_SENSORS para o array
    int num_sensores = 0;

    for (int i = 3; i < argc; i++) {
        if (num_sensores >= MAX_SENSORS) { // MAX_SENSORS é 100 de common.h
            fprintf(stderr, "Aviso: Limite de %d sensores atingido. Ignorando sensores adicionais.\n", MAX_SENSORS);
            break;
        }

        char *arg = argv[i];
        char *colon_pos = strchr(arg, ':');
        if (!colon_pos) {
            fprintf(stderr, "Erro: Formato invalido para sensor. Use <id_sensor>:<tipo_dado>. Argumento: '%s'\n", arg);
            return 1;
        }

        size_t id_len = colon_pos - arg;
        if (id_len == 0 || id_len >= MAX_SENSOR_ID_LENGTH) {
            fprintf(stderr, "Erro: ID do sensor muito longo ou vazio. Maximo %d caracteres. Argumento: '%s'\n", MAX_SENSOR_ID_LENGTH - 1, arg);
            return 1;
        }
        strncpy(sensores[num_sensores].id_sensor, arg, id_len);
        sensores[num_sensores].id_sensor[id_len] = '\0';

        const char *tipo_str = colon_pos + 1;
        sensores[num_sensores].tipo_dado = string_para_tipo_dado(tipo_str);

        if (sensores[num_sensores].tipo_dado == TYPE_UNKNOWN) {
            fprintf(stderr, "Erro: Tipo de dado invalido para o sensor '%s'. Tipo fornecido: '%s'\n", sensores[num_sensores].id_sensor, tipo_str);
            fprintf(stderr, "Tipos de dados validos: CONJ_Z, CONJ_Q, TEXTO, BINARIO.\n");
            return 1;
        }
        num_sensores++;
    }

    if (num_sensores == 0) {
        fprintf(stderr, "Erro: Nenhuma informacao de sensor valida foi fornecida.\n");
        return 1;
    }

    FILE *f_out = fopen("dados_brutos.txt", "w");
    if (!f_out) {
        perror("Erro ao abrir 'dados_brutos.txt' para escrita");
        return 1;
    }

    fprintf(f_out, "id_sensor;timestamp;valor\n");

    srand((unsigned int)time(NULL)); // Inicializa o gerador de números aleatórios uma vez no main

    long long current_timestamp = timestamp_inicio;
    while (current_timestamp <= timestamp_fim) {
        for (int i = 0; i < num_sensores; i++) {
            char valor_str[MAX_SENSOR_ID_LENGTH]; // Ajustado o tamanho do buffer para o valor
            gerar_valor_aleatorio(sensores[i].tipo_dado, valor_str, sizeof(valor_str));
            fprintf(f_out, "%s;%lld;%s\n", sensores[i].id_sensor, current_timestamp, valor_str);
        }
        current_timestamp++; // Incrementa o timestamp para a próxima "leitura"
    }

    fclose(f_out);
    printf("Arquivo 'dados_brutos.txt' gerado com sucesso.\n");

    return 0;
}