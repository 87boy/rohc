
#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <netinet/ip.h>

#if HAVE_WINSOCK2_H == 1
#include <winsock2.h>
#endif

#if HAVEARPA_INET_H == 1
#include <arpa/inet.h>
#endif

#include <errno.h>
#include <assert.h>
#include <stdarg.h>
#include <limits.h>

#include <ip.h>
#include <protocols/ipv6.h>
#include <protocols/tcp.h>

#include <rohc/rohc.h>
#include <rohc/rohc_buf.h>
#include <rohc/rohc_comp.h>
#include <rohc/rohc_decomp.h>

#define BUFFER_SIZE 2048
#define FAKE_PAYLOAD "Hello, ROHC world!"

static int is_verbose = 0;

static int gen_random_num(const struct rohc_comp *const comp,
                        void *const user_context)
{
    return rand();
}

static void dump_packet(const struct rohc_buf packet)
{
    size_t i;
    for (i = 0; i < packet.len; i++)
    {
        printf("%02X ", rohc_buf_byte_at(packet, i));
    }
    printf("\n");
}

static void print_rohc_traces(void *const priv_ctxt __attribute__((unused)),
                              const rohc_trace_level_t level,
                              const rohc_trace_entity_t entity __attribute__((unused)),
                              const int profile __attribute__((unused)),
                              const char *const format,
                              ...)
{
    if(level >= ROHC_TRACE_WARNING || is_verbose)
    {
        const char *level_descrs[] =
        {
            [ROHC_TRACE_DEBUG]   = "DEBUG",
            [ROHC_TRACE_INFO]    = "INFO",
            [ROHC_TRACE_WARNING] = "WARNING",
            [ROHC_TRACE_ERROR]   = "ERROR"
        };
        va_list args;
        fprintf(stdout, "[%s] ", level_descrs[level]);
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
    }
}

int main(int argc, char *argv[])
{
    struct rohc_comp *compressor = NULL;
    rohc_status_t rohc_status;

    uint8_t max_packets = 5;
    unsigned long counter;

    printf("This program will compress several IPv6 packets\n");
    printf("\nCreate the ROHC compressor\n");
    srand(time(NULL));
    compressor = rohc_comp_new2(ROHC_LARGE_CID, ROHC_LARGE_CID_MAX, gen_random_num, NULL); 
    if (compressor == NULL)
    {
        fprintf(stderr, "failed to create the ROHC compressor\n");
        return 1;
    }

    if (!rohc_comp_set_traces_cb2(compressor, print_rohc_traces, NULL))
    {
        fprintf(stderr, "failed to set the callback for traces on compressor\n");
        rohc_comp_free(compressor);
        return 1;
    }

    printf("Enable the IP-only compression profile\n");
    if (!rohc_comp_enable_profile(compressor, ROHC_PROFILE_IP))
    {
        fprintf(stderr, "failed to enable the IP-only profile\n");
        rohc_comp_free(compressor);
        return 1;
    }
    printf("ipv6_hdr size:%u\n", (uint32_t)sizeof(struct ipv6_hdr));
    
    for (counter=1; counter<=max_packets; ++counter)
    {
        //printf("Build a fake IP packet\n");
        const size_t payload_len = 10;
        const size_t packet_len = sizeof(struct ipv6_hdr) 
                                + sizeof(struct tcphdr)
                                + payload_len;
        uint8_t ip_buffer[packet_len];
        struct rohc_buf ip_packet = rohc_buf_init_empty(ip_buffer, packet_len);
        
        const size_t rohc_max_len = packet_len * 2;
        uint8_t rohc_buffer[rohc_max_len];
        struct rohc_buf rohc_packet = rohc_buf_init_empty(rohc_buffer, rohc_max_len);
        
        struct ipv6_hdr *ipv6;
        struct tcphdr *tcp;

        ip_packet.len += sizeof(struct ipv6_hdr);
        ipv6 = (struct ipv6_hdr *) rohc_buf_data(ip_packet);
        ipv6->version = 6;
        ipv6->tc1 = 0;
        ipv6->tc2 = 0;
        ipv6->flow1 = 0;
        ipv6->flow2 = htons(0);
        ipv6->plen = htons(sizeof(struct tcphdr) + payload_len);
        ipv6->nh = IPPROTO_TCP;
        ipv6->hl = 64;
        uint8_t idx;
        for (idx=0; idx<4; ++idx)
        {
            ipv6->saddr.u32[idx] = htonl(0xc0a80001);
            ipv6->daddr.u32[idx] = htonl(0xc0a80002);
        }
        rohc_buf_pull(&ip_packet, sizeof(struct ipv6_hdr));
        
        ip_packet.len += sizeof(struct tcphdr);
        tcp = (struct tcphdr *) rohc_buf_data(ip_packet);
        tcp->src_port = htons(5678);
        tcp->dst_port = htons(5678);
        tcp->data_offset = sizeof(struct tcphdr) / 4;
        tcp->rsf_flags = 1 << 1;
        tcp->window = 4096;
        tcp->checksum = 0;
        rohc_buf_pull(&ip_packet, sizeof(struct tcphdr));

        size_t i;
        for (i=0; i<payload_len; ++i)
        {
            rohc_buf_byte_at(ip_packet, i) = i % 0xff;
        }
        ip_packet.len += payload_len;
        rohc_buf_pull(&ip_packet, payload_len);
        
        rohc_buf_push(&ip_packet, packet_len);
        dump_packet(ip_packet);

        //printf("Compress the fake IP packet\n");
        rohc_status = rohc_compress4(compressor, ip_packet, &rohc_packet);
        if(rohc_status != ROHC_STATUS_OK)
        {
            fprintf(stderr, "compression of fake IP packet failed: %s (%d)\n",
                    rohc_strerror(rohc_status), rohc_status);
            /* cleanup compressor, then leave with an error code */
            rohc_comp_free(compressor);
            return 1;
        }
        /* dump the ROHC packet on terminal */
        //printf("ROHC packet resulting from the ROHC compression:\n");
        dump_packet(rohc_packet);
    }
    /* release the ROHC compressor when you do not need it anymore */
    printf("Destroy the ROHC compressor\n");
    rohc_comp_free(compressor);

    printf("The program ended successfully.\n");
    /* leave the program with a success code */
    return 0;
}

