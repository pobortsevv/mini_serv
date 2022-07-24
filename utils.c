/*
 * ===================================================================
 *
 *       Filename:  utils.c
 *
 *    Description: utility functions 
 *
 *        Version:  1.0
 *        Created:  24.07.2022 12:31:34
 *       Compiler:  clang++
 *
 *         Author:  sabra
 *
 * ===================================================================
 */

#include "utils.h"

void fatal() {
	write(2, "Fatal error\n", strlen("Fatal error\n"));
	close(sock_fd);
	exit(1);
}
