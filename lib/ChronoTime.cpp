/** @fileiChronoTime.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief ChronoTime class methods' definition
 */

#include "lib/ChronoTime.h"

void ChronoTime::update_all()
{
	this->begin = std::chrono::system_clock::now();
	this->end = this->begin;
	this->duration = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
}

void ChronoTime::update_begin()
{
	this->begin = std::chrono::system_clock::now();
	this->duration = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
}

void ChronoTime::update_end()
{
	this->end = std::chrono::system_clock::now();
	this->duration = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
}

long ChronoTime::get_duration() const
{
	return this->duration;
}
