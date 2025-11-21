from typing import List
from .models import EdgeRecord


class InMemoryRepository:
    """Repository placeholder that captures records in memory for the FFI pipeline."""

    def __init__(self) -> None:
        self._store: List[EdgeRecord] = []

    def save_record(self, record: EdgeRecord) -> None:
        self._store.append(record)

    def list_records(self) -> List[EdgeRecord]:
        return list(self._store)

    def clear(self) -> None:
        self._store.clear()

