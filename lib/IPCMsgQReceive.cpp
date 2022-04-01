/** @file IPCMsgQReceive.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief Includes IPCMsgQReceive class methods' definition
 */

#include "IPC.h"

void IPCMsgQReceive::init()
{
	this->timer.update_all(); // reset timer before use

	do
	{
		print_wait_msg("Waiting for /dev/mqueue");
		this->timer.update_end();
		this->mqd = mq_open(this->opts.server_name.c_str(), O_RDONLY | O_NONBLOCK, 0660, &(this->attr));
		sleep(1);
	} while (this->mqd == -1 && (this->timer.get_duration() < this->timeout));

	std::cout << std::endl;

	if (this->mqd == -1)
	{
		this->cleanup();
		throw std::runtime_error(
			"ERROR: Timeout. Waited for the sender more than " + std::to_string(this->timeout) + " seconds.");
	}
	else
		std::cout << "/dev/mqueue" << this->opts.server_name << " is opened." << std::endl;

	this->open_file();
	this->timer.update_all();
	this->buffer = new char[this->attr.mq_msgsize];
}

void IPCMsgQReceive::receive()
{
	std::cout << "Waiting for new message..." << std::endl;
	errno = 0; // clear error number
	while (errno != EAGAIN || ((errno == EAGAIN) && (this->timer.get_duration() < this->timeout)))
	{
		errno = 0; // clear error number
		this->ipc_info.read_bytes = mq_receive(this->mqd, buffer, this->attr.mq_msgsize, nullptr);
		if (this->ipc_info.read_bytes > 0)
		{
			std::cout << "\r" << std::flush;
			std::cout << "Received " << ++(this->ipc_info.number_of_msg) << " messages." << std::flush;
			this->write_to_file(buffer, this->ipc_info.read_bytes);
			this->timer.update_begin();
		}
		this->timer.update_end();
	}
	std::cout << "\nThe message queue is closed. Received data: " << this->get_file_size() << " byte(s)" << std::endl;
}
