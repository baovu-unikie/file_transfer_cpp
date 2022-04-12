/** @file IPCPipeSend.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#include "IPCPipeSend.h"

IPCPipeSend::~IPCPipeSend()
{
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
	this->timer.update_all();

}

void IPCPipeSend::transfer()
{
	std::vector<char> buffer(this->p_msgsize);
	long sent_bytes{0};
	long read_bytes{0};
	unsigned long total_sent_bytes{0};
	unsigned long file_size = this->get_file_size();
	if (file_size == 0)
		throw std::runtime_error("ERROR: File size = 0.");

	std::cout << "Sending..." << std::endl;
	while (!(this->fs.eof()))
	{
		this->fs.read(buffer.data(), this->p_msgsize);
		if (this->fs.bad()) // check read/writing error on i/o operation
			throw std::runtime_error("ERROR: istream::read().");

		read_bytes = this->fs.gcount();
		if (read_bytes > 0)
		{
			errno = 0; // clear errno
			sent_bytes = write(this->pd, buffer.data(), read_bytes);
			if (sent_bytes == read_bytes && sent_bytes != 0)
			{
				total_sent_bytes += sent_bytes;
				this->timer.update_all();

				if (this->fs.eof()) // wait for receiver 10s before close the pipe
					while (this->timer.get_duration() < this->timeout)
						this->timer.update_end();

			}
			else if (sent_bytes == -1 && errno == EAGAIN)
			{
				while (errno == EAGAIN && this->timer.get_duration() < this->timeout)
				{
					this->timer.update_end();
					errno = 0;
					sent_bytes = write(this->pd, buffer.data(), read_bytes);
					if (sent_bytes == read_bytes)
					{
						total_sent_bytes += sent_bytes;
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

	if (total_sent_bytes == file_size && sent_bytes != 0)
		std::cout << "Sent data: " << total_sent_bytes << " byte(s)." << std::endl;
	else
		throw std::runtime_error("ERROR: Uncompleted transfer");

}
