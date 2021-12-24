#include <dlt_service.h>

namespace auto_os::middleware {

dlt_service::dlt_service() noexcept
{
    log_ = auto_os::lib::logger_factory::Instance()->create(auto_os::lib::logging_type::console_logging);
    evt_mgr_ = auto_os::lib::event_manager::instance();

    log_->debug("starting dlt_service\n");
    msg_counter = 0;
}

dlt_service::~dlt_service()
{

}

void dlt_service::run()
{
    evt_mgr_->start();
}

}

int main()
{
    auto_os::middleware::dlt_service logger;

    logger.run();
}

