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
		r.send_all("0");
		map_info = r.read_all();
		r.send_all("0");
		map_info = r.read_all();
		r.send_all("4");
		map_info = r.read_all();
	}
	

	return 0;
}