#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <auto_lib.h>
#include <dlt_header.h>

namespace auto_os::middleware {

class dlt_logger  {
    public:
        explicit dlt_logger() noexcept;
        ~dlt_logger();

        void run();

    private:
        auto_os::lib::event_manager *evt_mgr_;
        std::shared_ptr<auto_os::lib::logger> log_;
        uint8_t msg_counter;
};

}

