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

#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in структура.

int main(void) {
	int sock_fd;

	// sockaddr_in структура, которая содержит инфу для подключения.
	// У неё есть три поля:
	//
	// 1) sin_family - семейство адресации, в нашем случае AF_INET
	// 2) sin_addr - структура, с одним полем s_addr - ip-адрес в
	// виде беззнакового четырёхбайтного целого, опять же в сетевом порядке
	// байтов.
	// 3) sin_port - порт подключения.
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr)); // Занулили стркутуру.

	// htol, htons
	// n - network, то есть сетевой порядок байтов.
	// h - host, порядок байтов этой машины.
	// s - короткие целые числа (short - 16 бит).
	// l - long (32 бит).

	servaddr.sin_family = AF_INET; // Работаем с протоколами TCP/IP
	servaddr.sin_addr.s_addr = htol(2130706433); // 127.0.0.1
	servaddr.sin_port = htons(80);

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
	if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return(-1);

	// decl: bind(int socket, const struct sockaddr *address, socklen_t address_len);
	// 
	// Для того, чтобы с сокетом мог установить соединение другой участник сетевого
	// взаимодействия, то этому сокету следует выдать адрес c помощью bind.
	//
	// socket - дескриптор сокета.
	//
	// address - указатель на структуру, содержащую адрес.
	//
	// address_len - размер структуры адреса в байтах.
	if((bind(sock_fd, (const struct sockaddr *)servaddr, sizeof(servaddr))) < 0)
		return(-1);


	return (0);
}

