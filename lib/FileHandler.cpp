/** @file FileHandler.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#include "lib/FileHandler.h"

FileHandler::~FileHandler()
{
	if (fs.is_open())
		fs.close();
}

unsigned long FileHandler::get_file_size()
{
	return boost::filesystem::file_size(this->file_name);
}

void FileHandler::open_file()
{
	if (this->mode == FileMode::READ)
		this->fs.open(this->file_name, std::fstream::in | std::fstream::binary);
	if (this->mode == FileMode::WRITE)
		this->fs.open(this->file_name, std::fstream::out | std::fstream::trunc | std::fstream::binary);
	if (!this->fs) // Check logical error on I/O operation (close/open)
		throw std::runtime_error("ERROR: " + this->file_name + ": " + strerror(errno));
}

void FileHandler::write_file(std::vector<char> &data, std::streamsize &data_size)
{
	if (this->mode == FileMode::WRITE)
	{
		this->fs.write(data.data(), data_size);
		if (this->fs.bad()) // check read/writing error on i/o operation
			throw std::runtime_error("ERROR: ostream::write().");
	}
	else
		throw std::runtime_error("ERROR: write_file(): Tried to write file in a wrong mode.");
}

void FileHandler::read_file(std::vector<char> &data, std::streamsize &data_size)
{
	if (this->mode == FileMode::READ)
	{
		this->fs.read(data.data(), data_size);
		if (this->fs.bad()) // check read/writing error on i/o operation
			throw std::runtime_error("ERROR: istream::read().");
	}
	else
		throw std::runtime_error("ERROR: read_file(): Tried to read file in a wrong mode.");
}

void FileHandler::close_file()
{
	fs.close();
}
