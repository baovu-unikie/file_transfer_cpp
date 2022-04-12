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
			std::string send_file_name = "tx_file_name";
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
			std::string rcv_file_name = "rx_file_name";
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
	f_write.close_file();
	EXPECT_EQ(f_write.get_file_size(), str_size);

	FileHandler f_not_exist(file_not_exist, FileMode::READ);
	EXPECT_THROW(f_not_exist.open_file(), std::runtime_error);

}

INSTANTIATE_TEST_SUITE_P
(, IPCHandlerTestSuite, testing::Values(
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>({"SendPipeShort", "-p", "my_pipe", "-f", "file_name"}),
					IPCProtocol::PIPE, 0, "file_name", "my_pipe", false, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>({"SendPipeShortBigFile", "-p", "my_pipe", "-f", "file_name"}),
					IPCProtocol::PIPE, 0, "file_name", "my_pipe", false, 1, "GB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>({"SendPipeShortFileSizeZero", "-p", "my_pipe", "-f", "file_name"}),
					IPCProtocol::PIPE, 0, "file_name", "my_pipe", false, 0, "MB"),
	std::make_tuple(IPCMode::SEND_MODE,
					std::vector<std::string>({"SendMsgQShort", "-q", "/my_queue", "-f", "file_name"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", false, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE,
					std::vector<std::string>({"SendMsgQShortFileSizeZero", "-q", "/my_queue", "-f", "file_name"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", false, 0, "MB"),
	std::make_tuple(IPCMode::SEND_MODE,
					std::vector<std::string>({"SendMsgQShortBigFile", "-q", "/my_queue", "-f", "file_name"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", false, 1, "GB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendPipeShortExtraEnd", "-p", "my_pipe", "-f", "file_name", "extra_arg"}), IPCProtocol::PIPE, 0, "file_name",
					"my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendMsgQShortExtraEnd", "-q", "/my_queue", "-f", "file_name", "extra_arg"}), IPCProtocol::MSG_QUEUE, 0,
					"file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendPipeShortExtraMid", "-p", "my_pipe", "extra_arg", "-f", "file_name"}), IPCProtocol::PIPE, 0, "file_name",
					"my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendMsgQShortExtraMid", "-q", "/my_queue", "extra_arg", "-f", "file_name"}), IPCProtocol::MSG_QUEUE, 0,
					"file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendPipeShortExtraStart", "extra_arg", "-p", "my_pipe", "-f", "file_name"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendMsgQShortExtraStart", "extra_arg", "-q", "/my_queue", "-f", "file_name"}), IPCProtocol::MSG_QUEUE, 0,
					"file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE,
					std::vector<std::string>({"SendPipeShortInverted", "-f", "file_name", "-p", "my_pipe"}),
					IPCProtocol::PIPE, 0, "file_name", "my_pipe", false, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE,
					std::vector<std::string>({"SendMsgQShortInverted", "-f", "file_name", "-q", "/my_queue"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", false, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendPipeShortExtraEndInverted", "-f", "file_name", "extra_arg", "-p", "my_pipe"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendMsgQShortExtraEndInverted", "-f", "file_name", "extra_arg", "-q", "/my_queue"}), IPCProtocol::MSG_QUEUE,
					0, "file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendPipeShortExtraMidInverted", "-f", "file_name", "extra_arg", "-p", "my_pipe"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendMsgQShortExtraMidInverted", "-f", "file_name", "extra_arg", "-q", "/my_queue"}), IPCProtocol::MSG_QUEUE,
					0, "file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendPipeShortExtraStartInverted", "extra_arg", "-f", "file_name", "-p", "my_pipe"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendMsgQShortExtraStartInverted", "extra_arg", "-f", "file_name", "-q", "/my_queue"}), IPCProtocol::MSG_QUEUE,
					0, "file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE,
					std::vector<std::string>({"SendPipeLong", "--pipe", "my_pipe", "-f", "file_name"}),
					IPCProtocol::PIPE, 0, "file_name", "my_pipe", false, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE,
					std::vector<std::string>({"SendMsgQLong", "--queue", "/my_queue", "-f", "file_name"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", false, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendPipeLongExtraEnd", "--pipe", "my_pipe", "-f", "file_name", "extra_arg"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendMsgQLongExtraEnd", "--queue", "/my_queue", "-f", "file_name", "extra_arg"}), IPCProtocol::MSG_QUEUE, 0,
					"file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendPipeLongExtraMid", "--pipe", "my_pipe", "extra_arg", "-f", "file_name"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendMsgQLongExtraMid", "--queue", "/my_queue", "extra_arg", "-f", "file_name"}), IPCProtocol::MSG_QUEUE, 0,
					"file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendPipeLongExtraStart", "extra_arg", "--pipe", "my_pipe", "-f", "file_name"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendMsgQLongExtraStart", "extra_arg", "--queue", "/my_queue", "-f", "file_name"}), IPCProtocol::MSG_QUEUE, 0,
					"file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE,
					std::vector<std::string>({"SendPipeLongInverted", "-f", "file_name", "--pipe", "my_pipe"}),
					IPCProtocol::PIPE, 0, "file_name", "my_pipe", false, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE,
					std::vector<std::string>({"SendMsgQLongInverted", "-f", "file_name", "--queue", "/my_queue"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", false, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendPipeLongExtraEndInverted", "-f", "file_name", "extra_arg", "--pipe", "my_pipe"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendMsgQLongExtraEndInverted", "-f", "file_name", "extra_arg", "--queue", "/my_queue"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendPipeLongExtraMidInverted", "-f", "file_name", "extra_arg", "--pipe", "my_pipe"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendMsgQLongExtraMidInverted", "-f", "file_name", "extra_arg", "--queue", "/my_queue"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendPipeLongExtraStartInverted", "extra_arg", "-f", "file_name", "--pipe", "my_pipe"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendMsgQLongExtraStartInverted", "extra_arg", "-f", "file_name", "--queue", "/my_queue"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE,
					std::vector<std::string>({"ReceivePipeShort", "-p", "my_pipe", "-f", "file_name"}),
					IPCProtocol::PIPE, 0, "file_name", "my_pipe", false, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE,
					std::vector<std::string>({"ReceiveMsgQShort", "-q", "/my_queue", "-f", "file_name"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", false, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceivePipeShortExtraEnd", "-p", "my_pipe", "-f", "file_name", "extra_arg"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceiveMsgQShortExtraEnd", "-q", "/my_queue", "-f", "file_name", "extra_arg"}), IPCProtocol::MSG_QUEUE, 0,
					"file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceivePipeShortExtraMid", "-p", "my_pipe", "extra_arg", "-f", "file_name"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceiveMsgQShortExtraMid", "-q", "/my_queue", "extra_arg", "-f", "file_name"}), IPCProtocol::MSG_QUEUE, 0,
					"file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceivePipeShortExtraStart", "extra_arg", "-p", "my_pipe", "-f", "file_name"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceiveMsgQShortExtraStart", "extra_arg", "-q", "/my_queue", "-f", "file_name"}), IPCProtocol::MSG_QUEUE, 0,
					"file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE,
					std::vector<std::string>({"ReceivePipeShortInverted", "-f", "file_name", "-p", "my_pipe"}),
					IPCProtocol::PIPE, 0, "file_name", "my_pipe", false, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE,
					std::vector<std::string>({"ReceiveMsgQShortInverted", "-f", "file_name", "-q", "/my_queue"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", false, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceivePipeShortExtraEndInverted", "-f", "file_name", "extra_arg", "-p", "my_pipe"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceiveMsgQShortExtraEndInverted", "-f", "file_name", "extra_arg", "-q", "/my_queue"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceivePipeShortExtraMidInverted", "-f", "file_name", "extra_arg", "-p", "my_pipe"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceiveMsgQShortExtraMidInverted", "-f", "file_name", "extra_arg", "-q", "/my_queue"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceivePipeShortExtraStartInverted", "extra_arg", "-f", "file_name", "-p", "my_pipe"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceiveMsgQShortExtraStartInverted", "extra_arg", "-f", "file_name", "-q", "/my_queue"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE,
					std::vector<std::string>({"ReceivePipeLong", "--pipe", "my_pipe", "-f", "file_name"}),
					IPCProtocol::PIPE, 0, "file_name", "my_pipe", false, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE,
					std::vector<std::string>({"ReceiveMsgQLong", "--queue", "/my_queue", "-f", "file_name"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", false, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceivePipeLongExtraEnd", "--pipe", "my_pipe", "-f", "file_name", "extra_arg"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceiveMsgQLongExtraEnd", "--queue", "/my_queue", "-f", "file_name", "extra_arg"}), IPCProtocol::MSG_QUEUE, 0,
					"file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceivePipeLongExtraMid", "--pipe", "my_pipe", "extra_arg", "-f", "file_name"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceiveMsgQLongExtraMid", "--queue", "/my_queue", "extra_arg", "-f", "file_name"}), IPCProtocol::MSG_QUEUE, 0,
					"file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceivePipeLongExtraStart", "extra_arg", "--pipe", "my_pipe", "-f", "file_name"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceiveMsgQLongExtraStart", "extra_arg", "--queue", "/my_queue", "-f", "file_name"}), IPCProtocol::MSG_QUEUE,
					0, "file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE,
					std::vector<std::string>({"ReceivePipeLongInverted", "-f", "file_name", "--pipe", "my_pipe"}),
					IPCProtocol::PIPE, 0, "file_name", "my_pipe", false, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE,
					std::vector<std::string>({"ReceiveMsgQLongInverted", "-f", "file_name", "--queue", "/my_queue"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", false, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceivePipeLongExtraEndInverted", "-f", "file_name", "extra_arg", "--pipe", "my_pipe"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceiveMsgQLongExtraEndInverted", "-f", "file_name", "extra_arg", "--queue", "/my_queue"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceivePipeLongExtraMidInverted", "-f", "file_name", "extra_arg", "--pipe", "my_pipe"}), IPCProtocol::PIPE, 0,
					"file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceiveMsgQLongExtraMidInverted", "-f", "file_name", "extra_arg", "--queue", "/my_queue"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceivePipeLongExtraStartInverted", "extra_arg", "-f", "file_name", "--pipe", "my_pipe"}), IPCProtocol::PIPE,
					0, "file_name", "my_pipe", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceiveMsgQLongExtraStartInverted", "extra_arg", "-f", "file_name", "--queue", "/my_queue"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>({"SendZeroArgs"}), IPCProtocol::PIPE, 0, "0", "0",
					true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>({"SendOneArgs", "-f"}), IPCProtocol::PIPE, 0, "0", "0",
					true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>({"SendTwoArgs", "-f", "file_name"}), IPCProtocol::PIPE,
					0, "0", "0", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>({"SendThreeArgs", "-f", "file_name", "-q"}),
					IPCProtocol::PIPE, 0, "0", "0", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE,
					std::vector<std::string>({"SendFourArgs", "-f", "file_name", "file_name", "-q"}), IPCProtocol::PIPE,
					0, "0", "0", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE,
					std::vector<std::string>({"SendFiveArgs", "-f", "file_name", "-q", "/my_queue", "extra"}),
					IPCProtocol::PIPE, 0, "0", "0", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE,
					std::vector<std::string>({"SendSixArgs", "file_name", "-f", "/my_queue", "-q", "/my_shm", "-s"}),
					IPCProtocol::PIPE, 0, "0", "0", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>(
						{"SendSevenArgs", "file_name", "-f", "/my_queue", "-q", "/my_shm", "-s", "5"}), IPCProtocol::PIPE, 0, "0", "0",
					true, 10, "MB"),

	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>({"ReceiveZeroArgs"}), IPCProtocol::PIPE, 0, "0",
					"0", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>({"ReceiveOneArgs", "-f"}), IPCProtocol::PIPE, 0,
					"0", "0", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>({"ReceiveTwoArgs", "-f", "file_name"}),
					IPCProtocol::PIPE, 0, "0", "0", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>({"ReceiveThreeArgs", "-f", "file_name", "-q"}),
					IPCProtocol::PIPE, 0, "0", "0", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE,
					std::vector<std::string>({"ReceiveFourArgs", "-f", "file_name", "file_name", "-q"}),
					IPCProtocol::PIPE, 0, "0", "0", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE,
					std::vector<std::string>({"ReceiveFiveArgs", "-f", "file_name", "-q", "/my_queue", "extra"}),
					IPCProtocol::PIPE, 0, "0", "0", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE,
					std::vector<std::string>({"ReceiveSixArgs", "file_name", "-f", "/my_queue", "-q", "/my_shm", "-s"}),
					IPCProtocol::PIPE, 0, "0", "0", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>(
						{"ReceiveSevenArgs", "file_name", "-f", "/my_queue", "-q", "/my_shm", "-s", "5"}), IPCProtocol::PIPE, 0, "0",
					"0", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>({"SendShortHelp", "-h"}), IPCProtocol::NONE, 0, "", "",
					false, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>({"SendLongHelp", "--help"}), IPCProtocol::NONE, 0, "",
					"", false, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>({"SendShortHelpExtra", "extra", "-h"}),
					IPCProtocol::NONE, 0, "", "", true, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>({"SendLongHelpExtra", "extra", "--help"}),
					IPCProtocol::NONE, 0, "", "", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>({"ReceiveShortHelp", "-h"}), IPCProtocol::NONE, 0,
					"", "", false, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>({"ReceiveLongHelp", "--help"}), IPCProtocol::NONE,
					0, "", "", false, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>({"ReceiveShortHelpExtra", "extra", "-h"}),
					IPCProtocol::NONE, 0, "", "", true, 10, "MB"),
	std::make_tuple(IPCMode::RECEIVE_MODE, std::vector<std::string>({"ReceiveLongHelpExtra", "extra", "--help"}),
					IPCProtocol::NONE, 0, "", "", true, 10, "MB")
), IPCHandlerTestSuite::PrintToStringParamName());

INSTANTIATE_TEST_SUITE_P
(, ChronoTimeTestSuite, testing::Values(
	std::make_tuple("UpdateAll", 0),
	std::make_tuple("Zero", 0),
	std::make_tuple("One", 1)
), ChronoTimeTestSuite::PrintToStringParamName());

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
