#ifndef DATA_ACQUISITION_H
#define DATA_ACQUISITION_H

// Definições e estruturas

// Aqui a precisão de ponto flutuante (float) é essencial, conforme requisito.

typedef struct {
    long long timestamp_ms; // Timestamp de alta precisão
    float sensor_value; 
    unsigned char sensor_id;
    unsigned int checksum;

}