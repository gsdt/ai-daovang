#ifndef REMOTE_H
#define REMOTE_H

#include "sockpp/tcp_connector.h"
#include <string>

class remote {
    private:
        sockpp::tcp_connector conn;

    public:
        remote(std::string host, in_port_t port, int64_t timeout);
        void send_all(std::string data);
        std::string read_all();
};

#endif