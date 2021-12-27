/**
 * @file dlt_enc_dec.h
 * @author Devendra Naga (devendra.aaru@outlook.com)
 * @brief implements dlt encode and decode
 * @version 0.1
 * @date 2021-12-27
 * 
 * @copyright Copyright (c) 2021-present All rights reserved
 * 
 */
#ifndef __AUTO_OS_MIDDLEWARE_DLT_ENCDEC_H__
#define __AUTO_OS_MIDDLEWARE_DLT_ENCDEC_H__

#include <dlt_msg_if.h>

namespace auto_os::middleware {

#define DLT_HDR_TYPE_USE_EXT_HEADER     0x01
#define DLT_HDR_TYPE_MSB_FIRST          0x02
#define DLT_HDR_TYPE_WITH_ECU_ID        0x04
#define DLT_HDR_TYPE_WITH_SESSION_ID    0x08
#define DLT_HDR_TYPE_WITH_TIMESTAMP     0x10

#define SET_4_BYTES(__left, __right) {\
    __left[0] = __right[0];\
    __left[1] = __right[1];\
    __left[2] = __right[2];\
    __left[3] = __right[3];\
}

struct dlt_standard_header {
    /**
     * | UEH | MSBF | WEID | WTMS | VERS      |
     * | 0   | 1    | 2    | 3    | 5 | 6 | 7 |
     * 
     * UEH - use extended header
     * 1 - if extended header
     * 0 - if extended header is not used
     * 
     * MSBF - msb first
     * 
     * 1 - big endian format
     * 0 - little endian format
     * 
     * WEID - with extended id
     * 
     * 1 - the ecu id will be in standard header
     * 0 - the ecu id will not be contained in standard header
     * 
     * WSID - with session id
     * 
     * 1 - the session id field is contained in standard header
     * 0 - the session id field is not contained in standard header
     * 
     * WTMS - with timestamp
     * 
     * 1 - the timestamp is contained in standard header
     * 0 - the timestamp is not contained in standard header
     */
    uint8_t header_type;

    /**
     * 8 bit integer 0 - 255
     */
    uint8_t msg_counter;

    /**
     * 16 bit integer
     * 
     * sum of length of standard header +
     *        length of optional dlt extended header +
     *        length of the optional payload
     */
    uint16_t length;

    /**
     * ⌈If the ECU ID is shorter than four 8-bit ASCII characters, the 
     * remaining characters shall be filled with 0x00. ⌋
     */
    uint8_t ecu_id[4];

    /**
     *  Session ID is used to identify the source of a log or trace message
     */
    uint8_t session_id[4];
    /**
     * The time resolution is in 0.1 milliseconds
     */
    uint32_t timestamp;

    dlt_standard_header()
    {
        set_defaults();
    }

    ~dlt_standard_header() { }

    /**
     * @brief Set the defaults object
     */
    inline void set_defaults()
    {
        header_type = 0;
        msg_counter = 0;
        length = 0;
        ecu_id[0] = ecu_id[1] = ecu_id[2] = ecu_id[3] = 0;
        session_id[0] = session_id[1] = session_id[2] = session_id[3] = 0;
        timestamp = 0;
    }

    /**
     * @brief Set the use ext hdr object
     */
    inline void set_use_ext_hdr() { header_type |= DLT_HDR_TYPE_USE_EXT_HEADER; }

    /**
     * @brief Set the msb first object
     */
    inline void set_msb_first() { header_type |= DLT_HDR_TYPE_MSB_FIRST; }

    /**
     * @brief Set the valid ecu id object
     */
    inline void set_valid_ecu_id() { header_type |= DLT_HDR_TYPE_WITH_ECU_ID; }

    /**
     * @brief Set the valid session id object
     */
    inline void set_valid_session_id() { header_type |= DLT_HDR_TYPE_WITH_SESSION_ID; }

    /**
     * @brief Set the valid timestamp object
     */
    inline void set_valid_timestamp() { header_type |= DLT_HDR_TYPE_WITH_TIMESTAMP; }

    /**
     * @brief Set the version object
     * 
     * @param in version value of version
     */
    inline void set_version(int version) { header_type |= (version << 5); }

    /**
     * @brief Set the msg counter object
     * 
     * @param in msg_count value of msg_count
     */
    inline void set_msg_counter(int msg_count) { msg_counter = msg_count; }

    /**
     * @brief Set the ecu id object
     * 
     * @param in ecuid value of ecuid
     */
    inline void set_ecu_id(const std::string ecuid) { SET_4_BYTES(ecu_id, ecuid); }

    /**
     * @brief Set the session id object
     * 
     * @param in sess_id value of session_id
     */
    inline void set_session_id(uint8_t *sess_id) { SET_4_BYTES(session_id, sess_id); }

    /**
     * @brief check if ecu_id is set
     * 
     * @return true if ecu id is set
     * @return false if ecu id is not set
     */
    inline bool has_ecu_id() { return !!(header_type & DLT_HDR_TYPE_WITH_ECU_ID); }

    /**
     * @brief check if session_id is set
     * 
     * @return true if session id is set
     * @return false 
     */
    inline bool has_session_id() { return !!(header_type & DLT_HDR_TYPE_WITH_SESSION_ID); }
    inline bool has_timestamp() { return !!(header_type & DLT_HDR_TYPE_WITH_TIMESTAMP); }
    inline bool has_ext_hdr() { return !!(header_type & DLT_HDR_TYPE_USE_EXT_HEADER); }
};

enum class dlt_extended_header_msg_type {
    eDLT_TYPE_LOG = 0x0, // DLT log message
    eDLT_TYPE_APP_TRACE, // DLT trace message
    eDLT_TYPE_NW_TRACE, // DLT network message
    eDLT_TYPE_CONTROL, // DLT control message
};

enum class dlt_extended_header_msg_type_info_log {
    eDLT_LOG_FATAL = 0x1, // Fatal system error
    eDLT_LOG_ERROR, // SWC error
    eDLT_LOG_WARN, // Correct behavior cannot be ensured
    eDLT_LOG_INFO, // Message of LogLevel type “Information
    eDLT_LOG_DEBUG, // Message of LogLevel type “Debug
    eDLT_LOG_VERBOSE, // Message of LogLevel type “Verbose”
};

enum class dlt_extended_header_msg_type_info_trace {
    eDLT_TRACE_VARIABLE = 0x1, // value of variable
    eDLT_TRACE_FUNCTION_IN, // Call of a function
    eDLT_TRACE_FUNCTION_OUT, // Return of a function
    eDLT_TRACE_STATE, // State of a State Machine
    eDLT_TRACE_VFB, // RTE events
};

enum class dlt_extended_header_msg_type_info_nw {
    eDLT_NW_TRACE_IPC, // Inter-Process-Communication
    eDLT_NW_TRACE_CAN, // CAN Communications bus
    eDLT_NW_TRACE_FLEXRAY, // FlexRay Communications bus
    eDLT_NW_TRACE_MOST, // Most Communications bus
    eDLT_NW_TRACE_ETHERNET, // Ethernet Communications bus
    eDLT_NW_TRACE_SOMEIP, // SOME/IP Communication
};

enum class dlt_extended_header_msg_type_info_ctrl {
    eDLT_CONTROL_REQUEST, // Request Control Message
    eDLT_CONTROL_RESPONSE, // Respond Control Message
};

struct dlt_extended_header {
    /**
     * | VERB | MSTP  | MTIN    |
     * | 0    | 1 2 3 | 4 5 6 7 |
     *
     * VERB - verbose mode
     * 
     * 1 - payload shall be transmitted in verbose mode
     * 0 - payload shall be transmitted in non verbose mode
     * 
     * MSTP - message type
     * 
     * type dlt_extended_header_msg_type
     * 
     * MTIN - messge type info
     * 
     * if eDLT_TYPE_LOG -> dlt_extended_header_msg_type_info_log
     * if eDLT_TYPE_APP_TRACE -> dlt_extended_header_msg_type_info_trace
     * if eDLT_TYPE_NW_TRACE -> dlt_extended_header_msg_type_info_nw
     * if eDLT_TYPE_CONTROL -> dlt_extended_header_msg_type_info_ctrl
     */
    uint8_t message_info;

    /**
     * Number of Arguments represents the number of consecutive parameters in the 
     * payload segment of one Dlt message. 
     */
    uint8_t number_of_args;

    /**
     * The Application ID field (APID) shall be a 32-bit field interpreted 
     * as four 8-bit ASCII characters.
     */
    uint8_t app_id[4];

    /**
     * user defined ID to logically group DLT messages generated by an application
     */
    uint8_t context_id[4];

    dlt_extended_header()
    {
        set_defaults();
    }

    ~dlt_extended_header() { }

    inline void set_defaults()
    {
        message_info = 0;
        number_of_args = 0;
        app_id[0] = app_id[1] = app_id[2] = app_id[3] = 0;
        context_id[0] = context_id[1] = context_id[2] = context_id[3] = 0;
    }

#define DLT_EXT_HDR_MSG_INFO_VERBOSE 0x01

    inline void set_verbose()
    {
        message_info |= DLT_EXT_HDR_MSG_INFO_VERBOSE;
    }

    inline void set_msg_type(dlt_extended_header_msg_type msg_type_val)
    {
        int msg_type = static_cast<int>(msg_type_val);

        message_info |= (msg_type << 1);
    }

    inline void set_msg_type_info_log(dlt_extended_header_msg_type_info_log msg_type_val_log)
    {
        int msg_type_log = static_cast<int>(msg_type_val_log);

        message_info |= (msg_type_log << 4);
    }

    inline void set_msg_type_info_trace(dlt_extended_header_msg_type_info_trace msg_type_val_trace)
    {
        int msg_type_trace = static_cast<int>(msg_type_val_trace);

        message_info |= msg_type_trace;
    }

    inline void set_msg_type_info_nw(dlt_extended_header_msg_type_info_nw msg_type_val_nw)
    {
        int msg_type_nw = static_cast<int>(msg_type_val_nw);

        message_info |= msg_type_nw;
    }

    inline void set_msg_type_info_ctrl(dlt_extended_header_msg_type_info_ctrl msg_type_val_ctrl)
    {
        int msg_type_ctrl = static_cast<int>(msg_type_val_ctrl);

        message_info |= msg_type_ctrl;
    }

    inline bool has_verbose()
    {
        return !!(message_info & DLT_EXT_HDR_MSG_INFO_VERBOSE);
    }

    inline void set_app_id(uint8_t *app_id_val)
    {
        app_id[0] = app_id_val[0];
        app_id[1] = app_id_val[1];
        app_id[2] = app_id_val[2];
        app_id[3] = app_id_val[3];
    }

    inline void set_context_id(uint8_t *ctx_id_val)
    {
        context_id[0] = ctx_id_val[0];
        context_id[1] = ctx_id_val[1];
        context_id[2] = ctx_id_val[2];
        context_id[3] = ctx_id_val[3];
    }
};

#define DLT_STD_HDR_HTYPE_LEN           1
#define DLT_STD_HDR_MSG_COUNTER_LEN     1
#define DLT_STD_HDR_LENGTH_LEN          2
#define DLT_STD_HDR_ECU_ID_LEN          4
#define DLT_STD_HDR_SESSION_ID_LEN      4
#define DLT_STD_HDR_TIMESTAMP_LEN       4

#define DLT_EXT_HDR_MSIN_LEN            1
#define DLT_EXT_HDR_NO_ARGS_LEN         1
#define DLT_EXT_HDR_APP_ID_LEN          4
#define DLT_EXT_HDR_CTX_ID_LEN          4

struct dlt_header {
    dlt_standard_header std_hdr;
    dlt_extended_header ext_hdr;
    dlt_msg_typeinfo msg_type_info;

    inline void set_msg_type_info(dlt_msg_typeinfo mt_val) { msg_type_info = mt_val; }
    inline int get_length(uint32_t payload_len)
    {
        int len = 0;

        len += DLT_STD_HDR_HTYPE_LEN +
               DLT_STD_HDR_MSG_COUNTER_LEN +
               DLT_STD_HDR_LENGTH_LEN;

        if (!!(std_hdr.header_type & DLT_HDR_TYPE_WITH_ECU_ID)) {
            len += DLT_STD_HDR_ECU_ID_LEN;
        }
        if (!!(std_hdr.header_type & DLT_HDR_TYPE_WITH_SESSION_ID)) {
            len += DLT_STD_HDR_SESSION_ID_LEN;
        }
        if (!!(std_hdr.header_type & DLT_HDR_TYPE_WITH_TIMESTAMP)) {
            len += DLT_STD_HDR_TIMESTAMP_LEN;
        }

        if (!!(std_hdr.header_type & DLT_HDR_TYPE_USE_EXT_HEADER)) {
            len += DLT_EXT_HDR_MSIN_LEN +
                   DLT_EXT_HDR_NO_ARGS_LEN +
                   DLT_EXT_HDR_APP_ID_LEN +
                   DLT_EXT_HDR_CTX_ID_LEN;
        }

        switch (msg_type_info) {
            case DLT_MSG_TYPEINFO_STRG: // 4 bytes is the length of typeinfo
                len += 4;
            break;
            default:
                return -1;
        }

        // payload len
        len += 2;
        len += payload_len;

        return len;
    }
    int encode(uint8_t *payload, uint16_t payload_len, uint8_t *buff, size_t buff_size, size_t &off);
    int decode(uint8_t *payload, uint16_t &payload_len, uint8_t *buff, size_t buff_size, size_t &off);
};

}

#endif

