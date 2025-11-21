#include "secure_transmit.h"
#include "audit_logger.h"
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_FFI_ENDPOINT "https://127.0.0.1:8000/ffi/v1/records"
#define MAX_HEADER_LEN 256
#define MAX_JSON_BUFFER_BASE 512
#define MAX_RECORD_JSON 128

static int ensure_curl_initialized(void) {
    static int initialized = 0;
    if (initialized) {
        return 0;
    }

    CURLcode code = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (code != CURLE_OK) {
        return -1;
    }

    initialized = 1;
    return 0;
}

static char *build_batch_payload(const char *batch_id,
                                 const char *correlation_id,
                                 const DataRecord *records,
                                 int count) {
    size_t required = MAX_JSON_BUFFER_BASE + (size_t)count * MAX_RECORD_JSON;
    char *payload = (char *)malloc(required);
    if (payload == NULL) {
        return NULL;
    }

    size_t offset = 0;
    int written = snprintf(payload + offset,
                           required - offset,
                           "{\"batch_id\":\"%s\",\"correlation_id\":\"%s\",\"records\":[",
                           batch_id, correlation_id);
    if (written < 0 || (size_t)written >= (required - offset)) {
        free(payload);
        return NULL;
    }

    offset += (size_t)written;

    for (int i = 0; i < count; ++i) {
        const DataRecord *record = &records[i];
        const char *separator = (i == 0) ? "" : ",";
        written = snprintf(payload + offset,
                           required - offset,
                           "%s{\"timestamp_ms\":%lld,\"sensor_value\":%.6f,\"sensor_id\":%u,\"checksum\":%u}",
                           separator,
                           record->timestamp_ms,
                           (double)record->sensor_value,
                           record->sensor_id,
                           record->checksum);
        if (written < 0 || (size_t)written >= (required - offset)) {
            free(payload);
            return NULL;
        }
        offset += (size_t)written;
    }

    written = snprintf(payload + offset, required - offset, "]}");
    if (written < 0 || (size_t)written >= (required - offset)) {
        free(payload);
        return NULL;
    }

    offset += (size_t)written;
    payload[offset] = '\0';
    return payload;
}

int transmit_secure_payload(const char *endpoint_url,
                            DataRecord *records,
                            int record_count,
                            const char *batch_id,
                            const char *correlation_id,
                            const char *ca_bundle_path,
                            const char *client_cert_path,
                            const char *client_key_path) {
    if (record_count <= 0 || records == NULL || batch_id == NULL || correlation_id == NULL) {
        return -1;
    }

    if (ensure_curl_initialized() != 0) {
        return -1;
    }

    const char *url = (endpoint_url && endpoint_url[0]) ? endpoint_url : DEFAULT_FFI_ENDPOINT;
    char *payload = build_batch_payload(batch_id, correlation_id, records, record_count);
    if (payload == NULL) {
        return -1;
    }

    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        free(payload);
        return -1;
    }

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    char correlation_header[MAX_HEADER_LEN];
    snprintf(correlation_header, sizeof(correlation_header), "X-Correlation-ID: %s", correlation_id);
    headers = curl_slist_append(headers, correlation_header);

    const char *audit_token = getenv("FFI_AUDIT_AUTH_TOKEN");
    if (audit_token && audit_token[0]) {
        char auth_header[MAX_HEADER_LEN];
        snprintf(auth_header, sizeof(auth_header), "X-Audit-Auth: %s", audit_token);
        headers = curl_slist_append(headers, auth_header);
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(payload));
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    if (ca_bundle_path && ca_bundle_path[0]) {
        curl_easy_setopt(curl, CURLOPT_CAINFO, ca_bundle_path);
    }
    if (client_cert_path && client_cert_path[0]) {
        curl_easy_setopt(curl, CURLOPT_SSLCERT, client_cert_path);
    }
    if (client_key_path && client_key_path[0]) {
        curl_easy_setopt(curl, CURLOPT_SSLKEY, client_key_path);
    }

    log_audit_event(AUDIT_EVENT_SECURE_TRANSMIT,
                    "operation=secure_transmit url=%s batch=%s record_count=%d",
                    url, batch_id, record_count);

    CURLcode res = curl_easy_perform(curl);
    long http_status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);

    int status = 0;
    if (res != CURLE_OK || http_status < 200 || http_status >= 300) {
        status = -1;
        log_audit_event(AUDIT_EVENT_SECURE_TRANSMIT_FAILURE,
                        "url=%s batch=%s error=%s http_status=%ld",
                        url,
                        batch_id,
                        curl_easy_strerror(res),
                        http_status);
    } else {
        log_audit_event(AUDIT_EVENT_SECURE_TRANSMIT,
                        "url=%s batch=%s http_status=%ld",
                        url,
                        batch_id,
                        http_status);
    }

    free(payload);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return status;
}

