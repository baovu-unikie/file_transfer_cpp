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
	this->open_file();
	this->open_shm();
	this->set_shm_size();
	this->map_shm();
	this->init_mutex();
	this->init_cond();
}

void IPCShmSend::transfer()
{
	long read_bytes{0};
	unsigned long file_size{this->get_file_size()};
	if (file_size == 0)
		throw std::runtime_error("ERROR: File size = 0.");

	unsigned long total_sent_bytes{0};
	std::cout << "Waiting for receiver..." << std::endl;

	this->timer.update_all();
	while (!this->is_end && this->timer.get_duration() < this->timeout)
	{
		lock_mutex();
		if (this->shm_ptr->is_read)
		{
			if (!this->fs.eof())
			{
				this->fs.read(&(this->shm_ptr->data_ap[0]), static_cast<std::streamsize>(this->shm_ptr->data_size));
				if (this->fs.bad())
					throw std::runtime_error("ERROR: istream::read().");

				read_bytes = this->fs.gcount();
				if (read_bytes > 0)
				{
					this->shm_ptr->is_read = false;
					this->shm_ptr->data_version++;
					this->shm_ptr->data_size = read_bytes;
					total_sent_bytes += read_bytes;
				}
			}
			else
			{
				this->shm_ptr->is_read = false;
				this->shm_ptr->data_size = 0;
				this->shm_ptr->data_version++;
				this->is_end = true;
			}
			this->timer.update_all();
		}
		unlock_mutex();
		send_cond_broadcast();
		this->timer.update_end();
	}

	if (total_sent_bytes == file_size && this->is_end)
		std::cout << "Sent: " << total_sent_bytes << " byte(s)." << std::endl;
	else if (this->timer.get_duration() >= this->timeout)
		throw std::runtime_error(
			"ERROR: Timeout. Waited for client more than " + std::to_string(this->timeout) + " seconds.");
	else
		throw std::runtime_error("ERROR: Uncompleted transfer.");
}

void IPCShmSend::map_shm()
{
	this->shm_ptr = (ipc_shm_header_t *) mmap64(NULL, this->shm_size_in_bytes,
												PROT_READ | PROT_WRITE, MAP_SHARED,
												this->shmd, 0);
	if (this->shm_ptr == MAP_FAILED)
		throw std::runtime_error(static_cast<std::string>("ERROR: mmap64(): ") + strerror(errno));

	// set values for new share memory
	this->shm_ptr->is_init = true;
	this->shm_ptr->is_read = true;
	this->shm_ptr->shared_mem_size = this->shm_size_in_bytes;
	this->shm_ptr->data_size = this->shm_size_in_bytes - (std::streamsize) sizeof(ipc_shm_header_t);
}

void IPCShmSend::open_shm()
{
	// remove old shared memory (if any)
	shm_unlink(this->opts.server_name.c_str());
	errno = 0;
	this->shmd = shm_open(this->opts.server_name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0660);
	if (shmd == -1)
		throw std::runtime_error(static_cast<std::string>("ERROR: shm_open():") + strerror(errno));
}
