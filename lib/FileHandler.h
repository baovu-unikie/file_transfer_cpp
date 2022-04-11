/** @file FileHandler.h
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <boost/filesystem.hpp>

enum class FileMode
{
	NONE, READ, WRITE
};

class FileHandler
{
	protected:
		std::fstream fs{};
		std::string file_name{};
		FileMode mode{FileMode::NONE};
	public:
		explicit FileHandler(std::string fn, FileMode m) : file_name(std::move(fn)), mode(m)
		{};
		~FileHandler();
		unsigned long get_file_size();
		void close_file();
		void open_file();
		void read_file(std::vector<char> &data, std::streamsize &data_size);
		void write_file(std::vector<char> &data, std::streamsize &data_size);
};

#endif //FILEHANDLER_H
