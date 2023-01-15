#include "../include/wrappers.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

// На java больше подходит с PascalCase
/**
 * @brief 
 * 
 * @param res 
 * @param text 
 */
void ErrorHandler(int res, char *text) {
    if (res == - 1) {
        perror(text);
        exit(EXIT_FAILURE);
    }
}


/**
 * @brief 
 * 
 * @param domain 
 * @param type 
 * @param protocol 
 * @return int 
 */
int Socket(int domain, int type, int protocol) {
    int res = socket(domain, type, protocol);
    ErrorHandler(res, "error while creating socket");
    return res;
}


/**
 * @brief 
 * 
 * @param sockfd 
 * @param addr 
 * @param addrlen 
 */
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int res = bind(sockfd, addr, addrlen);
    ErrorHandler(res, "bind failure");
}


/**
 * @brief 
 * 
 * @param sockfd 
 * @param backlog 
 */
void Listen(int sockfd, int backlog) {
    int res = listen(sockfd, backlog);
    ErrorHandler(res, "listen failure");
}


/**
 * @brief 
 * 
 * @param sockfd 
 * @param addr 
 * @param addrlen 
 * @return int 
 */
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int res = accept(sockfd, addr, addrlen);
    ErrorHandler(res, "accept failure");
    return res;
}


/**
 * @brief 
 * 
 * @param sockfd 
 * @param addr 
 * @param addrlen 
 */
void Connectfd(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int res = connect(sockfd, addr, addrlen);
    ErrorHandler(res, "connect failure");
}


/**
 * @brief 
 * 
 * @param af 
 * @param src 
 * @param dst 
 */
void Inet_ptonfd(int af, const char *src, void *dst) {
    int res = inet_pton(af, src, dst);
    if (res == 0) {
        printf("inet_pton failed: src does not contain a character"
               "string representing a valid network address int the specified"
               "address family\n");
        exit(EXIT_FAILURE);
    } else if (res == -1) {
        perror("inet_pton failure");
    }
}
