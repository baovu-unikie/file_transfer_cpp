/** @file IPCPipeReceive.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#include "IPCPipeReceive.h"

IPCPipeReceive::~IPCPipeReceive()
{
	if (this->pd > 0)
		close(this->pd);
	unlink(this->opts.server_name.c_str());
}

void IPCPipeReceive::init()
{
	std::cout << "Waiting for sender... " << std::endl;
	this->timer.update_all(); // reset timer
	do
	{
		this->pd = open(this->opts.server_name.c_str(), O_RDONLY | O_NONBLOCK);
		this->timer.update_end();
	} while (this->pd <= 0 && this->timer.get_duration() < this->timeout);

	if(this->pd <= 0)
		throw std::runtime_error(
			"ERROR: Timeout. Waited for [" + this->opts.server_name + "] more than " + std::to_string(this->timeout) + " seconds.");

	this->open_file();
	this->timer.update_all(); // reset timer

}

void IPCPipeReceive::transfer()
{
	std::vector<char> buffer(this->p_msgsize);
	std::cout << "Receiving..." << std::endl;
	while (this->timer.get_duration() < this->timeout)
	{
		errno = 0;
		this->info.read_bytes = read(this->pd, buffer.data(), this->p_msgsize);
		if (errno == 0 && this->info.read_bytes == 0)
			break;
		if (this->info.read_bytes > 0)
		{
			this->write_file(buffer, this->info.read_bytes);
			this->timer.update_all();
		}
		this->timer.update_end();
	}

	if (errno == EAGAIN && this->timer.get_duration() >= this->timeout)
		throw std::runtime_error(
			"ERROR: Timeout. Waited for the sender more than " + std::to_string(this->timeout) + " seconds.");
	else
	{
		this->fs.close();
		std::cout << "The pipe closed. Received data: " << this->get_file_size() << " byte(s)" << std::endl;
	}
}
