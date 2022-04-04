/** @file IPC.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief Include IPC class methods' definition & non-member functions' definition
 */

#include "lib/IPC.h"

/******************************************************************************/
/* IPC Class's methods ********************************************************/
/******************************************************************************/

unsigned long IPC::get_file_size() const
{
	return boost::filesystem::file_size(this->opts.file_name);
}

void IPC::cleanup()
{
	std::cout << "Closed file." << std::endl;
	fs.close();
}

void IPC::print_members() const
{
	std::cout << "Mode: " << static_cast<std::underlying_type<IPCMode>::type>(opts.mode) << "\n"
			  << "Protocol: " << static_cast<std::underlying_type<IPCProtocol>::type>(opts.protocol) << "\n"
			  << "server_name: " << opts.server_name << "\n"
			  << "file_name: " << opts.file_name << "\n"
			  << "mem_size: " << opts.mem_size << "\n"
			  << "timeout: " << timeout << "\n";
}

void IPC::auto_start()
{
	this->init();
	if (this->opts.mode == IPCMode::SEND_MODE)
		this->send();
	if (this->opts.mode == IPCMode::RECEIVE_MODE)
		this->receive();
}

void IPC::open_file()
{
	if (this->opts.mode == IPCMode::SEND_MODE)
		this->fs.open(opts.file_name, std::fstream::in | std::fstream::binary);
	if (this->opts.mode == IPCMode::RECEIVE_MODE)
		this->fs.open(opts.file_name, std::fstream::out | std::fstream::trunc | std::fstream::binary);

	if (this->fs.fail()) // Check logical error on I/O operation (close/open)
		throw std::runtime_error("ERROR: " + this->opts.file_name + ": " + strerror(errno));
}

void IPC::write_to_file(std::vector<char> &data, std::streamsize &data_size)
{
	this->fs.write(data.data(), data_size);
	if (this->fs.bad()) // check read/writing error on i/o operation
		throw std::runtime_error("ERROR: ostream::write().");
}


/******************************************************************************/
/* Non-member function definitions ********************************************/
/******************************************************************************/

ipc_options_t *ipc_get_options(IPCMode mode, int argc, char *argv[])
{
	auto *options = new ipc_options_t;
	std::vector<std::string> arg_list{};
	size_t arg_list_size{0};

	options->mode = mode;

	for (int i{1}; i < argc; i++)
	{
		arg_list.emplace_back(argv[i]);
	}
	arg_list_size = arg_list.size();

	if (arg_list_size != 4 && arg_list_size != 1 && arg_list_size != 5)
	{
		ipc_usage(mode, true);
	}

	for (size_t i{0}; i < arg_list_size; i++)
	{
		if ((arg_list.at(i) == "--help" || arg_list.at(i) == "-h") && options->protocol == IPCProtocol::NONE)
		{
			ipc_usage(mode, false);
			exit(EXIT_SUCCESS);
		}
		else if (i + 1 == arg_list_size)
		{
			ipc_usage(mode, true);
		}
			/* options with one argument */
		else if ((arg_list.at(i) == "--queue" || arg_list.at(i) == "-q") && options->protocol == IPCProtocol::NONE)
		{
			options->protocol = IPCProtocol::MSG_QUEUE;
			options->server_name = arg_list.at(++i);
		}
		else if ((arg_list.at(i) == "--pipe" || arg_list.at(i) == "-p") && options->protocol == IPCProtocol::NONE)
		{
			options->protocol = IPCProtocol::PIPE;
			options->server_name = arg_list.at(++i);
		}

		else if (arg_list.at(i) == "--file" || arg_list.at(i) == "-f")
		{
			options->file_name = arg_list.at(++i);
		}
			/* options with two arguments */
		else if (i + 2 == arg_list_size)
		{
			ipc_usage(mode, true);
		}
		else if ((arg_list.at(i) == "--shm" || arg_list.at(i) == "-s") && options->protocol == IPCProtocol::NONE)
		{
			options->protocol = IPCProtocol::SHARED_MEM;
			options->server_name = arg_list.at(++i);
			try
			{
				options->mem_size = stoi(arg_list.at(++i));
			} catch (const std::invalid_argument &ia)
			{
				std::cout << "ERROR: Invalid argument. Memory size should be an integer." << std::endl;
				ipc_usage(mode, true);
			}
		}
		else
		{
			ipc_usage(mode, true);
		}
	}

	ipc_validate_options(*options);
	return options;
}

void ipc_start(ipc_options_t &options)
{
	if (options.mode == IPCMode::RECEIVE_MODE)
	{
		if (options.protocol == IPCProtocol::MSG_QUEUE)
		{
			IPCMsgQReceive mq{options};
			mq.print_members();
			mq.auto_start();
		}
		else if (options.protocol == IPCProtocol::PIPE)
		{
			std::cout << "PIPE: Not implemented yet." << std::endl;
		}
		else if (options.protocol == IPCProtocol::SHARED_MEM)
		{
			std::cout << "SHARD_MEM: Not implemented yet." << std::endl;
		}
	}
	else if (options.mode == IPCMode::SEND_MODE)
	{
		if (options.protocol == IPCProtocol::MSG_QUEUE)
		{
			IPCMsgQSend mq{options};
			mq.print_members();
			mq.auto_start();
		}
		else if (options.protocol == IPCProtocol::PIPE)
		{
			std::cout << "PIPE: Not implemented yet." << std::endl;
		}
		else if (options.protocol == IPCProtocol::SHARED_MEM)
		{
			std::cout << "SHARD_MEM: Not implemented yet." << std::endl;
		}
	}
}

void ipc_usage(IPCMode mode, bool is_exit)
{
	std::string cmd{}, description{}, example1{}, example2{}, example3{};
	if (mode == IPCMode::SEND_MODE)
	{
		cmd = "ipc_send";
		description = "sends a file using a specified protocol.";
		example1 = "ipc_send -f file_to_send -q /queue_name";
		example2 = "ipc_send -f file_to_send -p pipe_name";
		example3 = "ipc_send -f file_to_send -s /shm_name 4";
	}
	if (mode == IPCMode::RECEIVE_MODE)
	{
		cmd = "ipc_receive";
		description = "receives file using a specified protocol";
		example1 = "ipc_receive -f file_name -q /queue_name";
		example2 = "ipc_receive -f file_name -p pipe_name";
		example3 = "ipc_receive -f file_name -s /shm_name 4";
	}

	std::cout << "===========================================================================\n"
			  << "SYNOPSIS\n"
			  << "  " << cmd << " -h | --help\n"
			  << "  " << cmd << " PROTOCOL --file FILE_PATH \n"
			  << "  " << cmd << " PROTOCOL -f FILE_PATH \n"
			  << "DESCRIPTION\n"
			  << "  " << cmd << ": " << description << "\n"
			  << "  Available PROTOCOL\n"
			  << "    -m | --message SERVER_NAME.\n"
			  << "    -q | --queue QUEUE_NAME.\n"
			  << "                 QUEUE_NAME should be started with '/' character.\n"
			  << "    -p | --pipe PIPE_NAME\n"
			  << "    -s | --shm SHARED_MEM_NAME SIZE_OF_SHM_IN_KB\n"
			  << "               SHARED_MEM_NAME should be started with '/' character.\n"
			  << "               0 < SIZE_OF_SHM_IN_KB ≤ " << SHARED_MEM_LIMIT_IN_KB << "\n"
			  << "EXAMPLES\n"
			  << "  " << example1 << "\n"
			  << "  " << example2 << "\n"
			  << "  " << example3 << std::endl
			  << "===========================================================================\n";

	if (is_exit)
	{
		throw std::runtime_error("Invalid argument.");
	}

}

void ipc_validate_options(ipc_options_t &options)
{
	bool is_valid{true};
	if (options.protocol == IPCProtocol::MSG_QUEUE || options.protocol == IPCProtocol::SHARED_MEM)
	{
		if (options.server_name.at(0) != '/' || options.server_name.size() == 1)
		{
			std::cout << "ERROR: Invalid server name. " << std::endl;
			is_valid = false;
		}

		if (options.protocol == IPCProtocol::SHARED_MEM &&
			(options.mem_size == 0 || options.mem_size > SHARED_MEM_LIMIT_IN_KB))
		{
			std::cout << "ERROR: Invalid memory size. It should be in range [1,"
					  << SHARED_MEM_LIMIT_IN_KB << "]." << std::endl;
			is_valid = false;
		}
	}

	if (!is_valid)
		ipc_usage(options.mode, true);
}

void print_wait_msg(const std::string &msg)
{
	static unsigned number_of_dots{0};
	std::string dots{"..."};
	std::cout << "\r" << std::flush;
	std::cout << msg << dots.substr(0, number_of_dots) << std::flush;
	number_of_dots++;
	if (number_of_dots > 3) number_of_dots = 0;
}

