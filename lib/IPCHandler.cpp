/** @file IPCHandler.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#include "lib/IPCHandler.h"

void IPCHandler::set_options(IPCMode m, std::vector<std::string> argv)
{
	this->opts.mode = m;
	size_t arg_list_size = argv.size();

	if (arg_list_size != 5 && arg_list_size != 2 && arg_list_size != 6)
		usage(true);

	for (size_t i{1}; i < arg_list_size; i++)
	{
		if ((argv.at(i) == "--help" || argv.at(i) == "-h") && opts.protocol == IPCProtocol::NONE)
		{
			usage(false);
			break;
		}
		else if (i + 1 == arg_list_size)
			usage(true);
			/* opts with one argument */
		else if ((argv.at(i) == "--queue" || argv.at(i) == "-q") && opts.protocol == IPCProtocol::NONE)
		{
			opts.protocol = IPCProtocol::MSG_QUEUE;
			opts.server_name = argv.at(++i);
		}
		else if ((argv.at(i) == "--pipe" || argv.at(i) == "-p") && opts.protocol == IPCProtocol::NONE)
		{
			opts.protocol = IPCProtocol::PIPE;
			opts.server_name = argv.at(++i);
		}

		else if (argv.at(i) == "--file" || argv.at(i) == "-f")
			opts.file_name = argv.at(++i);
			/* opts with two arguments */
		else if (i + 2 == arg_list_size)
			usage(true);
		else if ((argv.at(i) == "--shm" || argv.at(i) == "-s") && opts.protocol == IPCProtocol::NONE)
		{
			opts.protocol = IPCProtocol::SHARED_MEM;
			opts.server_name = argv.at(++i);
			try
			{
				opts.mem_size = stoi(argv.at(++i));
			} catch (const std::invalid_argument &ia)
			{
				std::cout << "ERROR: Invalid argument. Memory size should be an integer." << std::endl;
				usage(true);
			}
		}
		else
			usage(true);
	}

	unsigned number_of_slashes{0};
	for (auto c: opts.server_name)
		if (c == '/')
			number_of_slashes++;

	if (opts.protocol == IPCProtocol::MSG_QUEUE &&
		(opts.server_name.at(0) != '/' || opts.server_name.size() == 1 || number_of_slashes > 1))
	{
		throw std::runtime_error("ERROR: Invalid server name. ");
	}
	if (opts.protocol == IPCProtocol::SHARED_MEM && (opts.mem_size <= 0 || opts.mem_size > SHARED_MEM_LIMIT_IN_KB))
	{
		throw std::runtime_error(std::string("ERROR: Invalid memory size. It should be in range [1,") +
								 std::to_string(SHARED_MEM_LIMIT_IN_KB) + "].");
	}
	if (opts.protocol == IPCProtocol::SHARED_MEM && number_of_slashes > 0)
	{
		throw std::runtime_error("ERROR: Invalid server name. Shared memory name should not include any slash.");
	}
}

void IPCHandler::usage(bool is_exit) const
{
	std::string cmd{}, description{}, example1{}, example2{}, example3{};
	if (this->opts.mode == IPCMode::SEND_MODE)
	{
		cmd = "ipc_send";
		description = "sends a file using a specified protocol.";
		example1 = "ipc_send -f file_to_send -q /queue_name";
		example2 = "ipc_send -f file_to_send -p pipe_name";
		example3 = "ipc_send -f file_to_send -s shm_name 4";
	}
	if (this->opts.mode == IPCMode::RECEIVE_MODE)
	{
		cmd = "ipc_receive";
		description = "receives file using a specified protocol";
		example1 = "ipc_receive -f file_name -q /queue_name";
		example2 = "ipc_receive -f file_name -p pipe_name";
		example3 = "ipc_receive -f file_name -s shm_name 4";
	}

	std::cout << "===========================================================================\n"
			  << "SYNOPSIS\n"
			  << "  " << cmd << " -h | --help\n"
			  << "  " << cmd << " PROTOCOL --file FILE_PATH\n"
			  << "  " << cmd << " PROTOCOL -f FILE_PATH\n"
			  << "DESCRIPTION\n"
			  << "  " << cmd << ": " << description << "\n"
			  << "  Available PROTOCOL\n"
			  << "    -m | --message SERVER_NAME.\n"
			  << "    -q | --queue QUEUE_NAME.\n"
			  << "                 QUEUE_NAME should be started with '/' character.\n"
			  << "    -p | --pipe PIPE_NAME\n"
			  << "    -s | --shm SHARED_MEM_NAME SIZE_OF_SHM_IN_KB\n"
			  << "               0 < SIZE_OF_SHM_IN_KB ≤ " << SHARED_MEM_LIMIT_IN_KB << "\n"
			  << "EXAMPLES\n"
			  << "  " << example1 << "\n"
			  << "  " << example2 << "\n"
			  << "  " << example3 << std::endl
			  << "===========================================================================\n";

	if (is_exit)
		throw std::runtime_error("Invalid argument.");
}

const ipc_options_t &IPCHandler::get_options() const
{
	return opts;
}

IPCProtocol IPCHandler::start() const
{

	if (opts.mode == IPCMode::RECEIVE_MODE)
	{
		if (opts.protocol == IPCProtocol::MSG_QUEUE)
		{
			IPCMsgQReceive mq(this->opts);
			mq.init();
			mq.transfer();
		}
		else if (opts.protocol == IPCProtocol::PIPE)
		{
			IPCPipeReceive pipe(this->opts);
			pipe.init();
			pipe.transfer();
		}
		else if (opts.protocol == IPCProtocol::SHARED_MEM)
		{
			IPCShmReceive shm(this->opts);
			shm.init();
			shm.transfer();
		}
	}
	else if (opts.mode == IPCMode::SEND_MODE)
	{
		if (opts.protocol == IPCProtocol::MSG_QUEUE)
		{
			IPCMsgQSend mq(this->opts);
			mq.init();
			mq.transfer();
		}
		else if (opts.protocol == IPCProtocol::PIPE)
		{
			IPCPipeSend pipe(this->opts);
			pipe.init();
			pipe.transfer();
		}
		else if (opts.protocol == IPCProtocol::SHARED_MEM)
		{
			IPCShmSend shm(this->opts);
			shm.init();
			shm.transfer();
		}
	}
	return opts.protocol;
}

std::vector<std::string> to_vector(int argc, char *argv[])
{
	std::vector<std::string> vec;
	for (int i{0}; i < argc; i++)
		vec.emplace_back(argv[i]);
	return vec;
}
