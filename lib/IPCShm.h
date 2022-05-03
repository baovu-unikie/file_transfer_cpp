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
#include <memory>
#include <chrono>

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/exceptions.hpp>

using boost::interprocess::interprocess_mutex;
using boost::interprocess::interprocess_condition;
using boost::interprocess::scoped_lock;
using boost::interprocess::shared_memory_object;
using boost::interprocess::create_only;
using boost::interprocess::open_only;
using boost::interprocess::read_write;
using boost::interprocess::mapped_region;
using boost::interprocess::interprocess_exception;


struct ipc_shm_header_t
{
	interprocess_mutex mutex;
	interprocess_condition cond_sent;
	interprocess_condition cond_received;
	std::streamsize data_size;
	std::streamsize shared_mem_size;
	volatile bool is_init;
	volatile bool msg_in;
	volatile bool is_end;
	char data_ap[0]; // data access point - must always be the last member of the struct
};

class IPCShm
{
	protected:
		ipc_shm_header_t *shm_ptr{};
		std::streamsize shm_size_in_bytes{};
		std::shared_ptr<shared_memory_object > shm_obj_ptr;
		std::shared_ptr<mapped_region> shm_region_ptr;
		std::chrono::milliseconds shm_timeout{5000};
	public:
		explicit IPCShm(std::streamsize size) : shm_size_in_bytes(size)
		{};
		~IPCShm();
};


#endif //IPCSHM_H
