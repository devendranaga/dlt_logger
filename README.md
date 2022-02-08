# DLT_Logger

This repository implements AUTOSAR DLT protocol specification partially. This is only a reference implementation for study purposes.

# What is working

1. Logging via test app to the dlt_service works.
2. dlt_service passes messages to a remote ip and port, that works. wireshark capture is displayed below.
3. string based logging is only implemented. everything else is not implemented.


## configuration

| Configuration item | Description | Min value | Max value | Default value |
|--------------------|-------------|-----------|-----------|---------------|
| htype_use_extended_hdr | use extended header in dlt message | false | true | true |
| htype_msb_first | send msb first in dlt message | false | true | false |
| htype_send_ecu_id | send ecu id in dlt message | false | true | true |
| htype_send_timestamp | send timestamp in dlt message | false | true | true |
| htype_ecu_id | set ecu id string | - | - | ecu1 |
| htype_version | version value | 1 | 1 | 1 |
| ext_hdr_verbose_mode | set verbose mode in header | false | true | true |
| network.socket_type | type of local socket (unix only) | unix | unix | unix |
| network.unix_socket.server_path | type of server socket path |  - | - | /tmp/dlt.sock |
| network.storage_server.server_address | storage server address | - | - | 192.168.1.6 |
| network.storage_server.server_port | storage server port | 1024 | 65535 | 2225 |
| log_to_console | log to console | false | true | true |


