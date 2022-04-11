/** @file IPCPipeSend.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#include "IPCPipeSend.h"

IPCPipeSend::~IPCPipeSend()
{
	if (this->pd > 0)
		close(this->pd);
	unlink(this->opts.server_name.c_str());
}

void IPCPipeSend::init()
{
	// remove old pipe (if any)
	remove(this->opts.server_name.c_str());
	// create a fifo
	if (mkfifo(this->opts.server_name.c_str(), 0660) != 0)
		throw std::runtime_error("ERROR: " + this->opts.server_name + ": " + strerror(errno));
	// open the pipe with O_NONBLOCK to get EAGAIN
	this->pd = open(this->opts.server_name.c_str(), O_RDWR | O_NONBLOCK);
	if (this->pd == -1)
		throw std::runtime_error("ERROR: " + this->opts.server_name + ": " + strerror(errno));
	else
		std::cout << "[" << this->opts.server_name << "] is opened." << std::endl;


	this->open_file();
	this->info.file_size = this->get_file_size();
	if (this->info.file_size == 0)
		throw std::runtime_error("ERROR: File size = 0.");
	this->timer.update_all();

}

void IPCPipeSend::transfer()
{
	std::vector<char> buffer(this->p_msgsize);

	std::cout << "Sending..." << std::endl;
	while (!(this->fs.eof()))
	{
		this->fs.read(buffer.data(), this->p_msgsize);
		if (this->fs.bad()) // check read/writing error on i/o operation
			throw std::runtime_error("ERROR: istream::read().");

		this->info.read_bytes = this->fs.gcount();
		if (this->info.read_bytes > 0)
		{
			errno = 0; // clear errno
			this->info.sent_bytes = write(this->pd, buffer.data(), this->info.read_bytes);
			if (this->info.sent_bytes == this->info.read_bytes && this->info.sent_bytes != 0)
			{
				this->info.total_sent_bytes += this->info.sent_bytes;
				this->timer.update_all();

				if (this->fs.eof()) // wait for receiver 10s before close the pipe
					while (this->timer.get_duration() < this->timeout)
						this->timer.update_end();

			}
			else if (this->info.sent_bytes == -1 && errno == EAGAIN)
			{
				while (errno == EAGAIN && this->timer.get_duration() < this->timeout)
				{
					this->timer.update_end();
					errno = 0;
					this->info.sent_bytes = write(this->pd, buffer.data(), this->info.read_bytes);
					if (this->info.sent_bytes == this->info.read_bytes)
					{
						this->info.total_sent_bytes += this->info.sent_bytes;
						this->timer.update_all();
					}
				}
				if (errno == EAGAIN && this->timer.get_duration() >= this->timeout)
					throw std::runtime_error(
						"ERROR: Timeout. Waited for client more than " + std::to_string(this->timeout) + " seconds.");
			}
			else
				throw std::runtime_error(static_cast<std::string>("ERROR: write():") + strerror(errno));
		}
	}

	if (this->info.total_sent_bytes == this->info.file_size && this->info.sent_bytes != 0)
		std::cout << "Sent " << this->info.total_sent_bytes << " byte(s)." << std::endl;
	else
		throw std::runtime_error("ERROR: Uncompleted transfer");

}
