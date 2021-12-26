#ifndef __AUTO_MIDDLEWARE_DLT_LIB_CPP_H__
#define __AUTO_MIDDLEWARE_DLT_LIB_CPP_H__

#include <iostream>
#include <string>
#include <stdarg.h>
#include <memory>
#include <dlt_msg_if.h>
#include <auto_lib.h>

namespace auto_os::middleware {

#define SET_4_BYTES(__left, __right) {\
    __left[0] = __right[0];\
    __left[1] = __right[1];\
    __left[2] = __right[2];\
    __left[3] = __right[3];\
}

class dlt_lib {
    public:
        ~dlt_lib() { }

        dlt_lib(const dlt_lib &) = delete;
        const dlt_lib &operator=(const dlt_lib &) = delete;
        dlt_lib(const dlt_lib &&) = delete;
        const dlt_lib &&operator=(const dlt_lib &&) = delete;

        static dlt_lib *instance()
        {
            static dlt_lib lib;
            return &lib;
        }

        int connect(const std::string dlt_server_addr,
                    uint8_t *session_id);
        int connect(const std::string dlt_server_addr,
                    int dlt_port,
                    uint8_t *session_id);

        void info(const std::string app_id, const std::string ctx_id, const char *fmt, ...);
        void warning(const std::string app_id, const std::string ctx_id, const char *fmt, ...);
        void verbose(const std::string app_id, const std::string ctx_id, const char *fmt, ...);
        void error(const std::string app_id, const std::string ctx_id, const char *fmt, ...);
        void fatal(const std::string app_id, const std::string ctx_id, const char *fmt, ...);

    private:
        explicit dlt_lib() { }
        uint8_t session_id_[4];
        std::string server_path_;
        std::string client_path_;
        std::unique_ptr<auto_os::lib::unix_udp_client> client_;
        void send_dlt_msg(const std::string app_id, const std::string ctx_id, const char *fmt, va_list ap);
};

}

#endif
