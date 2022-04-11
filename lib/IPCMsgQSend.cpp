/** @file IPCMsgQSend.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#include "IPCMsgQSend.h"

IPCMsgQSend::~IPCMsgQSend()
{
	mq_close(this->mqd);
	mq_unlink(this->opts.server_name.c_str());
}

void IPCMsgQSend::init()
{
	// remove old queue name (if any)
	mq_unlink(this->opts.server_name.c_str());
	errno = 0; // clear errno
	this->mqd = mq_open(this->opts.server_name.c_str(), O_CREAT | O_EXCL | O_WRONLY | O_NONBLOCK, 0660, &(this->attr));

	if (this->mqd == -1)
		throw std::runtime_error("ERROR: " + this->opts.server_name + ": " + strerror(errno));
	else
		std::cout << "/dev/mqueue" << this->opts.server_name << " is opened." << std::endl;

	this->open_file();
	this->timer.update_all();
}

void IPCMsgQSend::transfer()
{
	long mq_send_return_value{0};
	long read_bytes{0};
	std::vector<char> buffer(this->attr.mq_msgsize);
	unsigned total_sent_bytes{0};
	unsigned long file_size = this->get_file_size();
	if (file_size == 0)
		throw std::runtime_error("ERROR: File size = 0.");

	std::cout << "Sending..." << std::endl;
	while (!this->fs.eof())
	{
		this->read_file(buffer, this->attr.mq_msgsize);
		read_bytes = this->fs.gcount();

		if (read_bytes > 0)
		{
			errno = 0; // clear errno
			mq_send_return_value = mq_send(this->mqd, buffer.data(), read_bytes,
										   this->priority);
			if (mq_send_return_value == 0)
			{
				total_sent_bytes += read_bytes;
				this->timer.update_all();
			}
			else if (mq_send_return_value == -1 && errno == EAGAIN)
			{
				while (errno == EAGAIN && this->timer.get_duration() < this->timeout)
				{
					this->timer.update_end();
					errno = 0; // clear errno
					mq_send_return_value = mq_send(this->mqd, buffer.data(), read_bytes,
												   this->priority);
					if (mq_send_return_value == 0)
					{
						total_sent_bytes += read_bytes;
						this->timer.update_all();
					}
				}
				if (errno == EAGAIN && this->timer.get_duration() >= this->timeout)
					throw std::runtime_error(
						"ERROR: Timeout. Waited for client more than " + std::to_string(this->timeout) + " seconds.");
			}
			else
				throw std::runtime_error(std::string("ERROR: mq_send(): ") + strerror(errno));
		}
	}

	std::cout << "Sent data: " << total_sent_bytes << std::endl;

	if (total_sent_bytes == file_size)
	{
		this->timer.update_all();

		size_t is_empty = 0;
		size_t mq_oldmsgs = 0; // store the previous value of mq_curmsgs
		do
		{
			this->timer.update_end();
			mq_getattr(this->mqd, &(this->attr));
			is_empty = this->attr.mq_curmsgs;
			if (is_empty != mq_oldmsgs) // if client pick up msg, reset timer
				this->timer.update_begin();
			mq_oldmsgs = is_empty;
		} while (is_empty != 0 && (this->timer.get_duration() < this->timeout));

		if (is_empty != 0)
			throw std::runtime_error(
				"ERROR: Timeout. Waited for client more than " + std::to_string(this->timeout) + " seconds.");
		else
			std::cout << "Client picked it up." << std::endl;
	}
	else
	{
		throw std::runtime_error("ERROR: Connection lost. The transfer is interrupted.");
	}
}

