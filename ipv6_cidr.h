#ifndef _IPV6_CIDR_H_
#define _IPV6_CIDR_H_

#include "queue.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif

typedef struct ipv6_cidr {
    struct in6_addr addr;
    int mask;
    TAILQ_ENTRY(ipv6_cidr) node;
} ipv6_cidr_t;

typedef struct ipv6_cidr_table {
    TAILQ_HEAD(, ipv6_cidr) cidrs;
} ipv6_cidr_table_t;

void ipv6_cidr_table_init(ipv6_cidr_table_t *table);

void ipv6_cidr_table_clear(ipv6_cidr_table_t *table);

ipv6_cidr_table_t *ipv6_cidr_table_create();

void ipv6_cidr_table_free(ipv6_cidr_table_t *table);

bool ipv6_in_cidr_table(ipv6_cidr_table_t *table, const char *ip_str);

int ipv6_cidr_put_to_table(ipv6_cidr_table_t *table, const char *cidr_str);

bool ipv6_cidr_table_is_empty(ipv6_cidr_table_t *table);

void ipv6_cidr_table_copy(ipv6_cidr_table_t *old_table, ipv6_cidr_table_t *new_table);

#endif