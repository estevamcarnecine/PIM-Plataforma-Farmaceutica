#include "data_acquisition.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define OUTPUT_FILENAME "records_test.bin"

int main() {
    DataRecord test_records[RECORD_BUFFER_SIZE];
    long long current_time = (long long)time(NULL) * 1000;

    printf("Iniciando simulação de %d registros...\n", RECORD_BUFFER_SIZE);

    for (int i = 0; i < RECORD_BUFFER_SIZE; i++) {
        test_records[i].timestamp_ms = current_time + i * 10;
        test_records[i].sensor_value = 25.0f + (float)(i % 5) * 0.1f;
        test_records[i].sensor_id = (unsigned char)(i % 3);
        test_records[i].checksum = 0;
    }
    
    printf("Tentando gravar dados no arquivo: %s\n", OUTPUT_FILENAME);
    
    int result = write_records_to_binary(test_records, RECORD_BUFFER_SIZE, OUTPUT_FILENAME);
    
    if (result == 0) {
        printf("SUCESSO: Arquivo binário '%s' criado com %d registros.\n", OUTPUT_FILENAME, RECORD_BUFFER_SIZE);
        printf("Tamanho esperado do arquivo: %zu bytes.\n", sizeof(DataRecord) * RECORD_BUFFER_SIZE);
        return EXIT_SUCCESS;
    } else {
        printf("FALHA: Erro ao gravar dados. Código de erro: %d\n", result);
        return EXIT_FAILURE;
    }
}