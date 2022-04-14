/** @file IPCShmSend.h
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#ifndef IPCSHMSEND_H
#define IPCSHMSEND_H

#include "lib/IPC.h"
#include "lib/FileHandler.h"
#include "lib/IPCShm.h"

class IPCShmSend: public IPC, public FileHandler, public IPCShm
{
	protected:
	public:
		explicit IPCShmSend(const ipc_options_t& options)
		: IPC(options),
		FileHandler(options.file_name, FileMode::READ),
		IPCShm(options.mem_size*1024){};
		~IPCShmSend();
		void init() override;
		void transfer() override;
		void open_shm();
		void map_shm();
};

#endif //IPCSHMSEND_H
