/** @file IPC.h
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief Includes IPC class, all derived classes, non-member functions' prototype and common structs shared between classes
 */

#ifndef FILE_TRANSFER_CPP_IPC_H
#define FILE_TRANSFER_CPP_IPC_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <mqueue.h> // message queue
#include <boost/filesystem.hpp> // file_size
#include <climits>
#include <sys/stat.h> // mkfifo
#include <sys/types.h>
#include <pthread.h> // mutex, condvar, semaphore, shared mem
#include <sys/mman.h> // shm_unlink

#include "ChronoTime.h"

#define SHARED_MEM_LIMIT_IN_KB 10240

enum class IPCProtocol
{
	NONE, MSG_QUEUE, PIPE, SHARED_MEM
};

enum class IPCMode
{
	SEND_MODE, RECEIVE_MODE
};

struct ipc_options_t
{
	IPCMode mode{};
	IPCProtocol protocol{IPCProtocol::NONE};
	std::string server_name{};
	std::string file_name{};
	size_t mem_size{0};
};

struct ipc_info_t
{
	long number_of_msg{};
	long read_bytes{};
	long sent_bytes{};
	unsigned long file_size{};
	unsigned long total_sent_bytes{};
};

struct ipc_shm_header_t
{
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	size_t data_size;
	size_t data_version;
	size_t shared_mem_size;
	volatile bool is_init;
	volatile bool is_read;
	char data_ap[0]; // data access point - must always be the last member of the struct
};

ipc_options_t ipc_get_options(IPCMode mode, int argc, char *argv[]);
void ipc_start(ipc_options_t &options);
void ipc_usage(IPCMode mode, bool is_exit);
void ipc_validate_options(ipc_options_t &options);
void print_wait_msg(const std::string &msg);

class IPC
{
	protected:
		std::fstream fs{};
		ipc_options_t opts{};
		long timeout{10};
		std::vector<char> buffer{};
		ChronoTime timer;
		ipc_info_t ipc_info{};

	public:
		// Constructor
		explicit IPC(ipc_options_t &options) : opts(options)
		{};

		// Virtual destructor
		virtual  ~IPC()
		{
			fs.close();
			std::cout << "Closed file." << std::endl;
		};

		// Methods
		unsigned long get_file_size() const;

		virtual void init()
		{};
		virtual void print_members() const;

		virtual void transfer()
		{};
		void auto_start();
		void open_file();
		void write_to_file(const std::vector<char> &data, const std::streamsize &data_size);
		void read_file(std::vector<char> &data, std::streamsize &data_size);
};

class IPCMsgQSend : public IPC
{
	protected:
		mqd_t mqd{};
		struct mq_attr attr{0, 10, 8192, 0};
		unsigned mq_priority{MQ_PRIO_MAX - 1};

	public:
		// Constructor
		explicit IPCMsgQSend(ipc_options_t options) : IPC(options)
		{};
		virtual ~IPCMsgQSend() override;
		// Methods
		virtual void init() override;
		void print_members() const override;
		virtual void transfer() override;

};

class IPCMsgQReceive : public IPCMsgQSend
{
	public:
		// Constructor
		explicit IPCMsgQReceive(ipc_options_t &options) : IPCMsgQSend(options)
		{};
		// Methods
		void init() override;
		void transfer() override;
};

class IPCPipeSend : public IPC
{
	protected:
		int pd{0};
		std::streamsize p_msgsize{PIPE_BUF};
	public:
		// Constructor
		explicit IPCPipeSend(ipc_options_t options) : IPC(options)
		{};
		~IPCPipeSend() override;
		// Methods
		void cleanup() override;
		void init();
		void print_members() const override;
		void send();

};

class IPCPipeReceive : public IPCPipeSend
{
	public:
		// Constructor
		explicit IPCPipeReceive(ipc_options_t options) : IPCPipeSend(options)
		{};
		// Methods
		void init();
		void receive();
};

#endif //FILE_TRANSFER_CPP_IPC_H
