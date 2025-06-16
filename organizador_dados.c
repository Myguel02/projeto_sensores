#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

typedef struct {
    char id_sensor[MAX_SENSOR_ID_LENGTH];
    leitura *leituras;
    int contagem;
    int capacidade;
    data_type tipo_dado;
} sensor_container;

sensor_container sensores_containers[MAX_SENSORS];
int num_sensores_ativos = 0;

int comparar_leituras(const void *a, const void *b) {
    const leitura *leitura_a = (const leitura *)a;
    const leitura *leitura_b = (const leitura *)b;
    if (leitura_a->timestamp < leitura_b->timestamp) return -1;
    if (leitura_a->timestamp > leitura_b->timestamp) return 1;
    return 0;
}

data_type inferir_tipo_dado(const char *valor_str) {
    char *endptr;

    strtol(valor_str, &endptr, 10);
    if (*endptr == '\0' || (*endptr != '\0' && isspace((unsigned char)*endptr) && strlen(endptr) == strspn(endptr, " \t\n\r\f\v"))) {
        return TYPE_INT;
    }

    strtof(valor_str, &endptr);
    if (*endptr == '\0' || (*endptr != '\0' && isspace((unsigned char)*endptr) && strlen(endptr) == strspn(endptr, " \t\n\r\f\v"))) {
        return TYPE_FLOAT;
    }

    char lower_val[MAX_VALUE_STRING_LENGTH];
    int len = strlen(valor_str);
    for(int i = 0; i < len && i < MAX_VALUE_STRING_LENGTH - 1; i++) {
        lower_val[i] = tolower((unsigned char)valor_str[i]);
    }
    lower_val[len < MAX_VALUE_STRING_LENGTH ? len : MAX_VALUE_STRING_LENGTH - 1] = '\0';

    if (strcmp(lower_val, "true") == 0 || strcmp(lower_val, "false") == 0) return TYPE_BOOL;

    return TYPE_STRING;
}

int encontrar_sensor_idx(const char *id_sensor) {
    for (int i = 0; i < num_sensores_ativos; i++) {
        if (strcmp(sensores_containers[i].id_sensor, id_sensor) == 0) {
            return i;
        }
    }
    return -1;
}

int adicionar_novo_sensor(const char *id_sensor, data_type tipo) {
    if (num_sensores_ativos >= MAX_SENSORS) {
        fprintf(stderr, "erro: limite maximo de sensores (%d) atingido.\n", MAX_SENSORS);
        return -1;
    }
    
    strncpy(sensores_containers[num_sensores_ativos].id_sensor, id_sensor, MAX_SENSOR_ID_LENGTH - 1);
    sensores_containers[num_sensores_ativos].id_sensor[MAX_SENSOR_ID_LENGTH - 1] = '\0';
    sensores_containers[num_sensores_ativos].leituras = NULL;
    sensores_containers[num_sensores_ativos].contagem = 0;
    sensores_containers[num_sensores_ativos].capacidade = 0;
    sensores_containers[num_sensores_ativos].tipo_dado = tipo;

    return num_sensores_ativos++;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("uso: %s <arquivo_de_leituras_brutas.txt>\n", argv[0]);
        return 1;
    }

    const char *nome_arquivo_entrada = argv[1];
    FILE *arquivo_entrada = fopen(nome_arquivo_entrada, "r");

    if (arquivo_entrada == NULL) {
        perror("erro ao abrir o arquivo de entrada");
        return 1;
    }

    printf("processando o arquivo: %s...\n", nome_arquivo_entrada);

    char linha[MAX_LINE_LENGTH];
    long long timestamp;
    char id_sensor[MAX_SENSOR_ID_LENGTH];
    char valor_str[MAX_VALUE_STRING_LENGTH];

    while (fgets(linha, sizeof(linha), arquivo_entrada) != NULL) {
        linha[strcspn(linha, "\n")] = '\0';
        
        char *ptr_linha = linha;
        char *end_of_timestamp = NULL;
        char *end_of_id_sensor = NULL;

        // Tenta ler o timestamp
        timestamp = strtoll(ptr_linha, &end_of_timestamp, 10);
        if (end_of_timestamp == ptr_linha || (*end_of_timestamp != '\0' && !isspace((unsigned char)*end_of_timestamp))) {
            fprintf(stderr, "aviso: linha com formato de timestamp invalido: '%s'\n", linha);
            continue;
        }
        ptr_linha = end_of_timestamp;

        // Pula espaços após o timestamp
        while (*ptr_linha != '\0' && isspace((unsigned char)*ptr_linha)) {
            ptr_linha++;
        }

        // Tenta ler o id_sensor
        end_of_id_sensor = ptr_linha;
        while (*end_of_id_sensor != '\0' && !isspace((unsigned char)*end_of_id_sensor)) {
            end_of_id_sensor++;
        }
        if (end_of_id_sensor == ptr_linha) { // id_sensor ausente
            fprintf(stderr, "aviso: linha com id_sensor ausente: '%s'\n", linha);
            continue;
        }
        size_t id_len = end_of_id_sensor - ptr_linha;
        if (id_len >= MAX_SENSOR_ID_LENGTH) {
             id_len = MAX_SENSOR_ID_LENGTH - 1; // Trunca se for muito longo
        }
        strncpy(id_sensor, ptr_linha, id_len);
        id_sensor[id_len] = '\0';
        
        ptr_linha = end_of_id_sensor;

        // Pula espaços após o id_sensor
        while (*ptr_linha != '\0' && isspace((unsigned char)*ptr_linha)) {
            ptr_linha++;
        }

        // O restante da linha é o valor_str
        if (*ptr_linha == '\0') {
            fprintf(stderr, "aviso: linha com valor ausente: '%s'\n", linha);
            continue;
        }
        strncpy(valor_str, ptr_linha, MAX_VALUE_STRING_LENGTH - 1);
        valor_str[MAX_VALUE_STRING_LENGTH - 1] = '\0';
        
        // Remover espaços em branco no final do valor_str
        int val_len = strlen(valor_str);
        while (val_len > 0 && isspace((unsigned char)valor_str[val_len - 1])) {
            valor_str[val_len - 1] = '\0';
            val_len--;
        }

        int idx = encontrar_sensor_idx(id_sensor);
        if (idx == -1) {
            data_type detected_type = inferir_tipo_dado(valor_str);
            idx = adicionar_novo_sensor(id_sensor, detected_type);
            if (idx == -1) {
                continue; 
            }
            printf("sensor '%s' detectado (tipo inferido: %d).\n", id_sensor, detected_type);
        }

        sensor_container *current_sensor_container = &sensores_containers[idx];
        
        if (current_sensor_container->contagem == current_sensor_container->capacidade) {
            current_sensor_container->capacidade = current_sensor_container->capacidade == 0 ? 50 : current_sensor_container->capacidade * 2;
            leitura *temp_leituras = realloc(current_sensor_container->leituras, current_sensor_container->capacidade * sizeof(leitura));
            if (temp_leituras == NULL) {
                perror("erro ao alocar memoria para leituras do sensor");
                fclose(arquivo_entrada);
                for (int i = 0; i < num_sensores_ativos; i++) {
                    free(sensores_containers[i].leituras);
                }
                return 1;
            }
            current_sensor_container->leituras = temp_leituras;
        }
        
        current_sensor_container->leituras[current_sensor_container->contagem].timestamp = timestamp;
        strncpy(current_sensor_container->leituras[current_sensor_container->contagem].id_sensor, id_sensor, MAX_SENSOR_ID_LENGTH - 1);
        current_sensor_container->leituras[current_sensor_container->contagem].id_sensor[MAX_SENSOR_ID_LENGTH - 1] = '\0';
        strncpy(current_sensor_container->leituras[current_sensor_container->contagem].valor_str, valor_str, MAX_VALUE_STRING_LENGTH - 1);
        current_sensor_container->leituras[current_sensor_container->contagem].valor_str[MAX_VALUE_STRING_LENGTH - 1] = '\0';
        current_sensor_container->contagem++;
    }

    fclose(arquivo_entrada);
    printf("leitura do arquivo de entrada concluida.\n");

    for (int i = 0; i < num_sensores_ativos; i++) {
        sensor_container *current_sensor = &sensores_containers[i];
        
        char nome_arquivo_saida[MAX_SENSOR_ID_LENGTH + 5];
        sprintf(nome_arquivo_saida, "%s.txt", current_sensor->id_sensor);

        FILE *arquivo_saida = fopen(nome_arquivo_saida, "w");
        if (arquivo_saida == NULL) {
            perror("erro ao criar arquivo de saida para o sensor");
            continue;
        }

        qsort(current_sensor->leituras, current_sensor->contagem, sizeof(leitura), comparar_leituras);

        for (int j = 0; j < current_sensor->contagem; j++) {
            fprintf(arquivo_saida, "%lld %s %s\n",
                    current_sensor->leituras[j].timestamp,
                    current_sensor->leituras[j].id_sensor,
                    current_sensor->leituras[j].valor_str);
        }
        fclose(arquivo_saida);
        printf("arquivo '%s' gerado com %d leituras e ordenado com sucesso.\n", current_sensor->contagem, nome_arquivo_saida); // Corrigido a ordem dos argumentos aqui
    }

    for (int i = 0; i < num_sensores_ativos; i++) {
        free(sensores_containers[i].leituras);
    }

    printf("programa 1 (organizacao dos dados) concluido.\n");
    return 0;
}