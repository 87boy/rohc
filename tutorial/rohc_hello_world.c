#include <stdio.h>  /* for the printf() function */
/* includes required to create a fake IP packet */
#include <netinet/ip.h>     /* for the IPv4 header */
#include <string.h>         /* for strlen() */
#include <rohc/rohc_buf.h>  /* for the rohc_buf_*() functions */ 
/* includes required to use the compression part of the ROHC library */
#include <time.h>             /* required by time() */
#include <rohc/rohc_comp.h>   /* for rohc_comp_*() functions */

/* The size (in bytes) of the buffers used in the program */
#define BUFFER_SIZE 2048
/* The payload for the fake IP packet */
#define FAKE_PAYLOAD "hello, ROHC world!"
 
/* return a random number every time it is called */
static int gen_random_num(const struct rohc_comp *const comp,
                          void *const user_context)
{
    return rand();
}
 
/* The main entry point of the program (arguments are not used) */
int main(int argc, char **argv)
{
    struct rohc_comp *compressor;  /* the ROHC compressor */

    /* the buffer that will contain the IPv4 packet to compress */
    uint8_t ip_buffer[BUFFER_SIZE];
    struct rohc_buf ip_packet = rohc_buf_init_empty(ip_buffer, BUFFER_SIZE);
    /* the header of the IPv4 packet */
    struct iphdr *ip_header;

    /* the buffer that will contain the resulting ROHC packet */
    uint8_t rohc_buffer[BUFFER_SIZE];
    struct rohc_buf rohc_packet = rohc_buf_init_empty(rohc_buffer, BUFFER_SIZE);
 
    rohc_status_t rohc_status;
    size_t i;
 
    /* print the purpose of the program on the console */
    printf("This program will compress one single IPv4 packet\n");
 
    /* initialize the random generator with the current time */
    srand(time(NULL));
 
    /* create a ROHC compressor with good default parameters */
    printf("create the ROHC compressor\n");
    compressor = rohc_comp_new2(ROHC_SMALL_CID, ROHC_SMALL_CID_MAX,
                                gen_random_num, NULL);
    if(compressor == NULL)
    {
        fprintf(stderr, "failed create the ROHC compressor\n");
        /* leave with an error code */
        return 1;
    }

    /* enable the IP-only compression profile */
    printf("enable the IP-only compression profile\n");
    if(!rohc_comp_enable_profile(compressor, ROHC_PROFILE_IP))
    {
        fprintf(stderr, "failed to enable the IP-only profile\n");
        /* cleanup compressor, then leave with an error code */
        rohc_comp_free(compressor);
        return 1;
     }

    /* create a fake IP packet for the purpose of this simple program */
    printf("build a fake IP packet\n");
    ip_header = (struct iphdr *) rohc_buf_data(ip_packet);
    ip_header->version = 4; /* we create an IP header version 4 */
    ip_header->ihl = 5;     /* min. IPv4 header length (in 32-bit words) */
    ip_packet.len += ip_header->ihl * 4;
    ip_header->tos = 0;     /* TOS is not important for the example */
    ip_header->tot_len = htons(ip_packet.len + strlen(FAKE_PAYLOAD));
    ip_header->id = 0;      /* ID is not important for the example */
    ip_header->frag_off = 0;/* No packet fragmentation */
    ip_header->ttl = 1;     /* TTL is not important for the example */
    ip_header->protocol = 134;  /* protocol number */
    ip_header->check = 0x3fa9;   /* checksum */
    ip_header->saddr = htonl(0x01020304); /* source address 1.2.3.4 */
    ip_header->daddr = htonl(0x05060708); /* destination addr. 5.6.7.8 */
 
    /* copy the payload just after the IP header */
    rohc_buf_append(&ip_packet, (uint8_t *) FAKE_PAYLOAD, strlen(FAKE_PAYLOAD));
 
    /* dump the newly-created IP packet on terminal */
    for(i = 0; i < ip_packet.len; i++)
    {
        printf("0x%02x ", rohc_buf_byte_at(ip_packet, i));
        if(i != 0 && ((i + 1) % 8) == 0)
        {
            printf("\n");
        }
    }
    if(i != 0 && (i % 8) != 0) /* be sure to go to the line */
    {
         printf("\n");
    }

    /* compress the fake IP packet */
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
    printf("ROHC packet resulting from the ROHC compression:\n");
    for(i = 0; i < rohc_packet.len; i++)
    {
        printf("0x%02x ", rohc_buf_byte_at(rohc_packet, i));
        if(i != 0 && ((i + 1) % 8) == 0)
        {
             printf("\n");
        }
    }
    if(i != 0 && (i % 8) != 0) /* be sure to go to the line */
    {
         printf("\n");
    }

    /* release the ROHC compressor when you do not need it anymore */
    printf("destroy the ROHC compressor\n");
    rohc_comp_free(compressor);

    /* leave the program with a success code */
    return 0;
}
