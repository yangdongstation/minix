# MINIX Network Stack Implementation Directory (/minix/net)

## Overview

The `/minix/net/` directory contains MINIX's complete network stack implementation, featuring the lwIP (lightweight IP) TCP/IP stack adapted for MINIX's microkernel architecture. This implementation provides comprehensive networking capabilities including TCP, UDP, IP, ICMP, and various link-layer protocols, all integrated with MINIX's message-passing system and service-oriented architecture.

## Directory Structure

```
minix/net/
├── Makefile          # Build configuration for network stack
├── Makefile.inc      # Common build settings and paths
├── lwip/             # lwIP TCP/IP stack implementation
└── uds/              # Unix Domain Sockets implementation
```

## Key Network Components

### LWIP - Lightweight IP Stack
The core TCP/IP implementation providing comprehensive networking protocols optimized for embedded and microkernel systems.

**Protocol Support:**
- **Link Layer**: Ethernet, PPP, SLIP
- **Network Layer**: IPv4, IPv6, ICMP, IGMP
- **Transport Layer**: TCP, UDP, RAW sockets
- **Application Layer**: DHCP, DNS, SNMP, HTTP
- **Security**: TLS/SSL integration points

**Core Architecture:**
```c
// lwIP network interface structure
struct netif {
    struct netif *next;
    struct ip_addr ip_addr;
    struct ip_addr netmask;
    struct ip_addr gw;
    
    // MINIX-specific fields
    endpoint_t driver_endpoint;
    int interface_index;
    
    // Function pointers
    err_t (*input)(struct pbuf *p, struct netif *inp);
    err_t (*output)(struct netif *netif, struct pbuf *p, struct ip_addr *ipaddr);
    err_t (*linkoutput)(struct netif *netif, struct pbuf *p);
    
    // Statistics
    u32_t tx_packets, rx_packets;
    u32_t tx_errors, rx_errors;
};
```

**Build Configuration:**
```makefile
PROG=   lwip
SRCS=   lwip.c mempool.c pchain.c addr.c addrpol.c tcpisn.c \
        mcast.c ipsock.c pktsock.c tcpsock.c udpsock.c rawsock.c \
        ifdev.c ifaddr.c loopif.c ethif.c ndev.c rttree.c route.c \
        rtsock.c lnksock.c lldata.c mibtree.c ifconf.c bpfdev.c \
        bpf_filter.c util.c

CPPFLAGS+= -I${NETBSDSRCDIR}/minix/lib/liblwip/dist/src/include
CPPFLAGS+= -I${NETBSDSRCDIR}/minix/lib/liblwip/lib
CPPFLAGS+= -DUSE_INET6           # IPv6 support
CPPFLAGS+= -DLWIP_STATS=1        # Enable statistics
CPPFLAGS+= -DLWIP_DEBUG          # Debug support

DPADD+= ${LIBLWIP} ${LIBSOCKEVENT} ${LIBSOCKDRIVER} \
        ${LIBCHARDRIVER} ${LIBSYS} ${LIBTIMERS}
LDADD+= -llwip -lsockevent -lsockdriver -lchardriver -lsys -ltimers

.include <minix.service.mk>
```

### UDS - Unix Domain Sockets
Implementation of Unix domain sockets providing local inter-process communication with socket semantics.

**Key Features:**
- Stream and datagram sockets
- File system namespace integration
- Credential passing
- Abstract namespace support
- Integration with MINIX VFS

## MINIX Microkernel Integration

### Message-Based Network Operations
All network operations are implemented using MINIX's message-passing architecture:

```c
// Socket creation through message passing
static int lwip_socket(message *m, int ipc_status) {
    int domain, type, protocol;
    int sock_fd;
    
    // Extract parameters from message
    domain = m->LWIP_SOCKET_DOMAIN;
    type = m->LWIP_SOCKET_TYPE;
    protocol = m->LWIP_SOCKET_PROTOCOL;
    
    // Create socket using lwIP
    sock_fd = lwip_socket_internal(domain, type, protocol);
    
    if (sock_fd >= 0) {
        // Return socket descriptor
        m->LWIP_SOCKET_RETVAL = sock_fd;
        m->m_type = LWIP_SOCKET_REPLY;
    } else {
        // Return error
        m->LWIP_SOCKET_RETVAL = -1;
        m->LWIP_SOCKET_ERROR = errno;
        m->m_type = LWIP_SOCKET_ERROR;
    }
    
    return _send(m->m_source, &m);
}
```

### Network Driver Integration
Network drivers communicate with the network stack through standardized interfaces:

```c
// Network driver message interface
struct net_driver_message {
    uint32_t ndm_type;
    endpoint_t ndm_endpoint;
    uint32_t ndm_interface;
    uint32_t ndm_operation;
    void *ndm_data;
    size_t ndm_size;
};

// Ethernet frame transmission
static int transmit_frame(struct netif *netif, struct pbuf *p) {
    message m;
    
    m.m_type = DEV_WRITE;
    m.DEVICE = netif->interface_index;
    m.COUNT = p->tot_len;
    m.ADDRESS = (char *)p->payload;
    m.PROC_NR = LWIP_PROC_NR;
    
    return _sendrec(netif->driver_endpoint, &m);
}
```

### Service Configuration Integration
Network services integrate with MINIX's service management:

```c
// Network service initialization
static void lwip_service_startup(void) {
    /* Initialize lwIP stack */
    lwip_init();
    
    /* Register network interfaces */
    netif_add(&eth_interface, &ipaddr, &netmask, &gw, 
              NULL, ethernetif_init, tcpip_input);
    
    /* Set default interface */
    netif_set_default(&eth_interface);
    
    /* Bring up interface */
    netif_set_up(&eth_interface);
    
    /* Register with service manager */
    sef_setcb_init_fresh(lwip_init_fresh);
    sef_startup();
}
```

## RISC-V 64-bit Specific Adaptations

### Memory Alignment for Network Buffers
```c
// RISC-V optimized network buffer alignment
#ifdef __riscv
#define RISCV_CACHE_LINE 64
#define NETBUF_ALIGN RISCV_ALIGN(RISCV_CACHE_LINE)

struct riscv_netbuf {
    uint8_t data[NETBUF_SIZE] NETBUF_ALIGN;
    uint16_t len;
    uint16_t flags;
    uint32_t checksum;
};
#endif
```

### Network Protocol Optimization
```c
// RISC-V optimized checksum calculation
#ifdef __riscv
static uint32_t riscv_checksum(const void *data, size_t len) {
    const uint32_t *p = data;
    uint64_t sum = 0;
    size_t i;
    
    // Use 64-bit operations for efficiency
    for (i = 0; i < len / 8; i++) {
        sum += ((const uint64_t *)p)[i];
    }
    
    // Handle remaining bytes
    if (len & 7) {
        const uint8_t *q = (const uint8_t *)&p[i * 2];
        uint32_t remainder = 0;
        memcpy(&remainder, q, len & 7);
        sum += remainder;
    }
    
    // Fold 64-bit sum to 32 bits
    while (sum >> 32) {
        sum = (sum & 0xFFFFFFFF) + (sum >> 32);
    }
    
    return ~((uint32_t)sum);
}
#endif
```

### Atomic Operations for Network Statistics
```c
// RISC-V atomic statistics updates
#ifdef __riscv
static inline void atomic_inc_pkt_count(uint64_t *counter) {
    __asm__ __volatile__ (
        "amoswap.d.aqrl zero, zero, (%0)"
        : : "r" (counter)
        : "memory"
    );
}
#endif
```

## Advanced Network Features

### Multi-Interface Support
```c
// Support for multiple network interfaces
struct minix_net_config {
    struct netif *interfaces[MAX_INTERFACES];
    int interface_count;
    struct route_table *routes;
    struct firewall_rules *firewall;
};

int add_network_interface(struct minix_net_config *config,
                         const char *ifname, endpoint_t driver_ep) {
    struct netif *interface;
    
    if (config->interface_count >= MAX_INTERFACES) {
        return ENOMEM;
    }
    
    interface = malloc(sizeof(struct netif));
    if (interface == NULL) {
        return ENOMEM;
    }
    
    // Initialize interface
    netif_add(interface, IP_ADDR_ANY, IP_ADDR_ANY, IP_ADDR_ANY,
              NULL, ethernetif_init, tcpip_input);
    
    interface->driver_endpoint = driver_ep;
    interface->interface_index = config->interface_count;
    
    config->interfaces[config->interface_count++] = interface;
    
    return OK;
}
```

### Advanced Routing Capabilities
```c
// Sophisticated routing table implementation
struct route_entry {
    struct ip_addr destination;
    struct ip_addr netmask;
    struct ip_addr gateway;
    struct netif *interface;
    uint32_t metric;
    uint32_t flags;
    uint32_t refcount;
};

struct route_table {
    struct route_entry *entries;
    size_t count;
    size_t capacity;
    pthread_rwlock_t lock;
};

struct netif *route_lookup(struct route_table *table, 
                          struct ip_addr *dest) {
    struct route_entry *best = NULL;
    uint32_t best_metric = UINT32_MAX;
    size_t i;
    
    pthread_rwlock_rdlock(&table->lock);
    
    for (i = 0; i < table->count; i++) {
        struct route_entry *entry = &table->entries[i];
        
        if (ip_addr_netcmp(dest, &entry->destination, 
                          &entry->netmask)) {
            if (entry->metric < best_metric) {
                best = entry;
                best_metric = entry->metric;
            }
        }
    }
    
    pthread_rwlock_unlock(&table->lock);
    
    return best ? best->interface : NULL;
}
```

### Network Security Features
```c
// Packet filtering and firewall
struct firewall_rule {
    uint32_t rule_id;
    uint8_t protocol;
    struct ip_addr src_addr;
    struct ip_addr src_mask;
    struct ip_addr dst_addr;
    struct ip_addr dst_mask;
    uint16_t src_port_min, src_port_max;
    uint16_t dst_port_min, dst_port_max;
    uint8_t action; // ALLOW or DENY
    uint32_t flags;
};

int firewall_filter_packet(struct firewall_rules *rules,
                          struct pbuf *p, struct netif *inp) {
    struct ip_hdr *iphdr = (struct ip_hdr *)p->payload;
    struct firewall_rule *rule;
    size_t i;
    
    // Check each rule
    for (i = 0; i < rules->count; i++) {
        rule = &rules->rules[i];
        
        // Match protocol
        if (rule->protocol != 0 && 
            rule->protocol != iphdr->_proto) {
            continue;
        }
        
        // Match addresses
        if (!ip_addr_netcmp(&iphdr->src, &rule->src_addr, 
                           &rule->src_mask)) {
            continue;
        }
        
        if (!ip_addr_netcmp(&iphdr->dest, &rule->dst_addr, 
                           &rule->dst_mask)) {
            continue;
        }
        
        // Rule matched, return action
        return rule->action;
    }
    
    // Default allow
    return FIREWALL_ALLOW;
}
```

### Quality of Service (QoS)
```c
// Traffic prioritization and QoS
struct qos_class {
    uint32_t class_id;
    const char *name;
    uint32_t bandwidth;     // Bytes per second
    uint32_t priority;      // 0-7, higher is better
    uint32_t queue_limit;   // Maximum queue size
    uint32_t flags;
};

struct qos_interface {
    struct netif *interface;
    struct qos_class *classes;
    size_t class_count;
    struct pbuf_queue *queues;
    pthread_mutex_t lock;
};

int qos_enqueue_packet(struct qos_interface *qos,
                      struct pbuf *p, uint32_t class_id) {
    struct qos_class *class = NULL;
    struct pbuf_queue *queue;
    size_t i;
    
    // Find class
    for (i = 0; i < qos->class_count; i++) {
        if (qos->classes[i].class_id == class_id) {
            class = &qos->classes[i];
            break;
        }
    }
    
    if (class == NULL) {
        return EINVAL;
    }
    
    queue = &qos->queues[i];
    
    pthread_mutex_lock(&qos->lock);
    
    // Check queue limit
    if (queue->count >= class->queue_limit) {
        pthread_mutex_unlock(&qos->lock);
        return ENOBUFS;
    }
    
    // Enqueue packet
    pbuf_queue_enqueue(queue, p);
    
    pthread_mutex_unlock(&qos->lock);
    
    return OK;
}
```

## Advanced Development Features

### Network Debugging and Monitoring
```c
// Comprehensive network statistics
struct minix_net_stats {
    // Interface statistics
    uint64_t rx_packets, tx_packets;
    uint64_t rx_bytes, tx_bytes;
    uint64_t rx_errors, tx_errors;
    uint64_t rx_dropped, tx_dropped;
    
    // Protocol statistics
    uint64_t tcp_segments, tcp_retransmissions;
    uint64_t udp_datagrams, udp_errors;
    uint64_t icmp_messages, icmp_errors;
    uint64_t ip_packets, ip_fragments;
    
    // Performance metrics
    uint64_t average_latency;
    uint64_t peak_throughput;
    uint64_t connection_count;
};

int get_network_statistics(struct minix_net_stats *stats) {
    message m;
    
    m.m_type = LWIP_GET_STATS;
    m.LWIP_STATS_BUFFER = stats;
    m.LWIP_STATS_SIZE = sizeof(*stats);
    
    return _sendrec(LWIP_PROC_NR, &m);
}
```

### BPF (Berkeley Packet Filter) Integration
```c
// Packet filtering and capture
struct bpf_program {
    uint32_t bf_len;
    struct bpf_insn *bf_insns;
};

struct bpf_filter {
    struct bpf_program program;
    uint32_t snaplen;
    uint16_t type;
    uint16_t flags;
};

int bpf_attach_filter(struct netif *interface, 
                     struct bpf_filter *filter) {
    message m;
    
    m.m_type = BPF_ATTACH_FILTER;
    m.BPF_INTERFACE = interface->interface_index;
    m.BPF_PROGRAM = &filter->program;
    m.BPF_FILTER = filter;
    
    return _sendrec(LWIP_PROC_NR, &m);
}
```

## Build System Integration

### Network Stack Build Configuration
```makefile
# Advanced network features
CPPFLAGS+= -DLWIP_TCP_KEEPALIVE=1
CPPFLAGS+= -DLWIP_TCP_TIMESTAMPS=1
CPPFLAGS+= -DLWIP_TCP_SACK_OUT=1
CPPFLAGS+= -DLWIP_IPV6_FRAG_COPYHEADER=1
CPPFLAGS+= -DLWIP_IPV6_MLD=1
CPPFLAGS+= -DLWIP_IPV6_DHCP6=1

# RISC-V optimizations
.if ${MACHINE_ARCH} == "riscv64"
CPPFLAGS+= -DRISCV_CHECKSUM_OPTIMIZATION
CPPFLAGS+= -DRISCV_ATOMIC_STATS
.endif

# Debug and profiling
.if defined(DEBUG)
CPPFLAGS+= -DLWIP_DEBUG=1
CPPFLAGS+= -DLWIP_STATS_DISPLAY=1
CPPFLAGS+= -DLWIP_PERF=1
.endif
```

## Testing and Validation

### Network Protocol Testing
```c
// Comprehensive protocol testing
TEST(tcp_connection_test) {
    int sockfd;
    struct sockaddr_in addr;
    
    // Create TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT(sockfd >= 0);
    
    // Set up address
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TEST_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    
    // Test connection
    ASSERT(bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == 0);
    ASSERT(listen(sockfd, 5) == 0);
    
    close(sockfd);
}
```

### Performance Testing
```bash
# Network performance testing
./test_network_throughput    # Throughput measurement
./test_network_latency       # Latency measurement
./test_network_stress        # Stress testing
./test_network_scalability   # Scalability testing
```

## Common Development Issues

### Build Issues
- **Missing lwIP libraries**: Ensure liblwip is built
- **IPv6 compilation errors**: Check inet6 support configuration
- **BPF compilation issues**: Verify BPF filter support

### Runtime Issues
- **Network driver communication**: Verify driver endpoint configuration
- **Memory allocation failures**: Check network buffer pool configuration
- **Interface initialization**: Validate network interface registration

### Debugging Techniques
- Enable lwIP debug output
- Use network tracing tools
- Monitor network statistics
- Check message queue status

## Future Enhancements

### Planned Features
- Advanced wireless networking (802.11)
- Bluetooth networking support
- Advanced security protocols
- Network virtualization support
- SDN (Software Defined Networking) integration

### Architectural Evolution
- Support for high-speed networking (10GbE+)
- Enhanced network function virtualization
- Better integration with container networking
- Improved cloud networking support

This directory represents MINIX's comprehensive networking capabilities, demonstrating how a full-featured TCP/IP stack can be effectively integrated into a microkernel architecture while maintaining high performance, reliability, and advanced networking features.