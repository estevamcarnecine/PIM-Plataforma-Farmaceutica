from dataclasses import dataclass
from enum import Enum
import os
import requests
from typing import Dict, Optional, Any


class AuditLevel(str, Enum):
    INFO = "INFO"
    SUCCESS = "SUCCESS"
    WARNING = "WARNING"
    ERROR = "ERROR"
    CRITICAL = "CRITICAL"


class AuditEventType(str, Enum):
    BATCH_STARTED = "BATCH_STARTED"
    BATCH_COMPLETED = "BATCH_COMPLETED"
    RECORD_PERSISTED = "RECORD_PERSISTED"
    CHECKSUM_MISMATCH = "CHECKSUM_MISMATCH"
    SECURE_TRANSMIT = "SECURE_TRANSMIT"


@dataclass
class AuditEventPayload:
    event_id: str
    source: str
    level: AuditLevel
    event_type: AuditEventType
    details: Dict[str, Any]
    correlation_id: str
    source_timestamp: Optional[str] = None

    def to_dict(self) -> Dict[str, Any]:
        payload: Dict[str, Any] = {
            "event_id": self.event_id,
            "source": self.source,
            "level": self.level.value,
            "event_type": self.event_type.value,
            "details": self.details,
            "correlation_id": self.correlation_id,
        }
        if self.source_timestamp:
            payload["source_timestamp"] = self.source_timestamp
        return payload


class AuditSinkClient:
    """Client for the Audit Log Sink Interface defined in docs."""

    def __init__(self) -> None:
        self._base_url = os.environ.get("AUDIT_SINK_URL", "http://localhost:5001")
        self._auth_token = os.environ.get("AUDIT_AUTH_TOKEN")
        self._headers = {"Content-Type": "application/json"}
        if self._auth_token:
            self._headers["X-Audit-Auth"] = self._auth_token

    def log_event(self, payload: AuditEventPayload) -> bool:
        url = f"{self._base_url.rstrip('/')}/audit/events"
        headers = dict(self._headers)
        cor_id = payload.correlation_id
        headers["X-Correlation-ID"] = cor_id

        try:
            response = requests.post(url, json=payload.to_dict(), headers=headers, timeout=5.0)
            response.raise_for_status()
            return response.json().get("status") == "ACCEPTED"
        except requests.RequestException as exc:
            print(f"[AuditClient] failed to deliver event {payload.event_type}: {exc}")
            return False

    def draft_details(self, record_index: int, checksum: int) -> Dict[str, Any]:
        return {"record_index": record_index, "checksum": checksum}

