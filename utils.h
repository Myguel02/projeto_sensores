#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <time.h>
#include "common.h"

long long converter_para_timestamp(int dia, int mes, int ano, int hora, int minuto, int segundo);

char* timestamp_para_string(long long timestamp, char* buffer, size_t buffer_size);

data_type string_para_tipo_dado(const char* tipo_str);

#endif 