#include <iostream>
#include <getopt.h>
#include <dlt_lib.hpp>

int main(int argc, char **argv)
{
    auto_os::middleware::dlt_lib *log;
    std::string session_id = "sess";
    std::string app_id = "app1";
    std::string context_id = "ctx1";

    // get dlt lib logging instance
    log = auto_os::middleware::dlt_lib::instance();
    // connect to the dlt daemon
    log->connect("/tmp/dlt.sock", (uint8_t *)(session_id.c_str()));
    // send test messages
    log->info(app_id, context_id, "testing dlt message\n");
    log->warning(app_id, context_id, "testing dlt message\n");
    log->verbose(app_id, context_id, "testing dlt message\n");
    log->error(app_id, context_id, "testing dlt message\n");
    log->fatal(app_id, context_id, "testing dlt message\n");
}

