/*
 * =====================================================================================
 *
 *       Filename:  eap_protocol.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/07/2009 02:55:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *       File Last Modify:$Date: 2009-10-13 01:24:24 +0800 (周二, 2009-10-13) $
 *
 *         Author:  BOYPT (PT), pentie@gmail.com
 *        Company:  http://apt-blog.co.cc
 *
 * =====================================================================================
 */

#ifndef EAP_PROTOCOL_H
#define EAP_PROTOCOL_H

#include	"commondef.h"

void
action_eapol_success(const struct eap_header *eap_head,
                        const struct pcap_pkthdr *packetinfo,
                        const uint8_t *packet);

void
action_eapol_failre(const struct eap_header *eap_head,
                        const struct pcap_pkthdr *packetinfo,
                        const uint8_t *packet);

void
action_eap_req_idnty(const struct eap_header *eap_head,
                        const struct pcap_pkthdr *packetinfo,
                        const uint8_t *packet);

void
action_eap_req_md5_chg(const struct eap_header *eap_head,
                        const struct pcap_pkthdr *packetinfo,
                        const uint8_t *packet);
void 
print_server_info (const uint8_t *packet);

void
print_notification_msg(const uint8_t *packet);

void 
send_eap_packet(enum EAPType send_type);

void
keep_alive(const struct eap_header *eap_head,
                        const struct pcap_pkthdr *packetinfo,
                        const uint8_t *packet);
#endif

