/** @file ChronoTimeTestSuite.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#include "test/ChronoTimeTestSuite.h"

INSTANTIATE_TEST_SUITE_P
(, ChronoTimeTestSuite, testing::Values(
	std::make_tuple("UpdateAll", 0),
	std::make_tuple("Zero", 0),
	std::make_tuple("One", 1)
), ChronoTimeTestSuite::PrintToStringParamName());
