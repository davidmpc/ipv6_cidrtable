#include "ipv6_cidr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ipv6_cidr_table_init(ipv6_cidr_table_t *table) {
    TAILQ_INIT(&table->cidrs);
}

void ipv6_cidr_table_clear(ipv6_cidr_table_t *table) {
    ipv6_cidr_t *c = NULL, *temp = NULL;
    TAILQ_FOREACH_SAFE(c, &table->cidrs, node, temp) {
        TAILQ_REMOVE(&table->cidrs, c, node);
        free(c);
    }
}

ipv6_cidr_table_t *ipv6_cidr_table_create() {
    ipv6_cidr_table_t *table = (ipv6_cidr_table_t *)calloc(1, sizeof(ipv6_cidr_table_t));
    ipv6_cidr_table_init(table);
    return table;
}

void ipv6_cidr_table_free(ipv6_cidr_table_t *table) {
    ipv6_cidr_table_clear(table);
    free(table);
}

bool ipv6_in_cidr(ipv6_cidr_t *cidr, struct in6_addr *ip) {
    int bytes_num = 0, bits_num = 0;
    bytes_num = cidr->mask / 8;
    if (memcmp(&cidr->addr, ip, bytes_num) != 0) {
        return false;
    }
    bits_num = cidr->mask % 8;
    if (bits_num != 0) {
        if ((*((uint8_t *)ip + bytes_num) & (uint8_t) ~((1 << (8 - bits_num)) - 1)) != *((uint8_t *)&cidr->addr + bytes_num)) {
            return false;
        }
    }
    return true;
}

bool ipv6_in_cidr_table(ipv6_cidr_table_t *table, const char *ip_str) {
    struct in6_addr ip;
    if (inet_pton(AF_INET6, ip_str, &ip) != 1) {
        return false;
    }
    ipv6_cidr_t *cidr;
    TAILQ_FOREACH(cidr, &table->cidrs, node) {
        if (ipv6_in_cidr(cidr, &ip)) {
            return true;
        }
    }
    return false;
}

// allow single ip that without mask value, e.g."10:20::"
static int parse_cidr(const char *cidr_str, ipv6_cidr_t *cidr) {
    char ip_str[64] = {0};
    char mask_str[16] = {0};
    int mask = 0;
    bool has_mask = false;
    for (size_t i = 0; i < strlen(cidr_str); i++) {
        if (cidr_str[i] == '/') {
            has_mask = true;
            memmove(ip_str, cidr_str, i);
            memmove(mask_str, cidr_str + i + 1, strlen(cidr_str) - (i + 1));
            break;
        }
    }
    if (has_mask) {
        if ((mask = atoi(mask_str)) == 0 || mask > 128 || mask <= 0) {
            return -1;
        }
    } else {
        mask = 128;
    }

    // get ip from cidr
    struct in6_addr ip;
    if (inet_pton(AF_INET6, has_mask ? ip_str : cidr_str, &ip) != 1) {
        return -1;
    }
    // get real ip which suffix is 0
    int bits_num = mask % 8;
    int bytes_num = mask / 8;
    if (bytes_num < 15) {
        memset((char *)&ip + bytes_num + 1, 0, 16 - bytes_num - 1);
    }
    *((uint8_t *)&ip + bytes_num) &= (uint8_t) ~((1 << (8 - bits_num)) - 1);

    memmove(&cidr->addr, &ip, sizeof(ip));
    cidr->mask = mask;
    return 0;
}

int ipv6_cidr_put_to_table(ipv6_cidr_table_t *table, const char *cidr_str) {
    ipv6_cidr_t *cidr = (ipv6_cidr_t *)calloc(1, sizeof(ipv6_cidr_t));
    if (parse_cidr(cidr_str, cidr) == -1) {
        free(cidr);
        return -1;
    }
    char ip_str[64] = {0};
    if (inet_ntop(AF_INET6, &cidr->addr, ip_str, sizeof(ip_str)) == NULL) {
        free(cidr);
        return -1;
    }
    if (ipv6_in_cidr_table(table, ip_str)) {
        free(cidr);
        return 0;
    }

    // check if we can delete some old cidrs
    ipv6_cidr_t *c = NULL, *temp = NULL;
    TAILQ_FOREACH_SAFE(c, &table->cidrs, node, temp) {
        if (ipv6_in_cidr(cidr, &c->addr)) {
            TAILQ_REMOVE(&table->cidrs, c, node);
            free(c);
        }
    }

    TAILQ_INSERT_TAIL(&table->cidrs, cidr, node);
    return 0;
}

bool ipv6_cidr_table_is_empty(ipv6_cidr_table_t *table) {
    return TAILQ_EMPTY(&table->cidrs);
}

void ipv6_cidr_table_copy(ipv6_cidr_table_t *old_table, ipv6_cidr_table_t *new_table) {
    ipv6_cidr_table_clear(new_table);
    ipv6_cidr_t *c = NULL;
    TAILQ_FOREACH(c, &old_table->cidrs, node) {
        ipv6_cidr_t *cidr = (ipv6_cidr_t *)calloc(1, sizeof(ipv6_cidr_t));
        cidr->addr = c->addr;
        cidr->mask = c->mask;
        TAILQ_INSERT_TAIL(&new_table->cidrs, cidr, node);
    }
}