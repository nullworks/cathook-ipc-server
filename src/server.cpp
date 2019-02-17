/*
 * main.cpp
 *
 *  Created on: Mar 19, 2017
 *      Author: nullifiedcat
 */

#include "ipcb.hpp"
#include "cathookipc.hpp"

cat_ipc::Peer<server_data_s, user_data_s>& peer() {
	static cat_ipc::Peer<server_data_s, user_data_s> object("cathook_followbot_server", false, true, true);
	return object;
}

#define ESC_PRINTF(s, ...) printf("\e[" s, __VA_ARGS__)
#define ESC_CUP(x, y) ESC_PRINTF("%d;%dH", (y), (x))
#define ESC_EL(n) ESC_PRINTF("%dK", (n))
#define ESC_SGR(n) ESC_PRINTF("%dm", (n))
#define ESC_ED(n) ESC_PRINTF("%dJ", (n));

#define TEXT_BOLD ESC_SGR(1)
#define TEXT_NORMAL ESC_SGR(0)

const std::string classes[] = {
	"Unknown", "Scout",
	"Sniper", "Soldier",
	"Demoman", "Medic",
	"Heavy", "Pyro",
	"Spy", "Engineer"
};

const std::string teams[] = {
	"UNK", "SPEC", "RED", "BLU"
};

bool good_class(int clazz) {
	return clazz > 0 && clazz < 10;
}

bool good_team(int team) {
	return team >= 0 && team < 4;
}

void print_status() {
	ESC_CUP(1, 1);
	ESC_ED(2);
	fflush(stdout);
	ESC_CUP(2, 2);
	TEXT_BOLD;
	printf("cathook IPC server");
	ESC_CUP(3, 4);
	printf("connected: ");
	TEXT_NORMAL; printf("%d / %d", peer().memory->peer_count, cat_ipc::max_peers); TEXT_BOLD;
	ESC_CUP(3, 5);
	printf("command count: ");
	TEXT_NORMAL; printf("%lu", peer().memory->command_count); TEXT_BOLD;
	ESC_CUP(3, 6);
	CatMemoryPool::pool_info_s info;
	peer().pool->statistics(info);
	printf("memory: ");
	TEXT_NORMAL; printf("%.1f%% free\n", float(info.free) / float(info.free + info.alloc) * 100); TEXT_BOLD;
	/*printf("command memory pool stats: ");
	CatMemoryPool::pool_info_s info;
	peer().pool->statistics(info);
	ESC_CUP(4, 8); ESC_EL(2);  printf("total:     ");
	ESC_CUP(4, 9); ESC_EL(2);  printf("free:      ");
	ESC_CUP(4, 10); ESC_EL(2); printf("allocated: ");
	TEXT_NORMAL;
	ESC_CUP(16, 8);  printf("%lu (%u blocks)", info.free + info.alloc, info.blkcnt);
	ESC_CUP(16, 9); printf("%lu (%u blocks)", info.free, info.freeblk);
	ESC_CUP(16, 10); printf("%lu (%u blocks)", info.alloc, info.allocblk);
	*/
	int ypos = 8;
	ESC_CUP(2, ypos);
	printf("%-2s %-9s %-21s %s\n", "ID", "SteamID", "Server IP", "Name");
	printf("    %-5s %-9s %-4s   %-5s   %-5s  %-9s %-11s %s", "State", "Class", "Team", "Score", "Total", "Health", "Heartbeat", "Party size");
	ypos += 3;
	TEXT_NORMAL;
	// Zeroth peer is the server.
	for (unsigned i = 0; i < cat_ipc::max_peers; i++) {
		if (!peer().memory->peer_data[i].free) {
			ESC_CUP(2, ypos);
			const auto& data = peer().memory->peer_user_data[i];
			printf("%-2u %-9u %-21s %s\n", i, data.friendid, data.ingame.server, data.name);
			if (data.connected && data.ingame.good) {
				printf("    %-5s %-9s %-4s   %-5d   %-5d   %-4d/%-4d %-11u %d\n",
						data.ingame.life_state ? "Dead" : "Alive",
						good_class(data.ingame.role) ? classes[data.ingame.role].c_str() : classes[0].c_str(),
						good_team(data.ingame.team) ? teams[data.ingame.team].c_str() : teams[0].c_str(),
						data.ingame.score, data.accumulated.score, data.ingame.health, data.ingame.health_max,
						time(nullptr) - peer().memory->peer_data[i].heartbeat, data.party_size);
			} else {
				printf("    %-5s %-9s %-4s   %-5s   %-5d   %-9s %-11u %d\n",
						"N/A", "N/A", "N/A", "N/A", data.accumulated.score,
						"N/A", time(nullptr) - peer().memory->peer_data[i].heartbeat,
						data.party_size);
			}
			ypos += 2;
		}
	}
	ESC_CUP(1, ypos + 1);
	fflush(stdout);
}

int main(int argc, char** argv) {
	unsigned long tick = 0;
	peer().Connect();
	peer().memory->global_data.magic_number = 0x0DEADCA7;
	bool silent = false;
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-s")) {
			silent = true;
		}
	}
	printf("cathook IPC server\n");
	while (true) {
		tick++;
		if (!(tick % 10)) { // Sweep/Process once every 10 seconds
			peer().SweepDead();
			peer().ProcessCommands();
		}
		if (not silent)
			print_status();
		sleep(1);
	}
	return 0;
}
