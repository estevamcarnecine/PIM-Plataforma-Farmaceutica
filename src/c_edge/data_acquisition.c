// Lógica real das funções, incluindo o cálculo do checksum e a escrita em binário.
#include "data_acquisition.h"
#include "audit_logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static unsigned int calculate_checksum(const unsigned char *bytes, size_t size) {

    unsigned int checksum = 0;
    for (size_t i = 0; i < size; i++) {
        checksum ^= bytes[i];
    }
    return checksum;
}

unsigned int compute_record_checksum(const DataRecord *record) {
    size_t payload_size = sizeof(DataRecord) - sizeof(record->checksum);
    return calculate_checksum((const unsigned char *)record, payload_size);
}

int write_records_to_binary(DataRecord *records, int count, const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        log_audit_event(AUDIT_EVENT_FILE_ACCESS_ERROR,
                        "operation=open_binary file=%s error=%s",
                        filename, strerror(errno));
        perror("Erro ao abrir arquivo para escrita binária");
        return -1;
    }

    if (log_audit_event(AUDIT_EVENT_BATCH_STARTED,
                        "file=%s record_count=%d", filename, count) != 0) {
        fprintf(stderr, "WARN: auditoria indisponível no início do lote\n");
    }

    for (int i = 0; i < count; i++) {
        DataRecord record_copy = records[i];
        unsigned int checksum = compute_record_checksum(&records[i]);
        record_copy.checksum = checksum;
        records[i].checksum = checksum;

        size_t written = fwrite(&record_copy, sizeof(DataRecord), 1, fp);

        if (written != 1) {
            log_audit_event(AUDIT_EVENT_RECORD_WRITE_FAILURE,
                            "file=%s index=%d sensor=%u checksum=0x%08X",
                            filename, i, record_copy.sensor_id, record_copy.checksum);
            perror("Erro na escrita binária do registro");
            fclose(fp);
            return -2;
        }

        log_audit_event(AUDIT_EVENT_RECORD_PERSISTED,
                        "file=%s index=%d sensor=%u timestamp=%lld checksum=0x%08X",
                        filename,
                        i,
                        record_copy.sensor_id,
                        record_copy.timestamp_ms,
                        record_copy.checksum);
    }

    if (log_audit_event(AUDIT_EVENT_BATCH_COMPLETED,
                        "file=%s record_count=%d bytes=%zu",
                        filename,
                        count,
                        sizeof(DataRecord) * (size_t)count) != 0) {
        fprintf(stderr, "WARN: auditoria indisponível ao finalizar o lote\n");
    }

    fclose(fp);
    return 0;
}