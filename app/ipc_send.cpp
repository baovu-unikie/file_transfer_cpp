/** @file ipc_send.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief Sends file using a specified IPC method.
 */

#include "../lib/IPC.h"

int main(int argc, char *argv[])
{
	try{

		ipc_options_t options = ipc_get_options(IPCMode::SEND_MODE, argc, argv);
		ipc_start(options);
	}
	catch (const std::exception &e)
	{
		std::rethrow_exception(std::current_exception());
	}
	return 0;
}
