#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h> // Para llabs
#include <ctype.h> // Para isspace

#include "common.h"

data_type inferir_tipo_dado(const char *valor_str);

bool parse_leitura_from_line(const char *line, leitura *nova_leitura) {
    char *ptr_line = (char *)line;
    char *endptr;

    nova_leitura->timestamp = strtoll(ptr_line, &endptr, 10);
    if (endptr == ptr_line || (*endptr != '\0' && !isspace((unsigned char)*endptr))) {
        return false;
    }
    ptr_line = endptr;

    while (*ptr_line != '\0' && isspace((unsigned char)*ptr_line)) {
        ptr_line++;
    }

    char *id_start = ptr_line;
    while (*ptr_line != '\0' && !isspace((unsigned char)*ptr_line)) {
        ptr_line++;
    }
    size_t id_len = ptr_line - id_start;
    if (id_len == 0 || id_len >= MAX_SENSOR_ID_LENGTH) {
        return false;
    }
    strncpy(nova_leitura->id_sensor, id_start, id_len);
    nova_leitura->id_sensor[id_len] = '\0';

    while (*ptr_line != '\0' && isspace((unsigned char)*ptr_line)) {
        ptr_line++;
    }

    if (*ptr_line == '\0') {
        return false;
    }
    strncpy(nova_leitura->valor_str, ptr_line, MAX_VALUE_STRING_LENGTH - 1);
    nova_leitura->valor_str[MAX_VALUE_STRING_LENGTH - 1] = '\0';
    
    size_t val_len = strlen(nova_leitura->valor_str);
    while (val_len > 0 && isspace((unsigned char)nova_leitura->valor_str[val_len - 1])) {
        nova_leitura->valor_str[val_len - 1] = '\0';
        val_len--;
    }

    nova_leitura->tipo = inferir_tipo_dado(nova_leitura->valor_str);
    
    return true;
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
    size_t len = strlen(valor_str);
    for(size_t i = 0; i < len && i < MAX_VALUE_STRING_LENGTH - 1; i++) {
        lower_val[i] = tolower((unsigned char)valor_str[i]);
    }
    lower_val[len < MAX_VALUE_STRING_LENGTH ? len : MAX_VALUE_STRING_LENGTH - 1] = '\0';

    if (strcmp(lower_val, "true") == 0 || strcmp(lower_val, "false") == 0) return TYPE_BOOL;

    return TYPE_STRING;
}

// Adaptação da busca binária para array ordenado decrescentemente
int buscar_leitura_mais_proxima(leitura *leituras, int num_leituras, long long target_timestamp) {
    if (num_leituras == 0) {
        return -1;
    }

    int low = 0;
    int high = num_leituras - 1;
    int closest_idx = 0; // Inicializa com o primeiro elemento

    // Garante que closest_idx inicial seja válido e atualiza min_diff
    long long min_diff = llabs(leituras[0].timestamp - target_timestamp);

    while (low <= high) {
        int mid = low + (high - low) / 2;
        long long current_timestamp = leituras[mid].timestamp;
        long long current_diff = llabs(current_timestamp - target_timestamp);

        // Se encontrou uma leitura com diferença menor
        if (current_diff < min_diff) {
            min_diff = current_diff;
            closest_idx = mid;
        } else if (current_diff == min_diff) {
            // Se as diferenças são iguais, preferir o timestamp mais recente (maior timestamp)
            // já que a ordem é decrescente, um índice menor significa um timestamp mais recente.
            if (current_timestamp > leituras[closest_idx].timestamp) {
                 closest_idx = mid;
            }
        }

        // Ajustar os limites para busca em array decrescente
        if (current_timestamp == target_timestamp) {
            return mid; // Encontrado exatamente
        } else if (current_timestamp > target_timestamp) {
            low = mid + 1; // O target_timestamp é menor (mais antigo), então procure à direita (timestamps menores)
        } else { // current_timestamp < target_timestamp
            high = mid - 1; // O target_timestamp é maior (mais recente), então procure à esquerda (timestamps maiores)
        }
    }
    
    // Após o loop, 'closest_idx' deve conter o índice do elemento mais próximo encontrado
    return closest_idx;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("uso: %s <nome_do_sensor> \"<data_e_hora_medida_DD/MM/YYYY HH:MM:SS>\"\n", argv[0]);
        printf("exemplo: %s temp \"15/03/2023 10:05:00\"\n", argv[0]);
        return 1;
    }

    const char *nome_sensor = argv[1];
    const char *data_hora_str = argv[2];

    int dia, mes, ano, hora, min, seg;
    if (sscanf(data_hora_str, "%d/%d/%d %d:%d:%d", &dia, &mes, &ano, &hora, &min, &seg) != 6) {
        fprintf(stderr, "erro: formato de data e hora invalido. Use DD/MM/YYYY HH:MM:SS\n");
        return 1;
    }

    long long target_timestamp = (long long)converter_para_timestamp(dia, mes, ano, hora, min, seg);
    if (target_timestamp == (long long)-1) {
        fprintf(stderr, "erro: data e hora fornecida eh invalida ou fora do range de representacao.\n");
        return 1;
    }

    char nome_arquivo_entrada[MAX_SENSOR_ID_LENGTH + 5];
    snprintf(nome_arquivo_entrada, sizeof(nome_arquivo_entrada), "%s.txt", nome_sensor);

    FILE *arquivo_entrada = fopen(nome_arquivo_entrada, "r");
    if (arquivo_entrada == NULL) {
        perror("erro ao abrir o arquivo do sensor");
        fprintf(stderr, "certifique-se de que o programa 1 ja foi executado e gerou '%s'\n", nome_arquivo_entrada);
        return 1;
    }

    leitura *leituras = NULL;
    int contagem = 0;
    int capacidade = 0;
    char linha[MAX_LINE_LENGTH];

    while (fgets(linha, sizeof(linha), arquivo_entrada) != NULL) {
        linha[strcspn(linha, "\n")] = '\0';

        if (contagem == capacidade) {
            capacidade = (capacidade == 0) ? 100 : capacidade * 2;
            leitura *temp_leituras = realloc(leituras, capacidade * sizeof(leitura));
            if (temp_leituras == NULL) {
                perror("erro ao alocar memoria para leituras");
                free(leituras);
                fclose(arquivo_entrada);
                return 1;
            }
            leituras = temp_leituras;
        }

        if (parse_leitura_from_line(linha, &leituras[contagem])) {
            contagem++;
        } else {
            fprintf(stderr, "aviso: linha com formato invalido ignorada no arquivo '%s': '%s'\n", nome_arquivo_entrada, linha);
        }
    }
    fclose(arquivo_entrada);

    if (contagem == 0) {
        printf("nenhuma leitura encontrada para o sensor '%s' no arquivo '%s'.\n", nome_sensor, nome_arquivo_entrada);
        free(leituras);
        return 0;
    }

    int indice_mais_proximo = buscar_leitura_mais_proxima(leituras, contagem, target_timestamp);

    if (indice_mais_proximo != -1) {
        leitura *leitura_encontrada = &leituras[indice_mais_proximo];
        char timestamp_str[50];
        timestamp_para_string(leitura_encontrada->timestamp, timestamp_str, sizeof(timestamp_str));

        printf("\n--- Leitura mais proxima encontrada ---\n");
        printf("Sensor ID: %s\n", leitura_encontrada->id_sensor);
        // Linha 202 refeita com espaço extra
        printf("  Timestamp: %lld (%s)\n", leitura_encontrada->timestamp, timestamp_str); 
        printf("Valor: %s\n", leitura_encontrada->valor_str);
        printf("Tipo Inferido: ");
        switch (leitura_encontrada->tipo) {
            case TYPE_INT: printf("Inteiro\n"); break;
            case TYPE_BOOL: printf("Booleano\n"); break;
            case TYPE_FLOAT: printf("Flutuante\n"); break;
            case TYPE_STRING: printf("String\n"); break;
            default: printf("Desconhecido\n"); break;
        }
        printf("--------------------------------------\n");
    } else {
        printf("Nao foi possivel encontrar uma leitura proxima para o sensor '%s'.\n", nome_sensor);
    }

    free(leituras);

    printf("programa 2 (consulta por instante) concluido.\n");
    return 0;
}