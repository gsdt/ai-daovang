#include <iostream>
#include <string>
#include "sockpp/tcp_connector.h"
#include "remote.h"

using namespace std::chrono;

const std::string MOVE_LEFT = "0\n";
const std::string MOVE_RIGHT = "1\n";
const std::string MOVE_UP = "2\n";
const std::string MOVE_DOWN = "3\n";
const std::string FREE = "4\n";
const std::string CRAFT = "5\n";

int main(int argc, char* argv[])
{
	sockpp::socket_initializer sockInit;
	
	std::string host = (argc > 1) ? argv[1] : "localhost";
	in_port_t port = (argc > 2) ? atoi(argv[2]) : 9000;

	std::cout << "Working on " << host << ":" << port << std::endl;

	remote r(host, port, 100);
	
	std::string map_info = r.read_all();

	while (true)
	{
		r.send_all("4");
		r.read_all();

		r.send_all("4");
		r.read_all();


		r.send_all("1");
		r.read_all();
	}
	

	return 0;
}