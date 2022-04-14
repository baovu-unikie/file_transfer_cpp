/** @file IPCShmReceive.h
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#ifndef IPCSHMRECEIVE_H
#define IPCSHMRECEIVE_H

#include "lib/IPC.h"
#include "lib/FileHandler.h"
#include "lib/IPCShm.h"

class IPCShmReceive: public IPC, public FileHandler, public IPCShm
{
	protected:
		unsigned number_of_tries {10};
		size_t last_version{0};
	public:
		explicit IPCShmReceive(const ipc_options_t& options)
		: IPC(options),
			FileHandler(options.file_name, FileMode::WRITE),
			IPCShm(options.mem_size*1024){};
		~IPCShmReceive();
		void init() override;
		void map_shm();
		void open_shm();
		void transfer() override;
};


#endif //IPCSHMRECEIVE_H
