/**
 * @file dlt_enc_dec.cc
 * @author Devendra Naga (devendra.aaru@outlook.com)
 * @brief implements dlt encoding and decoding
 * @version 0.1
 * @date 2021-12-26
 * 
 * @copyright Copyright (c) 2021-present All rights reserved
 * 
 */
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <auto_lib.h>
#include <dlt_enc_dec.h>

namespace auto_os::middleware {

#define SET_BYTE(__val, __buff, __off) {\
    (__buff[__off]) = (__val);\
    __off ++;\
}

#define SET_BYTES(__val, __len, __buff, __off) {\
    memcpy(__buff + __off, &(__val), __len);\
    __off += __len;\
}

#define COPY_BYTES(__val, __len, __buff, __off) {\
    memcpy(__buff + __off, __val, __len);\
    __off += __len;\
}

#define DLT_MSG_TYPEINFO_STR_VAL_BITS 0x00020000

int dlt_header::encode(uint8_t *payload, uint16_t payload_len, uint8_t *buff, size_t buff_size, size_t &off)
{
    /**
     *
     * standard header:
     * ================
     * 
     * | header type | message type | length  | ecu id  | session id | timestamp |
     * | 1 byte      | 1 byte       | 2 bytes | 4 bytes | 4 bytes    | 4 bytes   |
     * 
     * extended header:
     * ================
     * 
     * | message info | number of args | app id  | context id |
     * | 1 byte       | 1 byte         | 4 bytes | 4 bytes    |
     */

    uint16_t len;

    memset(buff, 0, buff_size);

    SET_BYTE(std_hdr.header_type, buff, off);
    SET_BYTE(std_hdr.msg_counter, buff, off);

    len = auto_os::lib::bswap16b(get_length(payload_len + 1));
    printf("length %d\n", len);
    SET_BYTES(len, 2, buff, off);

    if (std_hdr.has_ecu_id()) {
        COPY_BYTES(std_hdr.ecu_id, 4, buff, off);
    }
    if (std_hdr.has_session_id()) {
        COPY_BYTES(std_hdr.session_id, 4, buff, off);
    }
    if (std_hdr.has_timestamp()) {
        SET_BYTES(std_hdr.timestamp, 4, buff, off);
    }

    // encode ext header
    if (std_hdr.has_ext_hdr()) {
        SET_BYTE(ext_hdr.message_info, buff, off);

        if (ext_hdr.has_verbose()) {
            // set number of args - 1 for now
            SET_BYTE(1, buff, off);
        } else {
            // no of args are 0
            SET_BYTE(0, buff, off);
        }

        COPY_BYTES(ext_hdr.app_id, 4, buff, off);
        COPY_BYTES(ext_hdr.context_id, 4, buff, off);
    }

    uint32_t typeinfo = 0;
    int payload_len_total = 0;

    switch (msg_type_info) {
        case DLT_MSG_TYPEINFO_STRG:
            typeinfo |= DLT_MSG_TYPEINFO_STR_VAL_BITS;
            payload_len_total = payload_len + 1;
        break;
        default:
            return -1;
    }

    typeinfo = auto_os::lib::bswap32b(typeinfo);
    SET_BYTES(typeinfo, 4, buff, off);

    // do not network endian this byte.. DO NOT FIX
    SET_BYTES(payload_len_total, 2, buff, off);

    COPY_BYTES(payload, payload_len, buff, off);
    off ++;

    return off;
}

int dlt_header::decode(uint8_t *payload, uint16_t &payload_len, uint8_t *buff, size_t buff_size, size_t &off)
{
    return -1;
}

}
