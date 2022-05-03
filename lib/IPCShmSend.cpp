/** @file IPCShmSend.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#include "IPCShmSend.h"

IPCShmSend::~IPCShmSend()
{
	shm_unlink(this->opts.server_name.c_str());
}

void IPCShmSend::init()
{
	// remove old shared memory (if any)
	shared_memory_object::remove(this->opts.server_name.c_str());
	// create shared memory segment
	shm_obj_ptr = std::make_shared<shared_memory_object>(create_only, this->opts.server_name.c_str(), read_write, 0660);
	// set size
	shm_obj_ptr->truncate(this->shm_size_in_bytes);
	// map memory
	shm_region_ptr = std::make_shared<mapped_region>(*shm_obj_ptr, read_write);

	// get shm_ptr
	this->shm_ptr = new (shm_region_ptr->get_address()) ipc_shm_header_t;
	this->shm_ptr->is_end = false;
	this->shm_ptr->is_init = true;
	this->shm_ptr->shared_mem_size = this->shm_size_in_bytes;
	this->shm_ptr->data_size = this->shm_size_in_bytes - (std::streamsize) sizeof(ipc_shm_header_t);
}

void IPCShmSend::transfer()
{
	long read_bytes{0};
	unsigned long file_size{this->get_file_size()};
	if (file_size == 0)
		throw std::runtime_error("ERROR: File size = 0.");
	this->open_file();

	unsigned long total_sent_bytes{0};
	std::cout << "Waiting for receiver..." << std::endl;

	this->shm_ptr->msg_in = false;

	std::cout << "Sending..." << std::endl;
	this->timer.update_all();
	while (!this->shm_ptr->is_end && this->timer.get_duration() < this->timeout)
	{
		{
			scoped_lock<interprocess_mutex> lock(this->shm_ptr->mutex, std::chrono::system_clock::now() + this->shm_timeout);
			if (this->shm_ptr->msg_in)
				this->shm_ptr->cond_received.wait_for(lock, this->shm_timeout);

			if(!this->shm_ptr->msg_in)
			{
				if (!this->fs.eof())
				{
					this->fs.read(&(this->shm_ptr->data_ap[0]), static_cast<std::streamsize>(this->shm_ptr->data_size));
					if (this->fs.bad())
						throw std::runtime_error("ERROR: istream::read().");

					read_bytes = this->fs.gcount();
					if (read_bytes > 0)
					{
						this->shm_ptr->data_size = read_bytes;
						total_sent_bytes += read_bytes;
						this->shm_ptr->msg_in = true;
						this->timer.update_all();
					}
				}
				else if (this->fs.eof())
				{
					this->shm_ptr->data_size = 0;
					this->shm_ptr->msg_in = true;
					this->timer.update_all();
				}
				this->shm_ptr->cond_sent.notify_one();
				this->shm_ptr->msg_in = true;
			}
		}
		this->timer.update_end();
	}

	if (total_sent_bytes == file_size && this->shm_ptr->is_end)
		std::cout << "Sent: " << total_sent_bytes << " byte(s)." << std::endl;
	else
		throw std::runtime_error("ERROR: Uncompleted transfer.");
}
