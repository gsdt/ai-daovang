#include "remote.h"
#include "boardgame.h"
#include "constant.h"

#include <iostream>
#include <string>


int main(int argc, char *argv[])
{
	std::string host = (argc > 1) ? argv[1] : "localhost";
	int16_t port = (argc > 2) ? atoi(argv[2]) : 9000;

	std::cout << "Working on " << host << ":" << port << std::endl;

	remote r(host, port, 100);

	std::string init_map_data = r.read_all();
	boardgame game(init_map_data);

	int my_id = game.my_player_id;
	

	while (true)
	{
		int my_id = game.my_player_id;
		#ifdef DEBUG
		std::cout << "------------> " << game.T << " <-------------" << std::endl;
		#endif
		int action = game.get_best_move(my_id);
		if(action < 0) r.send_all(CMD[A_FREE]);
		else r.send_all(CMD[action]);
		
		std::string msg = r.read_all();
		game.update(msg);
	}

	return 0;
}