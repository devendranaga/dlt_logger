#ifndef __AUTO_MIDDLEWARE_DLT_SERVICE_H__
#define __AUTO_MIDDLEWARE_DLT_SERVICE_H__

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <auto_lib.h>
#include <dlt_enc_dec.h>

namespace auto_os::middleware {

class dlt_service {
    public:
        explicit dlt_service() noexcept;
        ~dlt_service();

        void run();

    private:
        auto_os::lib::event_manager *evt_mgr_;
        std::shared_ptr<auto_os::lib::logger> log_;
        uint8_t msg_counter;
};

}

#endif

