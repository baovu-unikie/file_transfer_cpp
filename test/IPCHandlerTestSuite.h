/** @file IPCHandlerTestSuite.h
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#ifndef IPCHANDLERTESTSUITE_H
#define IPCHANDLERTESTSUITE_H

#include <gtest/gtest.h>
#include "lib/IPCHandler.h"

class IPCHandlerTestSuite
	: public testing::TestWithParam<std::tuple<IPCMode, std::vector<std::string>, IPCProtocol, size_t, std::string, std::string, bool, std::streamsize, std::string>>
{
	public:
		struct PrintToStringParamName
		{
			template<class ParamType>
			std::string operator()(const testing::TestParamInfo<ParamType> &info)
			{
				auto params = static_cast<std::tuple<IPCMode, std::vector<std::string>, IPCProtocol, size_t, std::string, std::string, bool, std::streamsize, std::string>>(info.param);
				return std::get<1>(params).at(0);
			}
		};
		std::tuple<IPCMode, std::vector<std::string>, IPCProtocol, size_t, std::string, std::string, bool, std::streamsize, std::string> tuple = GetParam();
		// Get parameters from tuples
		IPCMode mode = std::get<0>(tuple);
		std::vector<std::string> argv = std::get<1>(tuple);
		IPCProtocol protocol = std::get<2>(tuple);
		size_t mem_size = std::get<3>(tuple);
		std::string file_name = std::get<4>(tuple);
		std::string server_name = std::get<5>(tuple);
		bool is_throw = std::get<6>(tuple);
		std::streamsize file_size = std::get<7>(tuple);
		std::string file_unit = std::get<8>(tuple);

	protected:
		IPCHandler ipc_handler;
};


#endif //IPCHANDLERTESTSUITE_H
