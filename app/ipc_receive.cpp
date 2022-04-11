/** @file ipc_receive.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */
 
#include "lib/IPCHandler.h"

int main(int argc, char *argv[])
{
	try{
		IPCHandler ipc;
		ipc.set_options(IPCMode::RECEIVE_MODE, to_vector(argc, argv));
		ipc.start();
	}
	catch (const std::exception &e)
	{
		std::rethrow_exception(std::current_exception());
	}

	return 0;
}