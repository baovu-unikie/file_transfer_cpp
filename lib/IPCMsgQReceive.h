/** @file IPCMsgQReceive.h
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#ifndef IPCMSGQRECEIVE_H
#define IPCMSGQRECEIVE_H

#include <mqueue.h>
#include <climits>
#include <utility>
#include "lib/IPC.h"
#include "lib/FileHandler.h"

class IPCMsgQReceive : public IPC, public FileHandler
{
	protected:
		mqd_t mqd{};
		mq_attr attr{0, 10, 8192, 0};
		unsigned priority{MQ_PRIO_MAX - 1};
	public:
		explicit IPCMsgQReceive(const ipc_options_t& options)
			: IPC(options),
			  FileHandler(options.file_name, FileMode::WRITE){};
		~IPCMsgQReceive();
		void init() override;
		void transfer() override;
};


#endif //IPCMSGQRECEIVE_H
