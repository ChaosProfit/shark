/*
 * namespaces.hpp
 *
 *  Created on: Mar 3, 2018
 *      Author: luguanglong
 */

#ifndef CONTAINER_NAMESPACES_HPP_
#define CONTAINER_NAMESPACES_HPP_

namespace shark{
int fileUpdate(char *filePath, char *value);
int userNsInit(int pid);
int utsNsInit(const char *hostname);
}  // namespace shark

#endif /* CONTAINER_NAMESPACES_HPP_ */
