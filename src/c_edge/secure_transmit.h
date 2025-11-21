#ifndef SECURE_TRANSMIT_H
#define SECURE_TRANSMIT_H

#include "data_acquisition.h"

int transmit_secure_payload(const char *endpoint_url,
                            DataRecord *records,
                            int record_count,
                            const char *batch_id,
                            const char *correlation_id,
                            const char *ca_bundle_path,
                            const char *client_cert_path,
                            const char *client_key_path);

#endif // SECURE_TRANSMIT_H

