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
# include <sys/select.h>
# include <netinet/in.h> // sockaddr_in структура.
# include <unistd.h>
# include <strings.h>
# include <stdlib.h>
# include <stdio.h>

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

extern int sock_fd; // Дескриптор слушающего сокета
extern int g_id; // Глобальный счётчик клиентов
extern t_client *g_clients; // Связанный список клиентов
extern char msg[42]; // Массив, в который мы будем записывать сообщение для клиентов
// msg - формируется на уровне runloop-a и при добавлении клиента.
// str - используется для получения сообщения в recv функции.
// tmp и buf - нужны на уровне ex_msg функции для отправки получего сообщения всем клиентам.
extern char str[42*4096], tmp[42*4096], buf[42*4096 + 42]; // Массивы char-ов для работы с сообщениями

extern fd_set curr_sock; // Множество из сокетов, с которыми мы на данный момент работаем
extern fd_set cpy_write, cpy_read; // Множество сокетов на I/O

void fatal(void); // Вызываем при системных ошибках (внутри закрывается слушающий сокет)
int get_max_fd(); // Необходим для select - определить максимальный дексприптор (кол-во обслуживающих)
int add_client_to_list(int fd); // Добавляем в конец списка клиента
int rm_client(int fd); // Удаление клиента из списка
int get_id(int fd); // Получаем id-шник клиента при отправке сообщения

#endif
