#include "data_acquisition.h"
#include "secure_transmit.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define OUTPUT_FILENAME "records_test.bin"
#define ENV_BATCH_ID "EDGE_BATCH_ID"
#define ENV_CORRELATION_ID "EDGE_CORRELATION_ID"
#define ENV_FFI_ENDPOINT "FFI_ENDPOINT_URL"
#define ENV_CA_BUNDLE "FFI_CA_BUNDLE"
#define ENV_CLIENT_CERT "FFI_CLIENT_CERT"
#define ENV_CLIENT_KEY "FFI_CLIENT_KEY"

static void random_bytes(unsigned char *buffer, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        buffer[i] = (unsigned char)(rand() & 0xFF);
    }
}

static void format_uuid_v4(char out[37]) {
    const char *template = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
    const char *hex_digits = "0123456789abcdef";

    unsigned char randomness[16];
    random_bytes(randomness, sizeof(randomness));

    size_t rnd_index = 0;
    for (size_t i = 0; i < 36; ++i) {
        char template_char = template[i];
        if (template_char == '-') {
            out[i] = '-';
            continue;
        }

        unsigned value = randomness[rnd_index >> 1];
        if ((rnd_index & 1) == 0) {
            value >>= 4;
        } else {
            value &= 0x0F;
        }
        ++rnd_index;

        if (template_char == 'y') {
            value = (value & 0x3) | 0x8;
        } else if (template_char == '4') {
            value = 4;
        }

        out[i] = hex_digits[value];
    }

    out[36] = '\0';
}

static void maybe_write_uuid(const char *env_value, char *out_uuid) {
    if (env_value && env_value[0] != '\0' && strlen(env_value) < 37) {
        strcpy(out_uuid, env_value);
    } else {
        format_uuid_v4(out_uuid);
    }
}

int main() {
    srand((unsigned)time(NULL));

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

    if (result != 0) {
        printf("FALHA: Erro ao gravar dados. Código de erro: %d\n", result);
        return EXIT_FAILURE;
    }

    printf("SUCESSO: Arquivo binário '%s' criado com %d registros.\n", OUTPUT_FILENAME, RECORD_BUFFER_SIZE);
    printf("Tamanho esperado do arquivo: %zu bytes.\n", sizeof(DataRecord) * RECORD_BUFFER_SIZE);

    char batch_id[37];
    char correlation_id[37];
    maybe_write_uuid(getenv(ENV_BATCH_ID), batch_id);
    maybe_write_uuid(getenv(ENV_CORRELATION_ID), correlation_id);

    const char *endpoint = getenv(ENV_FFI_ENDPOINT);
    const char *ca_bundle = getenv(ENV_CA_BUNDLE);
    const char *client_cert = getenv(ENV_CLIENT_CERT);
    const char *client_key = getenv(ENV_CLIENT_KEY);

    int transmit_result = transmit_secure_payload(endpoint,
                                                  test_records,
                                                  RECORD_BUFFER_SIZE,
                                                  batch_id,
                                                  correlation_id,
                                                  ca_bundle,
                                                  client_cert,
                                                  client_key);

    if (transmit_result != 0) {
        printf("FALHA: não foi possível transmitir os registros de forma segura (código %d).\n", transmit_result);
        return EXIT_FAILURE;
    }

    printf("Transmissão segura bem-sucedida para %s (batch=%s).\n", endpoint ? endpoint : "endpoint padrão", batch_id);
    return EXIT_SUCCESS;
}