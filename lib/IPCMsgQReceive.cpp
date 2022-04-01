/** @file IPCMsgQReceive.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#include "IPCMsgQReceive.h"

IPCMsgQReceive::~IPCMsgQReceive()
{
	if (this->mqd > 0)
		mq_close(this->mqd);
	mq_unlink(this->opts.server_name.c_str());
}

void IPCMsgQReceive::init()
{
	this->timer.update_all(); // reset timer before use

	std::cout << "Waiting for /dev/mqueue" << std::endl;
	do
	{
		this->timer.update_end();
		this->mqd = mq_open(this->opts.server_name.c_str(), O_RDONLY | O_NONBLOCK, 0660, &(this->attr));
	} while (this->mqd == -1 && (this->timer.get_duration() < this->timeout));

	if (this->mqd == -1)
		throw std::runtime_error(
			"ERROR: Timeout. Waited for the sender more than " + std::to_string(this->timeout) + " seconds.");
	else
		std::cout << "/dev/mqueue" << this->opts.server_name << " is opened." << std::endl;

	this->open_file();
	this->timer.update_all();
}

void IPCMsgQReceive::transfer()
{
	std::vector<char> buffer(this->attr.mq_msgsize);

	std::cout << "Waiting for new message..." << std::endl;
	errno = 0; // clear error number
	while (errno != EAGAIN || ((errno == EAGAIN) && (this->timer.get_duration() < this->timeout)))
	{
		errno = 0; // clear error number
		this->info.read_bytes = mq_receive(this->mqd, buffer.data(), this->attr.mq_msgsize, nullptr);
		if (this->info.read_bytes > 0)
		{
			this->write_file(buffer, this->info.read_bytes);
			this->timer.update_begin();
		}
		this->timer.update_end();
	}

	this->fs.close();
	std::cout << "The message queue is closed. Received data: " << this->get_file_size() << " byte(s)" << std::endl;
}
