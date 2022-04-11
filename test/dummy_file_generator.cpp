/** @file dummy_file_generator.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#include "test/dummy_file_generator.h"

void generate_dummy_files(const std::string &file_name, std::streamsize file_size, std::string &file_unit)
{
	try
	{
		boost::algorithm::trim(file_unit);
		std::ofstream ofs(file_name, std::ios::out | std::ios::binary);
		std::streamsize unit{0};
		std::cout << file_name << " " << file_size << " " << file_unit << std::endl;
		if (file_unit == "B")
			unit = 0;
		else if (file_unit == "KB")
			unit = 10;
		else if (file_unit == "MB")
			unit = 20;
		else if (file_unit == "GB")
			unit = 30;

		if (file_size > 0)
		{
			ofs.seekp((file_size << unit) - 1);
			ofs.write("", 1);
		}
		ofs.close();
	}
	catch (const std::exception &e)
	{
		std::rethrow_exception(std::current_exception());
	}
}