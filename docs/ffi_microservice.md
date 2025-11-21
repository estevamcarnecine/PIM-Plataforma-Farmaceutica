# FFI Integration Microservice

## Purpose
This Python microservice exposes a secure TLS endpoint for the Edge C module to push records. It verifies each checksum, persists through an in-memory repository (placeholder for the Repository Pattern), and emits audit events via the Audit Log Sink Interface before performing any persistence action.

## Components
- `main.py` – FastAPI application with `/ffi/v1/records` for batch ingestion, checksum validation, and audit logging.
- `audit_client.py` – Helper that posts compliant audit envelopes to the audit sink (`/audit/events`), reading `AUDIT_SINK_URL` and `AUDIT_AUTH_TOKEN` from the environment.
- `repository.py` – In-memory repository abstraction exposing `save_record()`, ready to be replaced with a durable store.
- `models.py` – Pydantic models for the request/response envelope plus record details.

## Build & Run
```sh
cd src/ffi_integration
python -m venv .venv
.venv\\Scripts\\activate
pip install -r requirements.txt
uvicorn ffi_integration.main:app --host 0.0.0.0 --port 8000
```
Set `AUDIT_SINK_URL` and `AUDIT_AUTH_TOKEN` before running so logged events reach the compliance sink.  

## Endpoint Contract
- `POST /ffi/v1/records` – body matches `BatchIngestionRequest` (batch_id, correlation_id, array of `EdgeRecord`).  
- Response is `BatchIngestionResult` (`processed_records` / `invalid_records`).  
- FastAPI auto-docs available at `/ffi/v1/openapi.json`.

## Operational Notes
- The service logs `BATCH_STARTED`, `RECORD_PERSISTED`, and `BATCH_COMPLETED` events through the sink on success; checksum mismatches trigger a `CHECKSUM_MISMATCH` event before raising `422`.
- The in-memory repository simply appends receipts but can later be wired to a file, database, or message queue when the Repository Pattern is formalized.
- The endpoint expects `X-Audit-Auth` (token mirrored from the edge’s `FFI_AUDIT_AUTH_TOKEN`) plus `X-Correlation-ID` so the audit sink can correlate events across services.

Next Step: implement the Audit Log Sink helper library that the Edge C and Python core use to serialize the envelope before delivery; this ensures compliance from every module inbound.

