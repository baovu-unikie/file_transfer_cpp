/** @file IPCPipeReceive.h
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#ifndef IPCPIPERECEIVE_H
#define IPCPIPERECEIVE_H

#include <sys/stat.h>
#include <fcntl.h>
#include <climits>
#include "lib/IPC.h"
#include "lib/FileHandler.h"

class IPCPipeReceive: public IPC, public FileHandler
{
	protected:
		int pd{0};
		std::streamsize  p_msgsize{PIPE_BUF};
	public:
		explicit IPCPipeReceive(const ipc_options_t& options)
		: IPC(options),
		FileHandler(options.file_name, FileMode::WRITE) {};
		~IPCPipeReceive();
		void init() override;
		void transfer() override;

};


#endif //IPCPIPERECEIVE_H
