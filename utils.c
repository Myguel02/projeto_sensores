#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"

long long converter_para_timestamp(int dia, int mes, int ano, int hora, int minuto, int segundo) {
    struct tm t = {0};

    t.tm_year = ano - 1900;
    t.tm_mon = mes - 1;
    t.tm_mday = dia;
    t.tm_hour = hora;
    t.tm_min = minuto;
    t.tm_sec = segundo;
    t.tm_isdst = -1;

    time_t timestamp = mktime(&t);

    if (timestamp == (time_t)-1) {
        return -1;
    }

    return (long long)timestamp;
}

char* timestamp_para_string(long long timestamp, char* buffer, size_t buffer_size) {
    time_t rawtime = (time_t)timestamp;
    struct tm *info;

    info = localtime(&rawtime);

    if (info == NULL) {
        snprintf(buffer, buffer_size, "Data Invalida");
        return buffer;
    }

    strftime(buffer, buffer_size, "%d/%m/%Y %H:%M:%S", info);
    return buffer;
}