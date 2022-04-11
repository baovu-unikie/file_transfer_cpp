/** @file test.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <boost/process.hpp>
#include "test/IPCHandlerTestSuite.h"
#include "test/ChronoTimeTestSuite.h"
#include "lib/FileHandler.h"
#include "test/dummy_file_generator.h"

TEST_P(IPCHandlerTestSuite, SetOptions)
{
	std::stringstream buffer;
	auto sbuf = std::cout.rdbuf();

	if (is_throw)
		EXPECT_THROW(ipc_handler.set_options(mode, argv), std::runtime_error);
	else
	{
		std::cout.rdbuf(buffer.rdbuf());
		ipc_handler.set_options(mode, argv);
		std::cout.rdbuf(sbuf);
		ipc_options_t opts = ipc_handler.get_options();
		EXPECT_EQ(opts.mode, mode);

		if (protocol == IPCProtocol::NONE)
		{
			if (opts.mode == IPCMode::SEND_MODE)
			{
				std::string expected = "===========================================================================\n";
				expected += "SYNOPSIS\n";
				expected += "  ipc_send -h | --help\n";
				expected += "  ipc_send PROTOCOL --file FILE_PATH\n";
				expected += "  ipc_send PROTOCOL -f FILE_PATH\n";
				expected += "DESCRIPTION\n";
				expected += "  ipc_send: sends a file using a specified protocol.\n";
				expected += "  Available PROTOCOL\n";
				expected += "    -m | --message SERVER_NAME.\n";
				expected += "    -q | --queue QUEUE_NAME.\n";
				expected += "                 QUEUE_NAME should be started with '/' character.\n";
				expected += "    -p | --pipe PIPE_NAME\n";
				expected += "    -s | --shm SHARED_MEM_NAME SIZE_OF_SHM_IN_KB\n";
				expected += "               SHARED_MEM_NAME should be started with '/' character.\n";
				expected += "               0 < SIZE_OF_SHM_IN_KB ≤ 10240\n";
				expected += "EXAMPLES\n";
				expected += "  ipc_send -f file_to_send -q /queue_name\n";
				expected += "  ipc_send -f file_to_send -p pipe_name\n";
				expected += "  ipc_send -f file_to_send -s /shm_name 4\n";
				expected += "===========================================================================\n";
				EXPECT_EQ(buffer.str(), expected);
			}
			else if (opts.mode == IPCMode::RECEIVE_MODE)
			{
				std::string expected = "===========================================================================\n";
				expected += "SYNOPSIS\n";
				expected += "  ipc_receive -h | --help\n";
				expected += "  ipc_receive PROTOCOL --file FILE_PATH\n";
				expected += "  ipc_receive PROTOCOL -f FILE_PATH\n";
				expected += "DESCRIPTION\n";
				expected += "  ipc_receive: receives file using a specified protocol\n";
				expected += "  Available PROTOCOL\n";
				expected += "    -m | --message SERVER_NAME.\n";
				expected += "    -q | --queue QUEUE_NAME.\n";
				expected += "                 QUEUE_NAME should be started with '/' character.\n";
				expected += "    -p | --pipe PIPE_NAME\n";
				expected += "    -s | --shm SHARED_MEM_NAME SIZE_OF_SHM_IN_KB\n";
				expected += "               SHARED_MEM_NAME should be started with '/' character.\n";
				expected += "               0 < SIZE_OF_SHM_IN_KB ≤ 10240\n";
				expected += "EXAMPLES\n";
				expected += "  ipc_receive -f file_name -q /queue_name\n";
				expected += "  ipc_receive -f file_name -p pipe_name\n";
				expected += "  ipc_receive -f file_name -s /shm_name 4\n";
				expected += "===========================================================================\n";
				EXPECT_EQ(buffer.str(), expected);
			}
		}
		else
		{
			EXPECT_EQ(opts.protocol, protocol);
			EXPECT_EQ(opts.mem_size, mem_size);
			EXPECT_EQ(opts.file_name, file_name);
			EXPECT_EQ(opts.server_name, server_name);
		}
	}
}

TEST_P(IPCHandlerTestSuite, ToVector)
{
	// construct char *arr[] to use as args of to_vector()
	char *arr[argv.size()];
	for (unsigned i{0}; i < argv.size(); i++)
		arr[i] = new char[argv.at(i).size() + 1];
	for (unsigned i{0}; i < argv.size(); i++)
		std::strcpy(arr[i], argv.at(i).c_str());

	std::vector<std::string> result = to_vector((int) argv.size(), arr);

	// test
	for (unsigned i{0}; i < argv.size(); i++)
		EXPECT_STREQ(arr[i], result.at(i).c_str());

	// free memory
	for (unsigned i{0}; i < argv.size(); i++)
		delete[] arr[i];
}

TEST_P(IPCHandlerTestSuite, IPCStart)
{
	if (!is_throw)
	{
		boost::process::child c;

		if(mode == IPCMode::SEND_MODE && file_size == 0)
		{
			ipc_handler.set_options(mode, argv);
			EXPECT_THROW(ipc_handler.start(), std::runtime_error);
		}
		else if (mode == IPCMode::RECEIVE_MODE && protocol != IPCProtocol::NONE)
		{
			std::string send_file_name = "tx_file";
			generate_dummy_files(send_file_name, file_size, file_unit);

			if (protocol == IPCProtocol::PIPE)
				c = boost::process::child("data/ipc_send", "-p", server_name.c_str(), "-f", send_file_name.c_str());
			if (protocol == IPCProtocol::MSG_QUEUE)
				c = boost::process::child("data/ipc_send", "-q", server_name.c_str(), "-f", send_file_name.c_str());
			if (protocol == IPCProtocol::SHARED_MEM)
				c = boost::process::child("data/ipc_send", "-s", server_name.c_str(), std::to_string(mem_size).c_str(),
										  "-f", send_file_name.c_str());

			ipc_handler.set_options(mode, argv);
			ipc_handler.start();
			c.wait();
			EXPECT_EQ(boost::filesystem::file_size(send_file_name), boost::filesystem::file_size(file_name));
		}
		else if (mode == IPCMode::SEND_MODE && protocol != IPCProtocol::NONE)
		{
			std::string rcv_file_name = "rx_file";
			generate_dummy_files(file_name, file_size, file_unit);

			if (protocol == IPCProtocol::PIPE)
				c = boost::process::child("data/ipc_receive", "-p", server_name.c_str(), "-f", rcv_file_name.c_str());
			if (protocol == IPCProtocol::MSG_QUEUE)
				c = boost::process::child("data/ipc_receive", "-q", server_name.c_str(), "-f", rcv_file_name.c_str());
			if (protocol == IPCProtocol::SHARED_MEM)
				c = boost::process::child("data/ipc_receive", "-s", server_name.c_str(),
										  std::to_string(mem_size).c_str(), "-f", rcv_file_name.c_str());

			ipc_handler.set_options(mode, argv);
			ipc_handler.start();
			c.wait();
			EXPECT_EQ(boost::filesystem::file_size(rcv_file_name), boost::filesystem::file_size(file_name));
		}
	}
}

TEST_P(IPCHandlerTestSuite, IPCStartNoSender)
{
	if (!is_throw && mode == IPCMode::RECEIVE_MODE && protocol != IPCProtocol::NONE)
	{
		std::string send_file_name = "tx_file";
		generate_dummy_files(send_file_name, file_size, file_unit);
		ipc_handler.set_options(mode, argv);
		EXPECT_THROW(ipc_handler.start(), std::runtime_error);
	}
}

TEST_P(IPCHandlerTestSuite, IPCStartNoReceiver)
{
	if (!is_throw && mode == IPCMode::SEND_MODE && protocol != IPCProtocol::NONE)
	{
		std::string rcv_file_name = "rx_file";
		generate_dummy_files(file_name, file_size, file_unit);
		ipc_handler.set_options(mode, argv);
		EXPECT_THROW(ipc_handler.start(), std::runtime_error);
	}
}

TEST_P(ChronoTimeTestSuite, Timer)
{
	if (test_name == "UpdateAll")
		timer.update_all();
	else
	{
		timer.update_begin();
		sleep(expected_duration);
		timer.update_end();
	}
	EXPECT_EQ(expected_duration, timer.get_duration());
}

TEST(FileHandlerTestSuite, FileOperation)
{
	std::streamsize str_size{4};
	std::string file_name_read{"data/readTestFile"};
	std::string file_not_exist{"notExistFileName"};
	std::string file_name_write{"writeTestFile"};
	std::vector<char> read_str{};
	read_str.resize(str_size);
	std::vector<char> write_str{'T', 'h', 'a', 't'};

	FileHandler f_read(file_name_read, FileMode::READ);
	f_read.open_file();
	EXPECT_NO_THROW(f_read.read_file(read_str, str_size));
	EXPECT_THROW(f_read.write_file(write_str, str_size), std::runtime_error);

	FileHandler f_write(file_name_write, FileMode::WRITE);
	f_write.open_file();
	EXPECT_NO_THROW(f_write.write_file(write_str, str_size));
	EXPECT_THROW(f_write.read_file(read_str, str_size), std::runtime_error);
	EXPECT_EQ(f_write.get_file_size(), str_size);

	FileHandler f_not_exist(file_not_exist, FileMode::READ);
	EXPECT_THROW(f_not_exist.open_file(), std::runtime_error);

}

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
