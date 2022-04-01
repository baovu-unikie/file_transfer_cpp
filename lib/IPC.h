/** @file IPC.h
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#ifndef IPC_H
#define IPC_H

#include <iostream>
#include <utility>
#include <vector>
#include "lib/ChronoTime.h"

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
	std::streamsize mem_size{0};
	std::string file_name;
	std::string server_name;
};

struct ipc_info_t
{
	long number_of_msg{};
	long read_bytes{};
	long sent_bytes{};
	unsigned long file_size{};
	unsigned long total_sent_bytes{};
};


class IPC
{
	protected:
		ipc_options_t opts{};
		ipc_info_t info{};
		long timeout{2};
		ChronoTime timer;
		std::vector<char> buffer{};

	public:
		explicit IPC(ipc_options_t options) : opts(std::move(options)){};
		virtual void init() = 0;
		virtual void transfer() = 0;
};


#endif //IPC_H
