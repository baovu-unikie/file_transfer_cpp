/** @file dummy_file_generator.h
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#ifndef DUMMY_FILE_GENERATOR_H
#define DUMMY_FILE_GENERATOR_H

#include <boost/algorithm/string/trim.hpp>
#include <iostream>
#include <fstream>

void generate_dummy_files(const std::string &file_name, std::streamsize file_size, std::string &file_unit);

#endif //DUMMY_FILE_GENERATOR_H
