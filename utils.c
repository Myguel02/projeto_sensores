#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool is_valid_date_basic(int dia, int mes, int ano) {
    (void)ano;
    if (mes < 1 || mes > 12 || dia < 1 || dia > 31) return false;
    return true;
}

time_t converter_para_timestamp(int dia, int mes, int ano, int hora, int min, int seg) {
    if (!is_valid_date_basic(dia, mes, ano) || hora < 0 || hora > 23 || min < 0 || min > 59 || seg < 0 || seg > 59) {
        return (time_t)-1;
    }

    struct tm t = {0};
    t.tm_year = ano - 1900;
    t.tm_mon = mes - 1;
    t.tm_mday = dia;
    t.tm_hour = hora;
    t.tm_min = min;
    t.tm_sec = seg;
    t.tm_isdst = -1;

    time_t timestamp = mktime(&t);

    if (timestamp == (time_t)-1) {
    }
    return timestamp;
}

void timestamp_para_string(long long timestamp, char *buffer, size_t buffer_size) {
    time_t rawtime = (time_t)timestamp;
    struct tm *info;

    info = localtime(&rawtime); 
    strftime(buffer, buffer_size, "%d/%m/%Y %H:%M:%S", info);
}