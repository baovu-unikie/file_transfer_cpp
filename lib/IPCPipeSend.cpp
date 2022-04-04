/** @file IPCPipeSend.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief Include IPCPipeSend class methods' definition
 */

#include "IPC.h"

IPCPipeSend::~IPCPipeSend()
{

	if (this->pd > 0)
	{
		mq_close(this->pd);
		std::cout << "Closed the pipe." << std::endl;
	}
	unlink(this->opts.server_name.c_str());
	std::cout << "Unlinked the pipe name." << std::endl;

}

void IPCPipeSend::cleanup()
{
	if (this->pd != -1)
	{
		close(this->pd);
		std::cout << "Closed the pipe." << std::endl;
	}
	remove(this->opts.server_name.c_str());
	std::cout << "Removed the pipe." << std::endl;
	IPC::cleanup();
}

void IPCPipeSend::init()
{
	// remove old pipe (if any)
	remove(this->opts.server_name.c_str());
	// create a fifo
	if (mkfifo(this->opts.server_name.c_str(), 0660) != 0)
	{
		this->cleanup();
		throw std::runtime_error("ERROR: " + this->opts.server_name + ": " + strerror(errno));
	}
	// open the pipe with O_NONBLOCK to get EAGAIN
	this->pd = open(this->opts.server_name.c_str(), O_RDWR | O_NONBLOCK);
	if (this->pd == -1)
	{
		this->cleanup();
		throw std::runtime_error("ERROR: " + this->opts.server_name + ": " + strerror(errno));
	}
	else
		std::cout << "[" << this->opts.server_name << "] is opened." << std::endl;


	this->open_file();
	this->ipc_info.file_size = this->get_file_size();
	if(this->ipc_info.file_size == 0)
	{
		this->cleanup();
		throw std::runtime_error("ERROR: File size = 0.");
	}
	this->buffer.resize(this->p_msgsize);
}

void IPCPipeSend::print_members() const
{
	IPC::print_members();
	std::cout << "pd: " << this->pd << std::endl;
}

void IPCPipeSend::send()
{
	this->timer.update_all();

	std::cout << "Sending..." << std::endl;
	while (!(this->fs.eof()))
	{
		this->fs.read(this->buffer.data(), this->p_msgsize);
		if (this->fs.bad()) // check read/writing error on i/o operation
			throw std::runtime_error("ERROR: istream::read().");

		this->ipc_info.read_bytes = this->fs.gcount();
		if (this->ipc_info.read_bytes >= 0)
		{
			errno = 0; // clear errno
			this->ipc_info.sent_bytes = write(this->pd, this->buffer.data(), this->ipc_info.read_bytes);
			if (this->ipc_info.sent_bytes == this->ipc_info.read_bytes && this->ipc_info.sent_bytes != 0)
			{
				this->ipc_info.total_sent_bytes += this->ipc_info.sent_bytes;
				this->timer.update_all();
			}
			else if (this->ipc_info.sent_bytes == -1 && errno == EAGAIN)
			{
				std::cout << "The pipe is full. Waiting for client to empty the pipe.";
				while (errno == EAGAIN && this->timer.get_duration() < this->timeout)
				{
					std::cout << "." << std::flush;
					this->timer.update_end();
					errno = 0;
					this->ipc_info.sent_bytes = write(this->pd, this->buffer.data(), this->ipc_info.read_bytes);
					if (this->ipc_info.sent_bytes == this->ipc_info.read_bytes)
					{
						this->ipc_info.total_sent_bytes += this->ipc_info.sent_bytes;
						this->timer.update_all();
					}
					sleep(1);
				}
				std::cout << std::endl;
				if (errno == EAGAIN && this->timer.get_duration() >= this->timeout)
				{
					this->cleanup();
					throw std::runtime_error(
						"ERROR: Timeout. Waited for client more than " + std::to_string(this->timeout) + " seconds.");
				}
			}
			else
			{
				this->cleanup();
				throw std::runtime_error(static_cast<std::string>("ERROR: write():") + strerror(errno));
			}
		}
	}

	if (this->ipc_info.total_sent_bytes == this->ipc_info.file_size && this->ipc_info.sent_bytes != 0)
	{
		std::cout << "Sent " << this->ipc_info.total_sent_bytes << " byte(s)." << std::endl;
	}
	else
	{
		this->cleanup();
		throw std::runtime_error("ERROR: Uncompleted transfer");
	}
}

