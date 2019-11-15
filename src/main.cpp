#include <iostream>
#include <string>
#include "sockpp/tcp_connector.h"
#include "remote.h"

using namespace std::chrono;

int main(int argc, char* argv[])
{
	sockpp::socket_initializer sockInit;
	
	std::string host = (argc > 1) ? argv[1] : "localhost";
	in_port_t port = (argc > 2) ? atoi(argv[2]) : 9000;

	std::cout << "Working on " << host << ":" << port << std::endl;

	remote r(host, port, 10);
	
	std::string map_info = r.read_all();

	while (true)
	{
		std::string zero = "0";
		std::string four = "1";
		r.send_all(zero);
		map_info = r.read_all();
		std::cout << "Readed: " << map_info.length() << std::endl;
		r.send_all(zero);
		map_info = r.read_all();
		std::cout << "Readed: " << map_info.length() << std::endl;
		r.send_all(four);
		map_info = r.read_all();
		std::cout << "Readed: " << map_info.length() << std::endl;
	}
	

	return 0;
}