/** @file ChronoTime.h
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief  ChronoTime class
 */

#ifndef FILE_TRANSFER_CPP_CHRONOTIME_H
#define FILE_TRANSFER_CPP_CHRONOTIME_H

#include <chrono>

/**
 * ChronoTime class offers methods to calculate duration between time points. Everytime a time point is updated,
 * the duration value will be re-calculated. It is used file_transfer project as a timer to determine when to
 * throw timeout error.
 */
class ChronoTime
{
	private:
		std::chrono::time_point<std::chrono::system_clock> begin{};
		std::chrono::time_point<std::chrono::system_clock> end{};
		long duration{};
	public:
		void update_all();
		void update_begin();
		void update_end();
		long get_duration() const;
};

#endif //FILE_TRANSFER_CPP_CHRONOTIME_H
