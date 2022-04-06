/** @file IPCMsgQSend.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief Include IPCMsgQSend class methods' definition
 */

#include "IPC.h"

IPCMsgQSend::~IPCMsgQSend()
{
	if (this->mqd > 0)
	{
		mq_close(this->mqd);
		std::cout << "Closed the message queue." << std::endl;
	}

	mq_unlink(this->opts.server_name.c_str());
	std::cout << "Unlinked the message queue name." << std::endl;
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
	this->ipc_info.file_size = this->get_file_size();
	if (this->ipc_info.file_size == 0)
		throw std::runtime_error("ERROR: File size = 0.");
	this->buffer.resize(this->attr.mq_msgsize);
	this->timer.update_all();
}

void IPCMsgQSend::print_members() const
{
	IPC::print_members();
	std::cout << "mqd: " << this->mqd << "\n"
			  << "attr.mq_flags: " << this->attr.mq_flags << "\n"
			  << "attr.mq_msgsize: " << this->attr.mq_msgsize << "\n"
			  << "attr.mq_maxmsg: " << this->attr.mq_maxmsg << "\n"
			  << "attr.mq_curmsgs: " << this->attr.mq_curmsgs << "\n"
			  << "mq_priority: " << this->mq_priority << std::endl;
}

void IPCMsgQSend::transfer()
{
	long mq_send_return_value{0};

	std::cout << "Sending..." << std::endl;
	while (!this->fs.eof())
	{
		this->read_file(buffer, this->attr.mq_msgsize);
		this->ipc_info.read_bytes = this->fs.gcount();

		if (this->ipc_info.read_bytes > 0)
		{
			errno = 0; // clear errno
			mq_send_return_value = mq_send(this->mqd, this->buffer.data(), this->ipc_info.read_bytes,
										   this->mq_priority);
			if (mq_send_return_value == 0)
			{
				this->ipc_info.total_sent_bytes += this->ipc_info.read_bytes;
				++(this->ipc_info.number_of_msg);
				this->timer.update_all();
			}
			else if (mq_send_return_value == -1 && errno == EAGAIN)
			{
				while (errno == EAGAIN && this->timer.get_duration() < this->timeout)
				{
					this->timer.update_end();
					errno = 0; // clear errno
					mq_send_return_value = mq_send(this->mqd, this->buffer.data(), this->ipc_info.read_bytes,
												   this->mq_priority);
					if (mq_send_return_value == 0)
					{
						this->ipc_info.total_sent_bytes += this->ipc_info.read_bytes;
						++(this->ipc_info.number_of_msg);
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

	std::cout << this->ipc_info.number_of_msg << " message(s) were sent to the queue (/dev/mqueue"
			  << this->opts.server_name << ")." << std::endl;

	if (this->ipc_info.total_sent_bytes == this->ipc_info.file_size)
	{
		this->timer.update_all();

		size_t is_empty = 0;
		size_t mq_oldmsgs = 0; // store the previous value of mq_curmsgs
		do
		{
			print_wait_msg("Waiting for client to pick up");
			this->timer.update_end();

			mq_getattr(this->mqd, &(this->attr));
			is_empty = this->attr.mq_curmsgs;
			if (is_empty != mq_oldmsgs) // if client pick up msg, reset timer
				this->timer.update_begin();
			mq_oldmsgs = is_empty;
			sleep(1);
		} while (is_empty != 0 && (this->timer.get_duration() < this->timeout));

		std::cout << std::endl;

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





