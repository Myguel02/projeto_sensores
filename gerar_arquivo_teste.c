#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>

#include "common.h"

data_type string_to_data_type(const char* type_str) {
    char lower_type[50];
    size_t len = strlen(type_str);
    for (size_t i = 0; i < len && i < sizeof(lower_type) - 1; i++) {
        lower_type[i] = tolower((unsigned char)type_str[i]);
    }
    lower_type[len < sizeof(lower_type) ? len : sizeof(lower_type) - 1] = '\0';
    if (strcmp(lower_type, "int") == 0) return TYPE_INT;
    if (strcmp(lower_type, "float") == 0) return TYPE_FLOAT;
    if (strcmp(lower_type, "bool") == 0) return TYPE_BOOL;
    if (strcmp(lower_type, "string") == 0) return TYPE_STRING;
    return TYPE_STRING;
}

int main(int argc, char *argv[]) {
    if (argc < 5 || (argc - 3) % 2 != 0) {
        printf("uso: %s \"<data_inicio_DD/MM/YYYY HH:MM:SS>\" \"<data_fim_DD/MM/YYYY HH:MM:SS>\" <sensor1_nome> <sensor1_tipo> [<sensor2_nome> <sensor2_tipo> ...]\n", argv[0]);
        printf("exemplo: %s \"01/01/2023 00:00:00\" \"31/12/2023 23:59:59\" temp int umidade float porta_entrada bool alerta string\n", argv[0]);
        return 1;
    }

    srand((unsigned int)time(NULL));

    const char *start_datetime_str = argv[1];
    const char *end_datetime_str = argv[2];

    int dia, mes, ano, hora, min, seg;
    
    if (sscanf(start_datetime_str, "%d/%d/%d %d:%d:%d", &dia, &mes, &ano, &hora, &min, &seg) != 6) {
        fprintf(stderr, "erro: formato de data e hora de inicio invalido. Use DD/MM/YYYY HH:MM:SS\n");
        return 1;
    }
    long long start_timestamp = (long long)converter_para_timestamp(dia, mes, ano, hora, min, seg);
    if (start_timestamp == (long long)-1) {
        fprintf(stderr, "erro: data e hora de inicio fornecida eh invalida.\n");
        return 1;
    }

    if (sscanf(end_datetime_str, "%d/%d/%d %d:%d:%d", &dia, &mes, &ano, &hora, &min, &seg) != 6) {
        fprintf(stderr, "erro: formato de data e hora de fim invalido. Use DD/MM/YYYY HH:MM:SS\n");
        return 1;
    }
    long long end_timestamp = (long long)converter_para_timestamp(dia, mes, ano, hora, min, seg);
    if (end_timestamp == (long long)-1) {
        fprintf(stderr, "erro: data e hora de fim fornecida eh invalida.\n");
        return 1;
    }

    if (start_timestamp >= end_timestamp) {
        fprintf(stderr, "erro: data e hora de inicio deve ser anterior a data e hora de fim.\n");
        return 1;
    }

    FILE *output_file = fopen("dados_gerados.txt", "w");
    if (output_file == NULL) {
        perror("erro ao criar o arquivo de saida 'dados_gerados.txt'");
        return 1;
    }

    printf("Gerando dados de teste...\n");

    long long total_duration = end_timestamp - start_timestamp;
    const int NUM_LEITURAS_POR_SENSOR = 2000;

    for (int i = 3; i < argc; i += 2) {
        const char *sensor_name = argv[i];
        const char *sensor_type_str = argv[i+1];
        data_type current_data_type = string_to_data_type(sensor_type_str);

        if (strlen(sensor_name) >= MAX_SENSOR_ID_LENGTH) {
            fprintf(stderr, "aviso: ID do sensor '%s' muito longo, sera truncado.\n", sensor_name);
        }

        printf("  Gerando %d leituras para o sensor '%s' (Tipo: %s) em ordem decrescente...\n", NUM_LEITURAS_POR_SENSOR, sensor_name, sensor_type_str);

        long long step_interval = (total_duration / NUM_LEITURAS_POR_SENSOR);
        if (step_interval == 0 && total_duration > 0) {
            step_interval = 1; 
        }

        for (int j = 0; j < NUM_LEITURAS_POR_SENSOR; j++) {
            long long current_timestamp;
            if (NUM_LEITURAS_POR_SENSOR - 1 - j == 0) {
                 current_timestamp = start_timestamp;
            } else {
                 current_timestamp = end_timestamp - (j * step_interval);
                 if (step_interval > 0) {
                     current_timestamp -= (rand() % step_interval);
                 }
                 if (current_timestamp < start_timestamp) {
                     current_timestamp = start_timestamp;
                 }
            }

            char value_buffer[MAX_VALUE_STRING_LENGTH];
            value_buffer[0] = '\0';

            switch (current_data_type) {
                case TYPE_INT:
                    snprintf(value_buffer, sizeof(value_buffer), "%d", rand() % 1000);
                    break;
                case TYPE_FLOAT:
                    snprintf(value_buffer, sizeof(value_buffer), "%.2f", (float)rand() / RAND_MAX * 100.0);
                    break;
                case TYPE_BOOL:
                    snprintf(value_buffer, sizeof(value_buffer), "%s", (rand() % 2 == 0) ? "true" : "false");
                    break;
                case TYPE_STRING:
                    snprintf(value_buffer, sizeof(value_buffer), "valor_%d", rand() % 1000);
                    break;
                default:
                    snprintf(value_buffer, sizeof(value_buffer), "UNKNOWN_VALUE");
                    break;
            }

            fprintf(output_file, "%lld,%s,%s\n", current_timestamp, sensor_name, value_buffer);
        }
    }

    fclose(output_file);
    printf("Arquivo 'dados_gerados.txt' criado com sucesso com timestamps decrescentes!\n");

    return 0;
}