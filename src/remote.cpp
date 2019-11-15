#include "remote.h"
#include "sockpp/tcp_connector.h"

#include <string>
#include <iostream>

#define BUFFER_SIZE 1024

using namespace std::chrono;

remote::remote(std::string host, in_port_t port, int64_t timeout) {
    this->conn.connect({host, port});

	if (!this->conn) {
		std::cerr << "Error connecting to server at "
			<< sockpp::inet_address(host, port)
			<< "\n\t" << this->conn.last_error_str() << std::endl;
	}

	std::cout << "Created a connection from " << this->conn.address() << std::endl;

    if (!this->conn.read_timeout(microseconds(timeout))) {
        std::cerr << "Error setting timeout on TCP stream: "
            << this->conn.last_error_str() << std::endl;
    }
}

void remote::send_all(std::string message) {
    size_t n = this->conn.write(message);
    if(n != message.length()) {
        std::cerr << "Error sending message" << std::endl;
    }
}

std::string remote::read_all() {
    #ifdef DEBUG
    auto start = high_resolution_clock::now();
    #endif

    char* buff = (char*) malloc(BUFFER_SIZE);
	std::string res;
	while(true) {
		ssize_t n = this->conn.read(buff, BUFFER_SIZE);
		if(n <= 0) break;
		res.append(buff, n);
	}
    
    #ifdef DEBUG
    auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start); 
	std::cout << "Time to read data from server: " << duration.count() << std::endl;
    #endif 
    
    return res;
}