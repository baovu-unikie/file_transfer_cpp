/** @file IPCHandlerTestSuite.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#include "test/IPCHandlerTestSuite.h"

INSTANTIATE_TEST_SUITE_P
(, IPCHandlerTestSuite, testing::Values(
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>({"SendPipeShort", "-p", "my_pipe", "-f", "file_name"}),
					IPCProtocol::PIPE, 0, "file_name", "my_pipe", false, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE, std::vector<std::string>({"SendPipeShortFileSizeZero", "-p", "my_pipe", "-f", "file_name"}),
					IPCProtocol::PIPE, 0, "file_name", "my_pipe", false, 0, "MB"),
	std::make_tuple(IPCMode::SEND_MODE,
					std::vector<std::string>({"SendMsgQShort", "-q", "/my_queue", "-f", "file_name"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", false, 10, "MB"),
	std::make_tuple(IPCMode::SEND_MODE,
					std::vector<std::string>({"SendMsgQShortFileSizeZero", "-q", "/my_queue", "-f", "file_name"}),
					IPCProtocol::MSG_QUEUE, 0, "file_name", "/my_queue", false, 0, "MB"),
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

