/** @file IPCShmReceive.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#include "IPCShmReceive.h"

IPCShmReceive::~IPCShmReceive()
{
	shm_unlink(this->opts.server_name.c_str());
}

void IPCShmReceive::init()
{
	this->open_file();
	std::cout << "Waiting for sender..." << std::endl;
	for(unsigned tries = 0;;)
	{
		try {
			shm_obj_ptr = std::make_shared<shared_memory_object>(open_only, this->opts.server_name.c_str(), read_write);
			break;
		}
		catch (std::exception &e){
			sleep(1);
		}
		++tries;
		if(tries >= this->timeout)
			throw std::runtime_error("Tried to open shared memory more than " + std::to_string(this->timeout) + " second(s).");
	}

	for(unsigned tries = 0;;)
	{
		shm_region_ptr = std::make_shared<mapped_region>(*shm_obj_ptr, read_write);
		if(shm_region_ptr->get_address() != nullptr)
			break;

		std::cout << ++tries << std::endl;

		if(tries > this->timeout)
			throw std::runtime_error("Tried to open shared memory more than " + std::to_string(this->timeout) + " second(s).");
		sleep(1);
	}
	this->shm_ptr = (ipc_shm_header_t*) shm_region_ptr->get_address();
}

void IPCShmReceive::transfer()
{
	std::cout << "Receiving..." << std::endl;
	unsigned long total_received_bytes{0};

	this->timer.update_all();
	while (!this->shm_ptr->is_end && this->shm_ptr->is_init && this->timer.get_duration() < this->timeout)
	{
		{
			scoped_lock<interprocess_mutex> lock(this->shm_ptr->mutex, std::chrono::system_clock::now() + this->shm_timeout);
			if (!this->shm_ptr->msg_in)
				this->shm_ptr->cond_sent.wait_for(lock, this->shm_timeout);

			if (this->shm_ptr->shared_mem_size != this->shm_size_in_bytes)
				throw std::runtime_error("ERROR: Shared memory size of server and client side are not the same.");

			if(this->shm_ptr->msg_in)
			{
				if (this->shm_ptr->data_size > 0)
				{
					this->fs.write(this->shm_ptr->data_ap, this->shm_ptr->data_size);
					if (this->fs.bad()) // check read/writing error on i/o operation
						throw std::runtime_error("ERROR: ostream::write().");

					total_received_bytes += this->shm_ptr->data_size;
					this->timer.update_all();
				}
				else
					this->shm_ptr->is_end = true;

				this->shm_ptr->msg_in = false;
				this->shm_ptr->cond_received.notify_one();
			}
		}
		this->timer.update_end();
	}
	if (this->timer.get_duration() < this->timeout)
		std::cout << "Received data: " << total_received_bytes << " byte(s)" << std::endl;
	else
		throw std::runtime_error("ERROR: Timeout. Waited for more than " + std::to_string(this->timeout) + " second(s).");
}

void IPCShmReceive::open_shm()
{
}

