/** @file IPCShm.cpp
 *  @author Bao Vu (bao.vu@unikie.com)
 *  @brief
 */

#include "IPCShm.h"

IPCShm::~IPCShm()
{
	munmap(this->shm_ptr, this->shm_size_in_bytes);
}
