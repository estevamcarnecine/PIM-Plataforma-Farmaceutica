# Edge ↔ FFI Interface

## Goal
Define the binary payload format, TLS settings, and handshake semantics that keep the Edge C module and the FastAPI FFI microservice synchronized on integrity, authentication, and audit requirements (FDA 21 CFR Part 11 / GAMP 5).

## Payload Structure

| Field | Type | Description |
| --- | --- | --- |
| `batch_id` | UUID/v4 string | Unique identifier produced by the edge batch controller. Included verbatim in the audit trail. |
| `correlation_id` | UUID/v4 string | Shared across all services (`EDGE_C`, `FFI_INTEGRATION`, downstream repos) for traceability. |
| `timestamp_ms` | `long long` | Unix epoch milliseconds at the moment of sampling. |
| `sensor_value` | float | IEEE-754 single precision reading. |
| `sensor_id` | unsigned char | Sensor origin (0–255). |
| `checksum` | unsigned int | XOR over `(timestamp_ms + sensor_value + sensor_id)` packed as `<qfB` (Little Endian). Must match the checksum stored in `DataRecord`. |

The JSON envelope posted to `/ffi/v1/records` mirrors `BatchIngestionRequest` from `src/ffi_integration/models.py`, but the C module crafts the individual `EdgeRecord` entries while observing the `checksum` rules above. The FFI service recomputes the checksum and rejects `422` if it differs, logging `CHECKSUM_MISMATCH`.

### Electronic Signature

The edge module attaches a digital signature in the `details` of a single `RecordPersisted` audit event sent after each successful write. This signature is currently modeled as `checksum` + `sensor_id`, and future work will replace it with a PKI-backed HMAC if hardware allows.

## Transport Layer

- **Protocol:** HTTPS/TLS 1.2+ (TLS 1.3 preferred) with mutual authentication when hardware/operating system support is available.  
- **Server:** FastAPI service listens on `/ffi/v1/records` (POST).  
- **Client:** Edge module uses `transmit_secure_payload(data, hash, ca_path, client_cert, client_key)` to POST the JSON payload.  
- Connection must be short-lived (per batch) to align with edge constraints and to keep logs per session.

### Request Headers

- `Content-Type: application/json`
- `X-Correlation-ID: <correlation_id>`
- `X-Audit-Auth`: HMAC/JWT token shared via `FFI_AUDIT_AUTH_TOKEN`, validated before processing and reused when the N-tier audit sink persists the event.
- Optional: `X-Edge-Version`, `X-Device-ID` for future device management/audit use.

### TLS Configuration

- Validate the server certificate chain using the provided CA bundle (`ca_path`).  
- If mutual TLS is enabled, the service validates the client certificate and maps `CommonName` to `sensor_id` ranges for fine-grained audit logging.  
- TLS negotiations must reject renegotiation and use forward secrecy ciphers only.

## Handshake

1. Edge C module initiates TLS connection with `transmit_secure_payload`.  
2. Sends JSON payload with `batch_id`, `correlation_id`, `records`.  
3. FFI service immediately logs `BATCH_STARTED` before checksum validation.  
4. FFI service verifies checksums, logs each `RECORD_PERSISTED` (or `CHECKSUM_MISMATCH` + `422`).  
5. Upon success, `BATCH_COMPLETED` is logged, and repository persistence occurs last.  
6. On failure (checksum/network/audit sink), the edge module receives a non-2xx error, logs locally, and retries per `RetryPolicy` (exponential backoff up to 3 attempts).

## Retry Policy

- Retry only on network/TLS/audit sink unavailability (`5xx`/timeout).  
- Do not retry checksum failures—they are data integrity errors that require human investigation.  
- Each retry increments `retry_attempt` inside audit `details`.

## Audit Flow

- Every successful payload results in audit entries for `BATCH_STARTED`, each `RECORD_PERSISTED`, and `BATCH_COMPLETED`.  
- Failure at any point logs `CHECKSUM_MISMATCH` or `SECURE_TRANSMIT` along with `error_message`.  
- The audit sink keeps the canonical timestamp, but the edge system adds `source_timestamp` for reference.

## Future Enhancements

- Add field-level encryption for `sensor_value` once PKI is in place.  
- Introduce streaming/gRPC variants for ultra-low latency paths.  
- Plug in the repository `publish_new_data_event` so downstream AI services can subscribe directly when batches complete.

