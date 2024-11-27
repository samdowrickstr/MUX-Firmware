#ifndef __UDPCLIENT_H
#define __UDPCLIENT_H

#include "lwip/ip_addr.h"
#include <stddef.h>

/* Function Prototypes */
void udpclient_init(void);
void udpsend_trigger(const char *data, size_t len);
void udpsend_serial(const char *data, size_t len);
void udp_set_server_ip(const char *ip);

#endif /* __UDPCLIENT_H */
