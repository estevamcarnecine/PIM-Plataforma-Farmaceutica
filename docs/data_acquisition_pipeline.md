# C Edge Data Acquisition Pipeline

## Overview
The C edge pipeline (`src/c_edge/main.c` + `data_acquisition.c`) simulates sensor data, packs every `DataRecord` into the 17-byte binary layout defined in `binary_format_spec.md`, computes a checksum for the first 13 bytes, writes the record to `records_test.bin`, and timestamps each operation in `audit.log` using the audit logger.

## Building
```sh
gcc src/c_edge/main.c src/c_edge/data_acquisition.c src/c_edge/audit_logger.c -o build/c_data_pipeline
```
This command targets the workspace root. The compiler produces `build/c_data_pipeline`, which bundles the new audit logger and packed records logic.

## Running
```sh
build/c_data_pipeline
```
The application writes 10 sample records to `records_test.bin` (170 bytes total) and appends audit entries to `audit.log`. Use the `AUDIT_LOG_FILE` environment variable to redirect the audit trail to another path if needed.

## Outputs
- `records_test.bin`: Binary payload containing the packed `DataRecord` sequence (timestamp, sensor value, sensor ID, checksum).
- `audit.log`: Line-based audit trail (`ISO 8601 timestamp | event | details`) that records batch start/completion, successful record persistence, and write failures for compliance (aligns with FDA 21 CFR Part 11/GAMP 5 audit trail expectations).

