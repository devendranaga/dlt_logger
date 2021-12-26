#include <iostream>
#include <getopt.h>
#include <dlt_lib.hpp>

int main(int argc, char **argv)
{
    auto_os::middleware::dlt_lib *log;
    std::string session_id = "sess";
    std::string app_id = "app1";
    std::string context_id = "ctx1";

    log = auto_os::middleware::dlt_lib::instance();
    log->connect("/tmp/dlt.sock", (uint8_t *)(session_id.c_str()));
    log->info(app_id, context_id, "testing dlt message\n");
}

