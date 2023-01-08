#ifndef SSHIENT_WRAPPERS_H
#define SSHIENT_WRAPPERS_H

#include "sys/types.h"
#include "sys/socket.h"

void ErrorHandler(int res, char* text);

int Socket (int domain, int type, int protocol);

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

void Listen(int sockfd, int backlog);

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

void Connectfd(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

void Inet_ptonfd(int af, const char *src, void *dst);

#endif
