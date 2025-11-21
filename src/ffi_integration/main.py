import struct
import uuid
from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from .models import BatchIngestionRequest, BatchIngestionResult, EdgeRecord
from .repository import InMemoryRepository
from .audit_client import AuditSinkClient, AuditEventPayload, AuditLevel, AuditEventType

app = FastAPI(title="FFI Integration Service", version="0.1", openapi_url="/ffi/v1/openapi.json")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["POST"],
    allow_headers=["*"],
)

repository = InMemoryRepository()
audit_client = AuditSinkClient()


def xor_checksum(record: EdgeRecord) -> int:
    packed = struct.pack("<qfB", record.timestamp_ms, record.sensor_value, record.sensor_id)
    checksum = 0
    for byte in packed:
        checksum ^= byte
    return checksum


def make_event_payload(event_type: AuditEventType, level: AuditLevel, batch_id: str, correlation_id: str, details: dict):
    return AuditEventPayload(
        event_id=str(uuid.uuid4()),
        source="FFI_INTEGRATION",
        level=level,
        event_type=event_type,
        details={"batch_id": batch_id, **details},
        correlation_id=correlation_id,
    )


@app.post("/ffi/v1/records", response_model=BatchIngestionResult)
def ingest_records(payload: BatchIngestionRequest) -> BatchIngestionResult:
    initial_event = make_event_payload(
        AuditEventType.BATCH_STARTED,
        AuditLevel.INFO,
        payload.batch_id,
        payload.correlation_id,
        {"record_count": len(payload.records)},
    )
    audit_client.log_event(initial_event)

    invalid_count = 0
    for index, record in enumerate(payload.records):
        details = {"record_index": index, "checksum": record.checksum}
        current_event = make_event_payload(
            AuditEventType.RECORD_PERSISTED,
            AuditLevel.SUCCESS,
            payload.batch_id,
            payload.correlation_id,
            details,
        )

        expected_checksum = xor_checksum(record)
        if expected_checksum != record.checksum:
            invalid_count += 1
            error_event = make_event_payload(
                AuditEventType.CHECKSUM_MISMATCH,
                AuditLevel.ERROR,
                payload.batch_id,
                payload.correlation_id,
                {"record_index": index, "expected_checksum": expected_checksum, "observed_checksum": record.checksum},
            )
            audit_client.log_event(error_event)
            raise HTTPException(status_code=422, detail=f"Checksum mismatch on record {index}")

        repository.save_record(record)
        audit_client.log_event(current_event)

    finish_event = make_event_payload(
        AuditEventType.BATCH_COMPLETED,
        AuditLevel.SUCCESS,
        payload.batch_id,
        payload.correlation_id,
        {"processed": len(payload.records) - invalid_count, "invalid": invalid_count},
    )
    audit_client.log_event(finish_event)

    return BatchIngestionResult(
        batch_id=payload.batch_id,
        processed_records=len(payload.records) - invalid_count,
        invalid_records=invalid_count,
    )

