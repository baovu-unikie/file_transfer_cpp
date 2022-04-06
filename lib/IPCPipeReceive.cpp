/** @file IPCPipeReceive.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief Include IPCPipeReceive class methods' definition
 */

#include "IPC.h"

void IPCPipeReceive::init()
{
	this->timer.update_all(); // reset timer
	do
	{
		print_wait_msg("Waiting for [" + this->opts.server_name + "]");
		this->pd = open(this->opts.server_name.c_str(), O_RDONLY | O_NONBLOCK);
		this->timer.update_end();
		sleep(1); // recheck every 1 sec
	} while (this->pd <= 0 && this->timer.get_duration() < this->timeout);
	std::cout << std::endl; // print_wait_msg doesn't add new line.

	if(this->pd <= 0)
		throw std::runtime_error(
			"ERROR: Timeout. Waited for [" + this->opts.server_name + "] more than " + std::to_string(this->timeout) + " seconds.");

	this->open_file();
	this->buffer.resize(this->p_msgsize);
	this->timer.update_all(); // reset timer
}

void IPCPipeReceive::transfer()
{
	std::cout << "Receiving..." << std::endl;
	while (this->timer.get_duration() < this->timeout)
	{
		errno = 0;
		this->ipc_info.read_bytes = read(this->pd, this->buffer.data(), this->p_msgsize);
		if (this->ipc_info.read_bytes > 0)
		{
			this->write_to_file(this->buffer, this->ipc_info.read_bytes);
			this->timer.update_all();
		}
		this->timer.update_end();
		if (errno == 0 && this->ipc_info.read_bytes == 0)
			break;
	}

	if (errno == EAGAIN && this->timer.get_duration() >= this->timeout)
		throw std::runtime_error(
			"ERROR: Timeout. Waited for the sender more than " + std::to_string(this->timeout) + " seconds.");
	else
		std::cout << "The pipe closed. Received data: " << this->get_file_size() << " byte(s)" << std::endl;
}

