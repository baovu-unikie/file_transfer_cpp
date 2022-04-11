/** @file ChronoTimeTestSuite.h
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#ifndef CHRONOTIMETESTSUITE_H
#define CHRONOTIMETESTSUITE_H

#include <gtest/gtest.h>
#include "lib/ChronoTime.h"

class ChronoTimeTestSuite : public testing::TestWithParam<std::tuple<std::string, int>>
{
	public:
		struct PrintToStringParamName
		{
			template<class ParamType>
			std::string operator()(const testing::TestParamInfo<ParamType> &info)
			{
				auto params = info.param;
				return std::get<0>(params);
			}
		};

		std::tuple<std::string, int> tuple = GetParam();
		std::string test_name = std::get<0>(tuple);
		int expected_duration = std::get<1>(tuple);

	protected:
		ChronoTime timer;

};


#endif //CHRONOTIMETESTSUITE_H
