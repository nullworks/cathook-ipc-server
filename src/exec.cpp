/*
 * exec.cpp
 *
 *  Created on: Jul 29, 2017
 *      Author: nullifiedcat
 */

#include "ipcb.hpp"
#include "cathookipc.hpp"

#include <string>
#include <stdio.h>

/*
 *
CatCommand exec_all("ipc_exec_all", "Execute command (on every peer)", [](const CCommand& args) {
	std::string command = args.ArgS();
	ReplaceString(command, " && ", " ; ");
	if (command.length() >= 63) {
		peer->SendMessage(0, 0, ipc::commands::execute_client_cmd_long, command.c_str(), command.length() + 1);
	} else {
		peer->SendMessage(command.c_str(), 0, ipc::commands::execute_client_cmd, 0, 0);
	}
});
 */

void ReplaceString(std::string& input, const std::string& what, const std::string& with_what) {
	size_t index;
	index = input.find(what);
	while (index != std::string::npos) {
		input.replace(index, what.size(), with_what);
		index = input.find(what, index + with_what.size());
	}
}

int main(int argc, const char** argv) {
	std::string cmd = "";
	if (argc < 2) return 1;
	char* endptr = nullptr;
	unsigned target_id = strtol(argv[1], &endptr, 10);
	if (endptr == argv[1]) {
		printf("Target id is NaN!\n");
		return 1;
	}
	if (target_id > 31) {
		printf("Invalid target id: %u\n", target_id);
		return 1;
	}
	for (int i = 2; i < argc; i++) {
		cmd += (std::string(argv[i]) + " ");
	}

	cat_ipc::Peer<server_data_s, user_data_s> peer("cathook_followbot_server", false, false);
	peer.Connect();

	printf("%u] %s\n", target_id, cmd.c_str());

	{
		if (peer.memory->peer_data[target_id].free) {
			printf("Trying to send command to a dead peer\n");
			return 1;
		}
	}
	ReplaceString(cmd, " && ", " ; ");
	if (cmd.length() >= 63) {
		peer.SendMessage(0, (1 << target_id), ipc_commands::execute_client_cmd_long, cmd.c_str(), cmd.length() + 1);
	} else {
		peer.SendMessage(cmd.c_str(), (1 << target_id), ipc_commands::execute_client_cmd, 0, 0);
	}


}
