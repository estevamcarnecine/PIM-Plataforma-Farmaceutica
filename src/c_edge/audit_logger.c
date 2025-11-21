#include "audit_logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#define DEFAULT_AUDIT_LOG_FILE "audit.log"
#define AUDIT_MESSAGE_BUFFER_BYTES 512
#define ISO_TIMESTAMP_BYTES 32

static const char *event_type_to_string(AuditEventType event) {
    switch (event) {
        case AUDIT_EVENT_BATCH_STARTED:
            return "BATCH_STARTED";
        case AUDIT_EVENT_RECORD_PERSISTED:
            return "RECORD_PERSISTED";
        case AUDIT_EVENT_RECORD_WRITE_FAILURE:
            return "RECORD_WRITE_FAILURE";
        case AUDIT_EVENT_BATCH_COMPLETED:
            return "BATCH_COMPLETED";
        case AUDIT_EVENT_FILE_ACCESS_ERROR:
            return "FILE_ACCESS_ERROR";
        default:
            return "UNKNOWN_EVENT";
    }
}

static void format_iso8601(char *buffer, size_t size) {
    if (size == 0) {
        return;
    }

    time_t now = time(NULL);
    struct tm snapshot;

#if defined(_MSC_VER)
    gmtime_s(&snapshot, &now);
#else
    struct tm *tm_result = gmtime(&now);
    if (tm_result != NULL) {
        snapshot = *tm_result;
    } else {
        memset(&snapshot, 0, sizeof(snapshot));
    }
#endif

    strftime(buffer, size, "%Y-%m-%dT%H:%M:%SZ", &snapshot);
}

static const char *resolve_log_path(void) {
    const char *env_path = getenv("AUDIT_LOG_FILE");
    return (env_path && env_path[0] != '\0') ? env_path : DEFAULT_AUDIT_LOG_FILE;
}

int log_audit_event(AuditEventType event, const char *format, ...) {
    if (format == NULL) {
        format = "";
    }

    char iso_timestamp[ISO_TIMESTAMP_BYTES] = {0};
    format_iso8601(iso_timestamp, sizeof(iso_timestamp));

    char message[AUDIT_MESSAGE_BUFFER_BYTES];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    const char *log_path = resolve_log_path();
    FILE *log_file = fopen(log_path, "a");
    if (log_file == NULL) {
        return -1;
    }

    fprintf(log_file, "%s | %s | %s\n", iso_timestamp, event_type_to_string(event), message);
    fclose(log_file);
    return 0;
}

