/** @file IPCShm.h
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#ifndef IPCSHM_H
#define IPCSHM_H

#include <iostream>
#include <pthread.h>
#include <unistd.h> // close()
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>

struct ipc_shm_header_t
{
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	std::streamsize data_size;
	size_t data_version;
	std::streamsize shared_mem_size;
	volatile bool is_init;
	volatile bool is_read;
	char data_ap[0]; // data access point - must always be the last member of the struct
};

class IPCShm
{
	protected:
		int shmd{};
		pthread_mutexattr_t mutex_attr{};
		pthread_condattr_t cond_attr{};
		ipc_shm_header_t *shm_ptr{};
		std::streamsize shm_size_in_bytes{};
		bool is_end{false};
	public:
		explicit IPCShm(std::streamsize size) : shm_size_in_bytes(size)
		{};
		~IPCShm();
		void init_cond();
		void init_mutex();
		void lock_mutex();
		void send_cond_broadcast();
		void set_shm_size() const;
		void unlock_mutex();
};


#endif //IPCSHM_H
