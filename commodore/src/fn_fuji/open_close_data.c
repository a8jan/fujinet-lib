#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "fujinet-fuji.h"
#include "fujinet-fuji-cbm.h"

// All commands are <01><cmd>, we reuse this buffer to send all data, and only have to set 2nd byte
uint8_t cmd_args[2] = { 0x01, 0x00 };

// this is a command that has no return data, so just push command, its data, and read the status
bool open_close_data(uint8_t cmd, bool should_close, uint16_t params_size, uint8_t *cmd_params)
{
	int bytes_written;
	bool is_success;
	cmd_args[1] = cmd;

	if (!open_or_write(cmd)) {
		return false;
	}

	is_open = true;

	// write the cmd parameters
	bytes_written = cbm_write(FUJI_CMD_CHANNEL, cmd_params, params_size);

	// we only use is_success if the write succeeded. We have to get the status either way.
	// so just store the is_success value and then decide whether to use it or not.
	// cbm_close(FUJI_CMD_CHANNEL);
	is_success = get_fuji_status(should_close);

	if (bytes_written != params_size) {
		printf("\nE %d != %d (s: %d)\n", bytes_written, params_size, is_success);
		// write failed, this is an out and out failure. The _fuji_status values will hold error strings etc.
		// force a close if it wouldn't have happened in the status
		if (!should_close) cbm_close(FUJI_CMD_CHANNEL);
		is_open = false;
		return false;
	}

	// if (!is_success) {
	// 	printf("\nerr: %d\ncmd: %d\n%s\n", _fuji_status.value.error, _fuji_status.value.cmd, _fuji_status.value.msg);
	// }
	return is_success;

}

bool open_close_data_1(uint8_t cmd, uint8_t param1)
{
	uint8_t pl[1];
	pl[0] = param1;
	return open_close_data(cmd, true, 1, pl);
}

bool open_close_data_2(uint8_t cmd, uint8_t param1, uint8_t param2)
{
	uint8_t pl[2];
	pl[0] = param1;
	pl[1] = param2;
	return open_close_data(cmd, true, 2, pl);
}
