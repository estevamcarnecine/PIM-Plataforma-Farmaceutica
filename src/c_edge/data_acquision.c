// Lógica real das funções, incluindo o cálculo do checksum e a escrita em binário.
#include "data_acquisition.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned int calculate_checksum(const void *data, size_t size) {

    unsigned int checksum = 0;
    const unsigned char *bytes = (const unsigned char *)data;
    for (size_t i = 0; i < size; i++) {
        checksum ^= bytes[i];
    }
    return checksum;
}

int write_records_to_binary(const DataRecord *records, int count, const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        perror("Erro ao abrir arquivo para escrita binária");
        return -1;
    }

    for (int i = 0; i < count; i++) {
        DataRecord record_copy = records[i];

        record_copy.checksum = calculate_checksum(&records[i], sizeof(DataRecord) - sizeof(unsigned int));
        
        size_t written = fwrite(&record_copy, sizeof(DataRecord), 1, fp);

        if (written != 1) {
            perror("Erro na escrita binária do registro");
            fclose(fp);
            return -2;
        }
    }

    fclose(fp);
    return 0;
}