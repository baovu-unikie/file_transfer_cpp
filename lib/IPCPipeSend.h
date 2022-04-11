/** @file IPCPipeSend.h
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#ifndef IPCPIPESEND_H
#define IPCPIPESEND_H

#include <sys/stat.h>
#include <fcntl.h>
#include <climits>
#include "lib/IPC.h"
#include "lib/FileHandler.h"

class IPCPipeSend: public IPC, public FileHandler
{
	protected:
		int pd{0};
		std::streamsize  p_msgsize{PIPE_BUF};
	public:
		explicit IPCPipeSend(const ipc_options_t& options)
		: IPC(options),
		FileHandler(options.file_name, FileMode::READ) {};
		~IPCPipeSend();
		void init() override;
		void transfer() override;
};


#endif //IPCPIPESEND_H
