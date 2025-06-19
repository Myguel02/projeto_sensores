#ifndef COMMON_H
#define COMMON_H

#include <time.h>

#define MAX_SENSORS 100
#define MAX_SAMPLES 100000
#define MAX_FILENAME_LENGTH 256
#define MAX_LINE_LENGTH 512
#define MAX_SENSOR_NAME_LENGTH 50
#define MAX_UNIT_LENGTH 10
#define MAX_SENSOR_ID_LENGTH 50

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_UNKNOWN
} data_type;

typedef struct {
    int id;
    char nome[MAX_SENSOR_NAME_LENGTH];
    char unidade[MAX_UNIT_LENGTH];
    data_type tipo_dado;
} Sensor;

typedef struct {
    long long timestamp;
    int sensor_id;
    char valor[MAX_SENSOR_NAME_LENGTH];
} Amostra;

typedef struct {
    char id_sensor[MAX_SENSOR_ID_LENGTH];
    data_type tipo_dado;
} SensorInfo;


#endif 