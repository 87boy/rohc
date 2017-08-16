#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <netinet/ip.h>

#include <rohc/rohc.h>
#include <rohc/rohc_buf.h>
#include <rohc/rohc_comp.h>
#include <rohc/rohc_decomp.h>

#define BUFFER_SIZE 2048

#define FAKE_PAYLOAD "Hello, ROHC world!"

static int gen_random_num(const struct rohc_comp *const comp,
                        void *const user_context)
{
    return rand();
}

static void dump_packet(const struct rohc_buf packet)
{
    size_t i;

    for(i = 0; i < packet.len; i++)
    {
        printf("0x%02x ", rohc_buf_byte_at(packet, i));
        if(i != 0 && ((i + 1) % 8) == 0)
        {
            printf("\n");
        }
    }
    if(i != 0 && (i % 8) != 0) /* be sure to go to the line */
    {
         printf("\n");
    }
}

int main(int argc, char **argv)
{
    struct rohc_comp *compressor;
    struct rohc_decomp *decompressor;

    uint8_t ip_buffer_comp[BUFFER_SIZE];
    struct rohc_buf ip_packet_comp = rohc_buf_init_empty(ip_buffer_comp, BUFFER_SIZE);
    struct iphdr *ip_header;

    uint8_t ip_buffer_decomp[BUFFER_SIZE];
    struct rohc_buf ip_packet_decomp = rohc_buf_init_empty(ip_buffer_decomp, BUFFER_SIZE);
    struct rohc_buf *rcvd_feedback = NULL;
    struct rohc_buf *feedback_send = NULL;

    uint8_t rohc_buffer[BUFFER_SIZE];
    struct rohc_buf rohc_packet = rohc_buf_init_empty(rohc_buffer, BUFFER_SIZE);
    
    rohc_status_t rohc_status;
    uint8_t seed;

    seed = time(NULL);
    srand(seed);
    printf("This program will compress one single IPv4 packet\n");
    printf(FAKE_PAYLOAD);
    printf("\nCreate the ROHC compressor\n");
    compressor = rohc_comp_new2(ROHC_SMALL_CID, ROHC_SMALL_CID_MAX, gen_random_num, NULL); 
    if (compressor == NULL)
    {
        fprintf(stderr, "failed create the ROHC compressor\n");
        return 1;
    }

    printf("Enable the IP-only compression profile\n");
    if (!rohc_comp_enable_profile(compressor, ROHC_PROFILE_IP))
    {
        fprintf(stderr, "failed to enable the IP-only profile\n");
        rohc_comp_free(compressor);
        return 1;
    }
    
    printf("Build a fake IP packet\n");
    ip_header = (struct iphdr *) rohc_buf_data(ip_packet_comp);
    ip_header->version = 4;
    ip_header->ihl = 5;
    ip_packet_comp.len += ip_header->ihl * 4;
    ip_header->tos = 0;
    ip_header->tot_len = htons(ip_packet_comp.len + strlen(FAKE_PAYLOAD));
    ip_header->id = 0;
    ip_header->frag_off = 0;
    ip_header->ttl = 1;
    ip_header->protocol = 134;
    ip_header->check = 0x3fa9;
    ip_header->saddr = htonl(0x01020304);
    ip_header->daddr = htonl(0x05060708);

    rohc_buf_append(&ip_packet_comp, (uint8_t *) FAKE_PAYLOAD, strlen(FAKE_PAYLOAD));

    dump_packet(ip_packet_comp);

    printf("Compress the fake IP packet\n");
    rohc_status = rohc_compress4(compressor, ip_packet_comp, &rohc_packet);
    if(rohc_status != ROHC_STATUS_OK)
    {
        fprintf(stderr, "compression of fake IP packet failed: %s (%d)\n",
                rohc_strerror(rohc_status), rohc_status);
        /* cleanup compressor, then leave with an error code */
        rohc_comp_free(compressor);
        return 1;
    }

    /* dump the ROHC packet on terminal */
    printf("ROHC packet resulting from the ROHC compression:\n");
    dump_packet(rohc_packet);

    /* release the ROHC compressor when you do not need it anymore */
    printf("Destroy the ROHC compressor\n");
    rohc_comp_free(compressor);

    printf("\nCreate the ROHC decompressor\n");
    decompressor = rohc_decomp_new2(ROHC_SMALL_CID, ROHC_SMALL_CID_MAX, ROHC_U_MODE);
    if (decompressor == NULL)
    {
        fprintf(stderr, "failed create the ROHC decompressor\n");
        return 1;
    }

    printf("Enable the IP-only decompression profile\n");
    if (!rohc_decomp_enable_profile(decompressor, ROHC_PROFILE_IP))
    {
        fprintf(stderr, "failed to enable the IP-only profile\n");
        rohc_decomp_free(decompressor);
        return 1;
    }

    printf("Decompress the ROHC packet\n");
    rohc_status = rohc_decompress3(decompressor, rohc_packet, &ip_packet_decomp,
                            rcvd_feedback, feedback_send);

    if (rohc_status != ROHC_STATUS_OK)
    {
        fprintf(stderr, "decompression of ROHC packet failed: %s (%d)\n",
                rohc_strerror(rohc_status), rohc_status);
        rohc_decomp_free(decompressor);
        return 1;
    }
    else
    {
        if (!rohc_buf_is_empty(ip_packet_decomp))
        {
            printf("IP packet resulting from the ROHC decompression:\n");
            dump_packet(ip_packet_decomp);
            size_t idx;
            for (idx=20; idx<ip_packet_decomp.len; ++idx)
            {
                printf("%c", rohc_buf_byte_at(ip_packet_decomp, idx));
            }
            printf("\n");
        }
        else
        {
            printf("no IP packet decompressed");
        }
    }

    printf("Destroy the ROHC decompressor\n");
    rohc_decomp_free(decompressor);
    printf("The program ended successfully.\n");
    /* leave the program with a success code */
    return 0;
}

