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

int get_max_fd() {
	t_client *tmp = g_clients;
	int max = sock_fd;

	while (tmp) {
		if (tmp->fd > max)
			max = tmp->fd;
		tmp = tmp->next;
	}

	return (max);
}

int get_id(int fd) {
	t_client *tmp = g_clients;
	
	while (tmp) {
		if (tmp->fd == fd) {
			return (tmp->fd);
		}
	}

	return (-1);
}

int add_client_to_list(int fd) {
	t_client *tmp = g_clients;
	t_client *new;

	if (!(new = calloc(1, sizeof(t_client))))
		fatal();
	
	new->id = g_id++;
	new->fd = fd;
	new->next = NULL;

	if (!g_clients) {
		g_clients = new;
	} else {
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}

	return (new->id);
}

int rm_client(int fd) {
	t_client *tmp = g_clients;
	t_client *del;
	int id = get_id(fd);

	if (tmp && tmp->fd == fd) {
		g_clients = tmp->next;
		free(tmp);
	} else {
		while (tmp && tmp->next && tmp->next->fd != fd)
			tmp = tmp->next;
		del = tmp->next;
		tmp->next = tmp->next->next;
		free(del);
	}
	
	return(id);
}
