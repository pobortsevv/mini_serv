/*
 * ===================================================================
 *
 *       Filename:  utils.h
 *
 *    Description: declaration of utils
 *
 *        Version:  1.0
 *        Created:  24.07.2022 12:19:38
 *       Compiler:  clang++
 *
 *         Author:  sabra
 *
 * ===================================================================
 */

#ifndef UTILS_H
# define UTILS_H

# include <sys/socket.h>
# include <netinet/in.h> // sockaddr_in структура.

// Наш сервер будет принимать подключение нескольких клиентов.
// Так что будем хранить их в связанном списке.
// 
// fd - дескриптор, который будет возвращать accept
// id - удостоверение личности. первый клиент получит идентификатор 0,
// а каждый новый клиент получит последний идентификатор клиента + 1
typedef struct	s_client {
	int fd;
	int id;
	struct s_client *next;
}								t_client;

void fatal(void); // Вызываем при системных ошибках (внутри закрывается слушающий сокет)

#endif

