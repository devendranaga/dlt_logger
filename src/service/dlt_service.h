#ifndef __AUTO_MIDDLEWARE_DLT_SERVICE_H__
#define __AUTO_MIDDLEWARE_DLT_SERVICE_H__

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <auto_lib.h>
#include <dlt_enc_dec.h>

#define MSG_COUNTER_MAX_UINT 255
#define DLT_CONFIG_FILE "./dlt_config.json"

namespace auto_os::middleware {

enum class network_conn_type {
    UNIX,
    UDP,
};

struct dlt_config {
    std::string ecu_id;
    network_conn_type conn_type;
    std::string unix_server_path;
    std::string storage_service_addr;
    int storage_service_port;

    ~dlt_config() { }
    dlt_config(const dlt_config &) = delete;
    const dlt_config& operator=(const dlt_config &) = delete;
    dlt_config(const dlt_config &&) = delete;
    const dlt_config&& operator=(const dlt_config &&) = delete;

    static dlt_config *instance()
    {
        static dlt_config config;
        return &config;
    }

    int parse(const std::string config_file);

    private:
        explicit dlt_config() { }
};

struct dlt_rx_msg {
    uint8_t rx_msg[4096];
    int rx_msg_len;
};

class dlt_service {
    public:
        explicit dlt_service() noexcept;
        ~dlt_service();

        void run();

    private:
        // [PRS_Dlt_00105] ⌈The Dlt module shall increment the Message Counter by one at
        // every Log and Trace message received via the Dlt API.⌋ (SRS_Dlt_00018)
        //
        // [PRS_Dlt_00106] ⌈If the Message Counter reaches 255, the counter shall wrap
        // around and start with the value ‘0’ at the next Log and Trace message to be
        // transmitted.⌋ (SRS_Dlt_00018
        //
        inline void inc_msg_counter()
        {
            msg_counter_ = (msg_counter_ + 1) % MSG_COUNTER_MAX_UINT;
        }

        // [PRS_Dlt_00308] ⌈If the ECU ID is shorter than four 8-bit ASCII characters, the
        // remaining characters shall be filled with 0x00. ⌋ (SRS_Dlt_00022)
        // 
        inline void fill_ecu_id()
        {
            dlt_config *config = dlt_config::instance();
            int i;

            ecu_id_[0] = ecu_id_[1] = ecu_id_[2] = ecu_id_[3] = 0x00;

            for (i = 0; i < config->ecu_id.length(); i ++) {
                ecu_id_[i] = config->ecu_id[i];
            }
        }
        void receive_dlt_message(int fd);
        void process_received_message();
        auto_os::lib::event_manager *evt_mgr_;
        std::shared_ptr<auto_os::lib::logger> log_;
        std::shared_ptr<auto_os::lib::unix_udp_server> server_;
        std::unique_ptr<auto_os::lib::udp_client> storage_client_;
        uint8_t msg_counter_;
        uint8_t ecu_id_[4];
        std::queue<dlt_rx_msg> rx_msg_list_;
        std::unique_ptr<std::thread> process_msg_thr_;
        std::mutex rx_msg_q_lock_;
};

}

#endif

