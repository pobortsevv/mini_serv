/*
 * ===================================================================
 *
 *       Filename:  main.c
 *
 *    Description: entrypoint 
 *
 *        Version:  1.0
 *        Created:  22.07.2022 18:43:09
 *       Compiler:  gcc
 *
 *         Author:  sabra
 *
 * ===================================================================
 */

#include "utils.h"

t_client *g_clients = NULL;
int sock_fd, g_id = 0;
char msg[42], str[42*4096], tmp[42*4096], buf[42*4096 + 42];
fd_set curr_sock;
fd_set cpy_write, cpy_read;

// Отправляем сообщение всем клиентским сокетам.
void send_all(int fd, char *str_req) {
	t_client *tmp = g_clients;

	while (tmp) {
		if (tmp->fd != fd && FD_ISSET(tmp->fd, &cpy_write)) {
			// Если дескриптор клиента доступен на write, то отправляем сообщение
			//
			// decl: ssize_t send(int socket, const void *buffer, size_t length, int flags);
			//
			// socket - fd, куда отправляем сообщение
			// buffer - то, что отправляем
			// length - размер сообщения
			// flags - флаги-доп-настройки, которые в нашем случае не нужны
			if (send(tmp->fd, str_req, strlen(str_req), 0) < 0)
				fatal();
		}
		tmp = tmp->next;
	}
}

void ex_msg(int fd) {
	int i = 0;
	int j = 0;

	while (str[i]) {
		tmp[j] = str[i];
		j++;
		if (str[i] == '\n') {
			sprintf(buf, "client %d: %s", get_id(fd), tmp);
			send_all(fd, buf);
			j = 0;
			bzero(&tmp, strlen(tmp));
			bzero(&buf, strlen(buf));
		}
		i++;
	}
	bzero(&str, strlen(str));
}

void add_client() {
	// Структура, для записи адреса сокета,
	// с которым установилось соединение.
	struct sockaddr_in clientaddr;
	// Размер структуры.
	socklen_t len = sizeof(clientaddr);
	int client_fd;

	// decl: int accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
	// Производим принятие соединения.
	//
	// socket - дескриптор слушающего сокета.
	// address - структура с адресом сокета клиента.
	// address_len - размер структуры.
	if ((client_fd = accept(sock_fd, (struct sockaddr *)&clientaddr, &len)) < 0)
		fatal();
	// Всем клиентам, которые были подключены к серверу, отправляется сообщение: "server: client %d just arrived\n"
	sprintf(msg, "server: client %d just arrived\n", add_client_to_list(client_fd)); // Сформировали сообщение
	send_all(client_fd, msg); // Отправили
	FD_SET(client_fd, &curr_sock); // Добавили fd-шник во мн-во сокетов, с которыми мы работаем. 
}

int main(int ac, char **av) {
	if (ac != 2) {
		write(2, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
		exit(1);
	}
	// sockaddr_in - структура, которая содержит инфу для подключения.
	// У неё есть три поля:
	//
	// 1) sin_family - семейство адресации, в нашем случае AF_INET
	// 2) sin_addr - структура, с одним полем s_addr - ip-адрес в
	// виде беззнакового четырёхбайтного целого, опять же в сетевом порядке
	// байтов.
	// 3) sin_port - порт подключения.
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr)); // Занулили стркутуру.
	uint16_t port = atoi(av[1]);

	// htol, htons
	// h - host, порядок байтов этой машины.
	// to - конвертируем в сетевой порядок (избегаем little/big endian отличий)
	// n - network, то есть сетевой порядок байтов.
	// s - короткие целые числа (short - 16 бит).
	// l - long (32 бит).
	servaddr.sin_family = AF_INET; // Работаем с протоколами TCP/IP
	servaddr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
	servaddr.sin_port = htons(port);

	// decl: int socket(int family, int type, int protocol);
	//
	// family - семейство адресации, мы будем использовать
	// AF_INET для взаимодействия посредством протоколов TCP/IP.
	//
	// type - тип взаимодействия, используем либо потоковое SOCK_STREAM,
	// либо дейтаграммное SOCK_DGRAM.
	//
	// protocol - конкретно используемый протокол. Мы рассматриваем потоковое
	// TCP/IP соединение, так что протокол можно не определять.
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		fatal();

	// decl: int bind(int socket, const struct sockaddr *address, socklen_t address_len);
	// 
	// Для того, чтобы с сокетом мог установить соединение другой участник сетевого
	// взаимодействия, то этому сокету следует выдать адрес c помощью bind.
	//
	// socket - дескриптор сокета.
	//
	// address - указатель на структуру, содержащую адрес.
	//
	// address_len - размер структуры адреса в байтах.
	if ((bind(sock_fd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) < 0)
		fatal();

	// decl: int listen(int socket, int backlog);
	//
	// После создания сокета, переводим его в слушающий режим.
	// Этот сокет будет предназначен искючительно для запросы соединения.
	// Поэтому ядро ОС самостоятельно будет создавать доп. сокет для
	// взаимодействия с клиентом. (Наша программа в данный момент серверная).
	// 
	// socket - дескриптор сокета.
	// backlog - размер очереди непринятых запросов.
	if (listen(sock_fd, 0) < 0)
		fatal();

	// Способ построения программ, при котором программа имеет главный цикл,
	// одна итерация которого соответствует наступлению некоторого события из
	// определённого множества, а все действия программы построены как реакция
	// на событие, называется событийно-управляемым программированием (event-driven)
	//
	// В данном случае у нас есть два события:
	// 1) подключение клиента к серверу
	// 2) обработка данных из клиентских сокетов
	
	FD_ZERO(&curr_sock); // Обнуляем множество сокетов
	FD_SET(sock_fd, &curr_sock); // Добавляем слушающий дескриптор во мн-во
	bzero(&tmp, sizeof(tmp));
	bzero(&buf, sizeof(buf));
	bzero(&str, sizeof(str));

	while(1) {
		// decl: select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds, fd_set *restrict errorfds, struct timeval *restrict timeout);
		// 
		// Наступление события операционная система должна сама и вернуть управление
		// процессу, при этом сообщить какое событие произошло. Выборкой событий
		// будет заниматься функция select. Этот вызов позволяет обрабатывать:
		// 1) Изменение состояния дескриптора
		// 2) истечение заданного кол-ва времени с момента входа в вызов
		// 3) получение процессом неигнорируемого сигнала
		//
		// nfds - кол-во значащих элементов в этих множествах (нужно установить max_d+1,
		// где max_d - максимальный номер дескриптора среди подлежащих обработки)
		// 
		// readfds, writefds, errorfds - множества файловых дескрипторов.
		//
		// timeout - структура с двумя полями long (кол-во секунд, кол-во микросекунд),
		// чтобы задать таймауты.
		//
		// fd_set - это объект "множества дескрипторов", предствляется как битовая строка,
		// где каждому дескриптору соответветсвует один бит.
		// Для работы с этим объектом есть след макросы:
		// FD_ZERO(fd_set *set) // очистить множество
		// FD_CLR(int fd, fd_set *set) // убрать дескриптор из мн-ва
		// FD_SET(int fd, fd_set *set) // добавить дескриптор к мн-ву
		// FD_ISSET(int fd, fd_set *set) // входит ли декср-р во мн-во?
		cpy_read = cpy_write = curr_sock;
		if (select(get_max_fd() + 1, &cpy_read, &cpy_write, NULL, NULL) < 0)
			continue;
		for (int fd = 0; fd <= get_max_fd(); fd++) {
			if (FD_ISSET(fd, &cpy_read)) { // Произошло ли событие в сокете?
				if (fd == sock_fd) { // Пришёл новый запрос на соединение. Добавляем клиент
					bzero(&msg, sizeof(msg));
					add_client();
					break;
				} else { // Пришли данные на прочтение из клиентского сокета
					int ret_recv = 1000;
					while (ret_recv == 1000 || str[strlen(str) - 1] != '\n') {
						// decl: ssize_t recv(int socket, void *buffer, size_t length, int flags);
						//
						// Получаем сообщение из сокета.
						// socket - дескриптор сокета
						// buffer - строка, куда будем записывать сообщение
						// length - размер сообщения (мы будем ожидать 1000)
						if ((ret_recv = recv(fd, str + strlen(str), 1000, 0)) <= 0)
							break;
					}

					// После окончания прочтения сообщения проверяем, произошла ли ошибка.
					// Если так, то удаляем клиента и закрываем дескриптор.
					if (ret_recv <= 0) {
						bzero(&msg, sizeof(msg));
						sprintf(msg, "server: client %d just left\n", rm_client(fd));
						send_all(fd, msg);
						FD_CLR(fd, &curr_sock);
						close(fd);
						break;
					} else { // Если ошибки не произошло - обрабатываем сообщение.
						ex_msg(fd);
					}
				}
			}
		}
	}

	return (0);
}

