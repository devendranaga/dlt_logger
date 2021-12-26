#ifndef __AUTO_MIDDLEWARE_DLT_MSG_IF_H__
#define __AUTO_MIDDLEWARE_DLT_MSG_IF_H__

#define DLT_SERVER_ADDRESS "/tmp/dlt.sock"

enum dlt_msg_log_lvl {
    DLT_MSG_LOG_LVL_INFO = 1,
    DLT_MSG_LOG_LVL_VERBOSE,
    DLT_MSG_LOG_LVL_WARNING,
    DLT_MSG_LOG_LVL_ERROR,
    DLT_MSG_LOG_LVL_FATAL,
};

enum dlt_msg_typeinfo {
    DLT_MSG_TYPEINFO_BOOL = 1,
    DLT_MSG_TYPEINFO_SINT,
    DLT_MSG_TYPEINFO_UINT,
    DLT_MSG_TYPEINFO_FLOA,
    DLT_MSG_TYPEINFO_ARAY,
    DLT_MSG_TYPEINFO_STRG,
    DLT_MSG_TYPEINFO_RAWD,
    DLT_MSG_TYPEINFO_VARI,
    DLT_MSG_TYPEINFO_FIXP,
    DLT_MSG_TYPEINFO_TRAI,
    DLT_MSG_TYPEINFO_STRU,
};

struct dlt_msg_if {
    uint8_t app_id[4];
    uint8_t ctx_id[4];
    uint8_t session_id[4];

    // dlt_msg_log_lvl
    uint8_t dlt_log_lvl;
    uint8_t dlt_msg_type_info;
    char dlt_msg[0];
} __attribute__ ((__packed__));

#endif
