#include <fstream>
#include <functional>
#include <jsoncpp/json/json.h>
#include <dlt_msg_if.h>
#include <dlt_service.h>

namespace auto_os::middleware {

int dlt_config::parse(const std::string config_file)
{
    Json::Value root;
    std::ifstream conf(config_file, std::ifstream::binary);

    conf >> root;

    use_ext_hdr = root["htype_use_extended_hdr"].asBool();
    use_msb_first = root["htype_msb_first"].asBool();
    send_ecu_id = root["htype_send_ecu_id"].asBool();
    send_timestamp = root["htype_send_timestamp"].asBool();
    version = root["htype_version"].asInt();
    verbose_mode = root["ext_hdr_verbose_mode"].asBool();
    ecu_id = root["htype_ecu_id"].asString();
    auto socket_type = root["network"]["socket_type"].asString();
    if (socket_type == "unix") {
        conn_type = network_conn_type::UNIX;
    } else if (socket_type == "udpv4") {
        conn_type = network_conn_type::UDP;
    }

    unix_server_path = root["network"]["unix_socket"]["server_path"].asString();
    storage_service_addr = root["network"]["storage_server"]["server_address"].asString();
    storage_service_port = root["network"]["storage_server"]["server_port"].asInt();

    return 0;
}

dlt_service::dlt_service() noexcept
{
    dlt_config *config;
    int ret;

    config = dlt_config::instance();
    ret = config->parse(DLT_CONFIG_FILE);
    if (ret < 0) {
        throw std::runtime_error("failed to parse dlt config file");
    }

    log_ = auto_os::lib::logger_factory::Instance()->create(auto_os::lib::logging_type::console_logging);
    log_->set_app_name("dlt_service");

    evt_mgr_ = auto_os::lib::event_manager::instance();

    // [PRS_Dlt_00613] ⌈After initialization of the Dlt module, the Message Counter
    // (MCNT) shall be set to ‘0’. ⌋()
    log_->debug("starting dlt_service\n");
    msg_counter_ = 0;

    // setup ecu id
    fill_ecu_id();

    server_ = std::make_shared<auto_os::lib::unix_udp_server>(config->unix_server_path);
    auto rx_callback = std::bind(&dlt_service::receive_dlt_message, this, std::placeholders::_1);
    evt_mgr_->create_socket_event(server_->get_socket(), rx_callback);
    log_->debug("created unix udp server [%s]\n", config->unix_server_path.c_str());

    process_msg_thr_ = std::make_unique<std::thread>(&dlt_service::process_received_message, this);
    process_msg_thr_->detach();
    log_->debug("created process_msg thread\n");

    storage_client_ = std::make_unique<auto_os::lib::udp_client>();
    log_->debug("created client interface to storage\n");
}

void dlt_service::receive_dlt_message(int fd)
{
    dlt_rx_msg dlt_msg;
    std::string sender_path;
    int ret;

    // receive a message from the client
    ret = server_->recv_msg(sender_path, dlt_msg.rx_msg, sizeof(dlt_msg.rx_msg));
    if (ret < 0) {
        return;
    }

    dlt_msg.rx_msg_len = ret;

    printf("received msg len %d\n", ret);

    {
        // queue received messages
        std::unique_lock<std::mutex> lock(rx_msg_q_lock_);
        rx_msg_list_.emplace(dlt_msg);
    }
}

void dlt_service::process_received_message()
{
    dlt_config *config = dlt_config::instance();

    while (1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        {
            std::unique_lock<std::mutex> lock(rx_msg_q_lock_);
            int q_len = rx_msg_list_.size();
            while (q_len > 0) {
                dlt_rx_msg msg = rx_msg_list_.front();

                dlt_msg_if *rx_msg = (dlt_msg_if *)msg.rx_msg;

                dlt_header hdr;
                uint8_t tx_buf[1024];
                size_t off = 0;

                hdr.set_msg_type_info(dlt_msg_typeinfo::DLT_MSG_TYPEINFO_STRG);
                hdr.std_hdr.set_use_ext_hdr();
                hdr.std_hdr.set_valid_ecu_id();
                hdr.std_hdr.set_valid_session_id();
                hdr.std_hdr.set_version(1);
                hdr.std_hdr.set_msg_counter(msg_counter_);
                hdr.std_hdr.set_ecu_id(config->ecu_id);
                hdr.std_hdr.set_session_id(rx_msg->session_id);

                hdr.ext_hdr.set_verbose();
                hdr.ext_hdr.set_app_id(rx_msg->app_id);
                hdr.ext_hdr.set_context_id(rx_msg->ctx_id);
                switch (rx_msg->dlt_log_lvl) {
                    case DLT_MSG_LOG_LVL_INFO:
                        hdr.ext_hdr.set_msg_type(
                            dlt_extended_header_msg_type::eDLT_TYPE_LOG);
                        hdr.ext_hdr.set_msg_type_info_log(
                            dlt_extended_header_msg_type_info_log::eDLT_LOG_INFO);
                    break;
                    case DLT_MSG_LOG_LVL_WARNING:
                        hdr.ext_hdr.set_msg_type(
                            dlt_extended_header_msg_type::eDLT_TYPE_LOG);
                        hdr.ext_hdr.set_msg_type_info_log(
                            dlt_extended_header_msg_type_info_log::eDLT_LOG_WARN);
                    break;
                    case DLT_MSG_LOG_LVL_VERBOSE:
                        hdr.ext_hdr.set_msg_type(
                            dlt_extended_header_msg_type::eDLT_TYPE_LOG);
                        hdr.ext_hdr.set_msg_type_info_log(
                            dlt_extended_header_msg_type_info_log::eDLT_LOG_VERBOSE);
                    break;
                    case DLT_MSG_LOG_LVL_ERROR:
                        hdr.ext_hdr.set_msg_type(
                            dlt_extended_header_msg_type::eDLT_TYPE_LOG);
                        hdr.ext_hdr.set_msg_type_info_log(
                            dlt_extended_header_msg_type_info_log::eDLT_LOG_ERROR);
                    break;
                    case DLT_MSG_LOG_LVL_FATAL:
                        hdr.ext_hdr.set_msg_type(
                            dlt_extended_header_msg_type::eDLT_TYPE_LOG);
                        hdr.ext_hdr.set_msg_type_info_log(
                            dlt_extended_header_msg_type_info_log::eDLT_LOG_FATAL);
                    break;
                    default:
                        rx_msg_list_.pop();
                        q_len = rx_msg_list_.size();
                    continue;
                }

                int len = hdr.encode((uint8_t *)(rx_msg->dlt_msg), msg.rx_msg_len - sizeof(dlt_msg_if),
                           tx_buf, sizeof(tx_buf), off);

                storage_client_->send_msg(config->storage_service_addr,
                                          config->storage_service_port,
                                          tx_buf, len);

                inc_msg_counter();

                rx_msg_list_.pop();
                q_len = rx_msg_list_.size();
                printf("q len %d\n", q_len);
            }
        }
    }
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

