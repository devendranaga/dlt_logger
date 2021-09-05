#include <dlt_logger.h>

namespace auto_os::middleware {

dlt_logger::dlt_logger() noexcept
{
    log_ = auto_os::lib::logger_factory::Instance()->create(auto_os::lib::logging_type::console_logging);
    evt_mgr_ = auto_os::lib::event_manager::instance();

    msg_counter = 0;
}

dlt_logger::~dlt_logger()
{

}

}

int main()
{

}
