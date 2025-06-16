#ifndef COMMON_H
#define COMMON_H

#include <time.h>
#include <stdbool.h>

#define MAX_SENSOR_ID_LENGTH 50
#define MAX_VALUE_STRING_LENGTH 200
#define MAX_LINE_LENGTH 512
#define MAX_SENSORS 100

typedef enum {
    TYPE_INT,
    TYPE_BOOL,
    TYPE_FLOAT,
    TYPE_STRING
} data_type;

typedef struct {
    long long timestamp;
    char id_sensor[MAX_SENSOR_ID_LENGTH];
    char valor_str[MAX_VALUE_STRING_LENGTH];
    data_type tipo;
} leitura;

time_t converter_para_timestamp(int dia, int mes, int ano, int hora, int min, int seg);
void timestamp_para_string(long long timestamp, char *buffer, size_t buffer_size);

#endif