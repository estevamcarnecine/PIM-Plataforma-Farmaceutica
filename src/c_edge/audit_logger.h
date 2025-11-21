#ifndef AUDIT_LOGGER_H
#define AUDIT_LOGGER_H

/**
 * Events that the audit logger can persist. Chosen to map to key
 * moments in the C-edge data acquisition flow (start/stop, record writes,
 * write failures, and file access problems) in order to satisfy regulatory
 * audit-trail requirements.
 */
typedef enum {
    AUDIT_EVENT_BATCH_STARTED,
    AUDIT_EVENT_RECORD_PERSISTED,
    AUDIT_EVENT_RECORD_WRITE_FAILURE,
    AUDIT_EVENT_BATCH_COMPLETED,
    AUDIT_EVENT_FILE_ACCESS_ERROR,
    AUDIT_EVENT_SECURE_TRANSMIT,
    AUDIT_EVENT_SECURE_TRANSMIT_FAILURE
} AuditEventType;

/**
 * Logs an audit event with a timestamped ISO 8601 entry. The formatted message
 * should describe the context (record index, file, checksum, error detail,
 * etc.). Returns 0 on success or a negative value if the audit log file could
 * not be opened.
 */
int log_audit_event(AuditEventType event, const char *format, ...);

#endif // AUDIT_LOGGER_H

