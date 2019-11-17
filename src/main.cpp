#include "remote.h"
#include "boardgame.h"
#include "constant.h"

#include <iostream>
#include <string>
#include <chrono>

int main(int argc, char *argv[])
{
	std::string host = (argc > 1) ? argv[1] : "localhost";
	int16_t port = (argc > 2) ? atoi(argv[2]) : 9000;

	std::cout << "Working on " << host << ":" << port << std::endl;

	remote r(host, port, 100);

	std::string init_map_data = r.read_all();
	boardgame game(init_map_data);

	int my_id = game.my_player_id;
	int code = game.can_move(my_id, D_RIGHT);

	std::cout << code << std::endl;
	// exit(0);

	int current_direction = D_RIGHT;
	std::string current_command = MOVE_RIGHT;

	while (true)
	{
		int my_id = game.my_player_id;
		
		code = game.can_craft(my_id);
		if (code == OK) {
			r.send_all(ACT_CRAFT);
		}
		if(code == NOT_ENERGY) {
			r.send_all(ACT_FREE);
		}
		if(code == NOT_GOLD) {
			code = game.can_move(my_id, current_direction);
			if(code == OK) {
				r.send_all(current_command);
				if(current_direction == D_DOWN) {
					if(game.can_move(my_id, D_RIGHT) == OUT_SIDE) {
						current_direction = D_LEFT;
						current_command = MOVE_LEFT;
					}
					else {
						current_direction = D_RIGHT;
						current_command = MOVE_RIGHT;
					}
				}
			}
			if(code == NOT_ENERGY) {
				r.send_all(ACT_FREE);
			}
			if(code == OUT_SIDE) {
				if(current_direction == D_RIGHT || current_direction == D_LEFT) {
					current_direction = D_DOWN;
					current_command = MOVE_DOWN;
				}
				continue;
			}
		}
		std::string msg = r.read_all();
		game.update(msg);
	}

	return 0;
}