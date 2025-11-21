// Definindo a estrutura de dados e o protótipo da função que o módulo C usará.

#ifndef DATA_ACQUISITION_H
#define DATA_ACQUISITION_H

/* Packed layout required to match the 17-byte binary specification. */
#pragma pack(push, 1)
typedef struct {
    long long timestamp_ms; /* 8 bytes */
    float sensor_value;     /* 4 bytes */
    unsigned char sensor_id;/* 1 byte */
    unsigned int checksum;  /* 4 bytes */
} DataRecord;
#pragma pack(pop)

typedef char DataRecord_size_assert[(sizeof(DataRecord) == 17) ? 1 : -1];

#define RECORD_BUFFER_SIZE 10

unsigned int compute_record_checksum(const DataRecord *record);

int write_records_to_binary(DataRecord *records, int count, const char *filename);

#endif