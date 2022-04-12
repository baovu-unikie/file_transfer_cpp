/** @file IPCMsgQSend.h
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#ifndef IPCMSGQSEND_H
#define IPCMSGQSEND_H

#include <mqueue.h>
#include <climits> // MQ_PRIO_MAX
#include "IPC.h"
#include "FileHandler.h"

class IPCMsgQSend : public IPC, public FileHandler
{
	protected:
		mqd_t mqd{};
		mq_attr attr{0, 10, 8192, 0};
		unsigned priority{MQ_PRIO_MAX - 1};
	public:
		explicit IPCMsgQSend(const ipc_options_t &options)
			: IPC(options),
			  FileHandler(options.file_name, FileMode::READ)
		{};
		~IPCMsgQSend();
		void init() override;
		void transfer() override;
};

#endif //IPCMSGQSEND_H
