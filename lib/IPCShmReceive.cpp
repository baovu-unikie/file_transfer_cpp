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
	this->open_shm();
	this->map_shm();
}

void IPCShmReceive::transfer()
{
	this->timer.update_all();
	std::cout << "Receiving..." << std::endl;
	unsigned long total_received_bytes{0};
	while (!this->is_end && this->timer.get_duration() < this->timeout && this->shm_ptr->is_init)
	{
		lock_mutex();
		while (this->last_version == this->shm_ptr->data_version)
		{
			if (pthread_cond_wait(&(this->shm_ptr->cond), &(this->shm_ptr->mutex)) != 0)
				throw std::runtime_error(std::string("ERROR: pthread_cond_wait(): ") + strerror(errno));
		}
		if (this->shm_ptr->shared_mem_size != this->shm_size_in_bytes)
			throw std::runtime_error("ERROR: Shared memory size of server and client side are not the same.");

		if (this->shm_ptr->data_size != 0)
		{
			this->fs.write(this->shm_ptr->data_ap, this->shm_ptr->data_size);
			if (this->fs.bad()) // check read/writing error on i/o operation
				throw std::runtime_error("ERROR: ostream::write().");

			total_received_bytes += this->shm_ptr->data_size;
			this->timer.update_all();
		}
		else
			this->is_end = true;

		this->shm_ptr->is_read = true;
		this->last_version = this->shm_ptr->data_version;
		unlock_mutex();
		this->timer.update_end();
	}
	if (this->timer.get_duration() >= this->timeout)
		throw std::runtime_error(
			"ERROR: Timeout. Waited for server more than " + std::to_string(this->timeout) + " seconds.");

	std::cout << "Received data: " << total_received_bytes << " byte(s)" << std::endl;
}

void IPCShmReceive::map_shm()
{
	for (unsigned tries = 0;;)
	{
		this->shm_ptr = (ipc_shm_header_t *) mmap64(NULL, this->shm_size_in_bytes,
													PROT_READ | PROT_WRITE, MAP_SHARED,
													this->shmd, 0);
		if (this->shm_ptr != MAP_FAILED)
			break;

		++tries;

		if (tries >= this->timeout)
			throw std::runtime_error("ERROR: Waited for sender for more than " + std::to_string(this->timeout) + " second(s). mmap64(): " + strerror(errno));
		sleep(1);
	}
}

void IPCShmReceive::open_shm()
{
	std::cout << "Waiting for sender..." << std::endl;
	for (unsigned tries = 0;;)
	{
		this->shmd = shm_open(this->opts.server_name.c_str(), O_RDWR, 0660);
		if (this->shmd != -1)
			break;

		++tries;

		if (tries >= this->timeout)
			throw std::runtime_error("ERROR: Waited for sender for more than " + std::to_string(this->timeout) + " second(s). shm_open(): " + strerror(errno));

		sleep(1);
	}
}

