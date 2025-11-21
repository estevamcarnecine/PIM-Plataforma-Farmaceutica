from pydantic import BaseModel, Field, conint
from typing import List, Literal


class EdgeRecord(BaseModel):
    batch_id: str = Field(..., description="Identifier for the sensor batch")
    correlation_id: str = Field(..., description="Trace correlation ID across services")
    timestamp_ms: int = Field(..., description="Epoch milliseconds from the edge device")
    sensor_value: float = Field(..., description="IEEE-754 single precision measurement")
    sensor_id: conint(ge=0, le=255) = Field(..., description="Sensor identifier")
    checksum: int = Field(..., description="Integrity hash XORed over the packed fields")


class BatchIngestionRequest(BaseModel):
    batch_id: str = Field(..., description="Logical batch identifier for this payload")
    correlation_id: str = Field(..., description="Shared correlation across modules")
    records: List[EdgeRecord] = Field(..., min_items=1, description="Records supplied by the edge module")


class BatchIngestionResult(BaseModel):
    batch_id: str
    processed_records: int
    invalid_records: int


class RecordDetail(BaseModel):
    record_index: int
    checksum: int

