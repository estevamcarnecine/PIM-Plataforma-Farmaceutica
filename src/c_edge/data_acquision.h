// Definindo a estrutura de dados e o protótipo da função que o módulo C usará.

#ifndef DATA_ACQUISITION_H
#define DATA_ACQUISITION_H

typedef struct {
    long long timestamp_ms; 
    float sensor_value;     
    unsigned char sensor_id; 
    unsigned int checksum;  
} DataRecord;

#define RECORD_BUFFER_SIZE 10

int write_records_to_binary(const DataRecord *records, int count, const char *filename);

#endif