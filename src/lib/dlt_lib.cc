#include <dlt_lib.hpp>

namespace auto_os::middleware {

int dlt_lib::connect(const std::string dlt_server_addr, uint8_t *session_id)
{
    std::unique_ptr<auto_os::lib::random_generator> rg;
    char client_path[1024];
    uint32_t rand_val;

    server_path_ = dlt_server_addr;
    SET_4_BYTES(session_id_, session_id);

    rg = auto_os::lib::random_generator_factory::get()->create(
            auto_os::lib::random_generator_type::elinux_random);

    rg->get(rand_val);

    snprintf(client_path, sizeof(client_path), "/tmp/dlt_client_%u.sock",
                                                rand_val);
    client_path_ = std::string(client_path);
    client_ = std::make_unique<auto_os::lib::unix_udp_client>(client_path_);

    return 0;
}

void dlt_lib::info(const std::string app_id, const std::string ctx_id, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    send_dlt_msg(app_id, ctx_id, fmt, ap);
    va_end(ap);
}

void dlt_lib::send_dlt_msg(const std::string app_id, const std::string ctx_id, const char *fmt, va_list ap)
{
    char *data[4096];
    dlt_msg_if *msg = (dlt_msg_if *)data;
    int len;

    SET_4_BYTES(msg->app_id, app_id);
    SET_4_BYTES(msg->ctx_id, ctx_id);
    SET_4_BYTES(msg->session_id, session_id_);
    msg->dlt_log_lvl = DLT_MSG_LOG_LVL_INFO;
    msg->dlt_msg_type_info = DLT_MSG_TYPEINFO_STRG;

    len = vsnprintf(msg->dlt_msg, sizeof(data) - sizeof(dlt_msg_if), fmt, ap);

    client_->send_msg(server_path_, (uint8_t *)data, sizeof(dlt_msg_if) + len);
}

}
