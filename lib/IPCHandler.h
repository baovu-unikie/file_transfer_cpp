/** @file IPCHandler.h
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#ifndef IPCHANDLER_H
#define IPCHANDLER_H

#include <iostream>
#include <vector>
#include "lib/IPC.h"
#include "lib/IPCMsgQSend.h"
#include "lib/IPCMsgQReceive.h"


const unsigned SHARED_MEM_LIMIT_IN_KB {10240};

std::vector<std::string> to_vector(int argc, char* argv[]);

class IPCHandler
{
	private:
		ipc_options_t opts{};

	public:
		void set_options(IPCMode m, std::vector<std::string> argv);
		void usage(bool is_exit) const;
		const ipc_options_t &get_options() const;
		IPCProtocol start() const;
};

#endif //IPCHANDLER_H
