/*
 * =====================================================================================
 *
 *       Filename:  zruijie.c
 *
 *    Description:  
 *
 *        Version:  0.1
 *        Created:  07/06/2009 08:07:12 AM
 *       Revision:  none
 *       Compiler:  gcc
 *       File Last Modify:$Date: 2009-11-05 21:28:17 +0800 (Thu, 05 Nov 2009) $
 *
 *         Author:  BOYPT (PT), pentie@gmail.com
 *        Company:  http://apt-blog.co.cc
 *
 * =====================================================================================
 */
#include <assert.h>
#include "zte.h"
#include "eap_protocol.h"

/* #####   GLOBLE VAR DEFINITIONS   ######################### */
/*-----------------------------------------------------------------------------
 *  程序的主控制变量
 *-----------------------------------------------------------------------------*/
char        errbuf[PCAP_ERRBUF_SIZE];   /* error buffer */
pcap_t      *handle;			        /* packet capture handle */
enum STATE  state;                      /* program state */
//pthread_t   live_keeper_id;             /*保鲜报文线程id*/
//pthread_t   exit_waiter_id;
uint8_t     muticast_mac[] =            /* Star认证服务器多播地址 */
                      //  {0x01, 0xd0, 0xf8, 0x00, 0x00, 0x03};
                      //{0x00, 0x1a, 0xa9, 0x19, 0x23, 0xd4};
			{0x01, 0x80, 0xC2, 0x00, 0x00, 0x03};

/* #####   GLOBLE VAR DEFINITIONS   ###################
 *-----------------------------------------------------------------------------
 *  用户信息的赋值变量，由init_argument函数初始化
 *-----------------------------------------------------------------------------*/
int         background = 0;            /* 后台运行标记  */
int         exit_flag = 0;
char        *dev = NULL;               /* 连接的设备名 */
char        *username = NULL;          
char        *password = NULL;

/* #####   GLOBLE VAR DEFINITIONS   ######################### 
 *-----------------------------------------------------------------------------
 *  报文相关信息变量，由init_info 、init_device函数初始化。
 *-----------------------------------------------------------------------------*/
int         username_length;
int         password_length;
uint8_t     local_mac[ETHER_ADDR_LEN]; /* MAC地址 */
char        dev_if_name[64];


// debug function
void 
print_hex(const uint8_t *array, int count)
{
    int i;
    for(i = 0; i < count; i++){
        if ( !(i % 16))
            fprintf (stderr, "\n");
        fprintf(stderr, "%02x ", array[i]);
    }
    fprintf(stderr, "\n");
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_packet
 *  Description:  pcap的回呼函数，当收到EAPOL报文时自动被调用
 * =====================================================================================
 */
void
get_packet(uint8_t *args, const struct pcap_pkthdr *pcaket_header, 
    const uint8_t *packet)
{
	/* declare pointers to packet headers */
	const struct ether_header *ethernet;  /* The ethernet header [1] */
    const struct eap_header *eap_header;
    enum EAPType p_type;

    ethernet = (struct ether_header*)(packet);
    eap_header = (struct eap_header *)(packet + SIZE_ETHERNET);

    p_type = get_eap_type(eap_header);
    if (p_type != ERROR)
        action_by_eap_type(p_type, eap_header, pcaket_header, packet);
    return;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_eap_type
 *  Description:  根据报文的动作位返回enum EAPType内定义的报文类型
 * =====================================================================================
 */
enum EAPType 
get_eap_type(const struct eap_header *eap_header) 
{
    if(eap_header->eapol_t == EAPOL_KEY) {
	return EAP_KEEP_ALIVE;
    }
    switch (eap_header->eap_t){
        case 0x00:
            if ( eap_header->eap_op == 0x1c &&
                        eap_header->eap_id == 0x00)
                return RUIJIE_EAPOL_MSG;
            break;
        case 0x01:
            if ( eap_header->eap_op == 0x01)
                return EAP_REQUEST_IDENTITY;
            if ( eap_header->eap_op == 0x04)
                return EAP_REQUETS_MD5_CHALLENGE;
            break;
        case 0x03:
            return EAP_SUCCESS;
            break;
        case 0x04:
            return EAP_FAILURE;
    }
    fprintf (stderr, "&&IMPORTANT: Unknown Package : eap_t :    %02x\n"
                     "                               eap_id:    %02x\n"
                     "                               eap_op:    %02x\n", 
                    eap_header->eap_t, eap_header->eap_id,
                    eap_header->eap_op);
    return ERROR;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  action_by_eap_type
 *  Description:  根据eap报文的类型完成相关的应答
 * =====================================================================================
 */
void 
action_by_eap_type(enum EAPType pType, 
                        const struct eap_header *eap_head,
                        const struct pcap_pkthdr *packetinfo,
                        const uint8_t *packet) {
    switch(pType){
        case EAP_SUCCESS:
            action_eapol_success (eap_head, packetinfo, packet);
            break;
        case EAP_FAILURE:
            action_eapol_failre (eap_head, packetinfo, packet);
            break;
        case EAP_REQUEST_IDENTITY:
            action_eap_req_idnty (eap_head, packetinfo, packet);
            break;
        case EAP_REQUETS_MD5_CHALLENGE:
            action_eap_req_md5_chg (eap_head, packetinfo, packet);
            break;
        case RUIJIE_EAPOL_MSG:
            print_notification_msg (packet);
            break;
	case EAP_KEEP_ALIVE:
	    keep_alive(eap_head,packetinfo,packet);
	    break;
        default:
            return;
    }
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init_info
 *  Description:  初始化本地信息。（字符串->二进制数值）
 * =====================================================================================
 */
void init_info()
{
    extern uint32_t  ruijie_live_serial_num;
    if(username == NULL || password == NULL){
        fprintf (stderr,"Error: NO Username or Password prompted.\n"
                        "Try --help for usage.\n");
        exit(EXIT_FAILURE);
    }
    username_length = strlen(username);
    password_length = strlen(password);

    ruijie_live_serial_num = 0x0000102b;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init_device
 *  Description:  初始化设备。主要是找到打开网卡、获取网卡MAC、IP，
 *  同时设置pcap的初始化工作句柄。
 * =====================================================================================
 */
void init_device()
{
    struct          bpf_program fp;			/* compiled filter program (expression) */
    char            filter_exp[51];         /* filter expression [3] */
    pcap_if_t       *alldevs;
//    pcap_addr_t     *addrs;

	/* Retrieve the device list */
	if(pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		fprintf(stderr,"FATIAL Error: pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

    /* 自动选择第一块设备 */
    if(dev == NULL) {
        pcap_if_t   *d;
        for (d = alldevs; d; d = d->next) {

            /* Skip loopback device */
            if (d->flags & PCAP_IF_LOOPBACK)
                continue;

            dev = d->name;         
            break;
        }
    }

    if (dev == NULL){
        fprintf(stderr, "FATIAL ERROR: No suitable device found. Use --dev to select one. \n");
        exit(EXIT_FAILURE);
    }
    else {
        strcpy (dev_if_name, dev);
    }

	/* open capture device */
	handle = pcap_open_live(dev, SNAP_LEN, 1, 1000, errbuf);

	if (handle == NULL) {
		fprintf(stderr, "FATIAL ERROR: Couldn't open device %s: %s\n", dev, errbuf);
		exit(EXIT_FAILURE);
	}

	/* make sure we're capturing on an Ethernet device [2] */
	if (pcap_datalink(handle) != DLT_EN10MB) {
		fprintf(stderr, "FATIAL ERROR: %s is not an Ethernet\n", dev);
		exit(EXIT_FAILURE);
	}

#ifdef SIOCGIFHWADDR

    /* get device basic infomation */
    struct ifreq ifr;
    int sock;
    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    strcpy(ifr.ifr_name, dev);

    //获得网卡Mac
    if(ioctl(sock, SIOCGIFHWADDR, &ifr) < 0)
    {
        perror("ioctl");
        exit(EXIT_FAILURE);
    }
    memcpy(local_mac, ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN);
#else
    if (bsd_get_mac (dev, local_mac) != 0) {
		fprintf(stderr, "FATIL: Fail getting BSD/MACOS Mac Address.\n");
		exit(EXIT_FAILURE);
    }
#endif

    /* construct the filter string */
    sprintf(filter_exp, "ether dst %02x:%02x:%02x:%02x:%02x:%02x"
                        " and ether proto 0x888e", 
                        local_mac[0], local_mac[1],
                        local_mac[2], local_mac[3],
                        local_mac[4], local_mac[5]);

	/* compile the filter expression */
	if (pcap_compile(handle, &fp, filter_exp, 0, 0) == -1) {
		fprintf(stderr, "FATIAL ERROR:Couldn't parse filter %s: %s\n",
		    filter_exp, pcap_geterr(handle));
		exit(EXIT_FAILURE);
	}

	/* apply the compiled filter */
	if (pcap_setfilter(handle, &fp) == -1) {
		fprintf(stderr, "FATIAL ERROR:Couldn't install filter %s: %s\n",
		    filter_exp, pcap_geterr(handle));
		exit(EXIT_FAILURE);
	}
    pcap_freecode(&fp);
    pcap_freealldevs(alldevs);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init_frames
 *  Description:  初始化发送帧的数据缓冲区
 * =====================================================================================
 */
void 
init_frames()
{
    extern uint8_t      eapol_start[];        /* EAPOL START报文 */
    extern uint8_t      eapol_logoff[];       /* EAPOL LogOff报文 */
    extern uint8_t      eap_response_ident[]; /* EAP RESPON/IDENTITY报文 */
    extern uint8_t      eap_response_md5ch[]; /* EAP RESPON/MD5 报文 */
    extern uint8_t      eap_life_keeping[];   /* EAPOL KEEP ALIVE*/
    int                 data_index;


    /*****  EAPOL Header  *******/
    uint8_t eapol_eth_header[SIZE_ETHERNET];
    struct ether_header *eth = (struct ether_header *)eapol_eth_header;
    memcpy (eth->ether_dhost, muticast_mac, ETHER_ADDR_LEN);
    memcpy (eth->ether_shost, local_mac, ETHER_ADDR_LEN);
    eth->ether_type =  htons (0x888e);

    /**** EAPol START ****/
    uint8_t start_data[4] = {0x01, 0x01, 0x00, 0x00};
    memset (eapol_start, 0, 1000);
    memcpy (eapol_start, eapol_eth_header, SIZE_ETHERNET);
    memcpy (eapol_start + OFFSET_EAPOL, start_data, sizeof(start_data));

    /****EAPol LOGOFF ****/
    uint8_t logoff_data[4] = {0x01, 0x02, 0x00, 0x00};
    memset (eapol_logoff, 0, 1000);
    memcpy (eapol_logoff, eapol_eth_header, SIZE_ETHERNET);
    memcpy (eapol_logoff + OFFSET_EAPOL, logoff_data, sizeof(logoff_data));

    /* EAP RESPONSE IDENTITY */
    uint8_t eap_resp_iden_head[9] = {0x01, 0x00, 
                                    0x00, 0x00, /* eapol_length */
                                    0x02, 0x00, 
                                    0x00, 0x00, /* eap_length */
                                    0x01};
    eap_resp_iden_head[3] = 5 + (uint8_t)username_length;
    eap_resp_iden_head[7] = 5 + (uint8_t)username_length;
    memset (eap_response_ident, 0, 1000);
    memcpy (eap_response_ident, eapol_eth_header, SIZE_ETHERNET);    
    memcpy (eap_response_ident + SIZE_ETHERNET, eap_resp_iden_head, 9);
    memcpy (eap_response_ident + SIZE_ETHERNET + 9, username, username_length);


    /** EAP RESPONSE MD5 Challenge **/
    uint8_t eap_resp_md5_head[10] = {0x01, 0x00, 
                                   0x00, 0x00,/* eapol_length */
                                   0x02, 0x36, 
                                   0x00, 0x00,/* eap-length */
                                   0x04, 0x10};
    eap_resp_md5_head[3] = 22 + (uint8_t)username_length;
    eap_resp_md5_head[7] = 22 + (uint8_t)username_length;
    memset (eap_response_md5ch, 0, 1000);
    memcpy (eap_response_md5ch, eapol_eth_header, SIZE_ETHERNET);
    memcpy (eap_response_md5ch + SIZE_ETHERNET, eap_resp_md5_head, 10);
    // 剩余16位在收到REQ/MD5报文后由fill_password_md5填充 
    memcpy (eap_response_md5ch + SIZE_ETHERNET + 10 + 16, username, username_length);

    /* LIFE KEEP PACKET */    
    uint8_t eapol_keep_alive[52]  = { 
0x01, //version
0x03, //type
0x00, 0x30, //length
0x01,//decript type
0x00, 0x04,//key length
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//Replay Counter: 8字节
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16字节的Key IV 前8字节
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16字节的Key IV 后8字节
0x00,				//index	
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16字节的Key Signature 前8字节
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16字节的Key Signature 后8字节
0x00,0x00,0x00,0x00							//4字节的Key
};
    memset (eap_life_keeping, 0, 1000);
    memcpy (eap_life_keeping, eapol_eth_header, SIZE_ETHERNET);
    memcpy (eap_life_keeping + SIZE_ETHERNET, eapol_keep_alive, 52);	

}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  show_local_info
 *  Description:  显示信息
 * =====================================================================================
 */
void 
show_local_info ()
{
    char    buf[32];
    printf("##### ZTE for GDST ver. %s ######\n", ZTE_VER );
    printf("Device:     %s\n", dev_if_name);
    printf("MAC:        %02x:%02x:%02x:%02x:%02x:%02x\n",
                        local_mac[0],local_mac[1],local_mac[2],
                        local_mac[3],local_mac[4],local_mac[5]);
    printf("######################################\n");
}



#ifndef SIOCGIFHWADDR
static int bsd_get_mac(const char ifname[], uint8_t eth_addr[])
{
    struct ifreq *ifrp;
    struct ifconf ifc;
    char buffer[720];
    int socketfd,error,len,space=0;
    ifc.ifc_len=sizeof(buffer);
    len=ifc.ifc_len;
    ifc.ifc_buf=buffer;

    socketfd=socket(AF_INET,SOCK_DGRAM,0);

    if((error=ioctl(socketfd,SIOCGIFCONF,&ifc))<0)
    {
        perror("ioctl faild");
        exit(1);
    }
    if(ifc.ifc_len<=len)
    {
        ifrp=ifc.ifc_req;
        do
        {
            struct sockaddr *sa=&ifrp->ifr_addr;
            
            if(((struct sockaddr_dl *)sa)->sdl_type==IFT_ETHER) {
                if (strcmp(ifname, ifrp->ifr_name) == 0){
                    memcpy (eth_addr, LLADDR((struct sockaddr_dl *)&ifrp->ifr_addr), 6);
                    return 0;
                }
            }
            ifrp=(struct ifreq*)(sa->sa_len+(caddr_t)&ifrp->ifr_addr);
            space+=(int)sa->sa_len+sizeof(ifrp->ifr_name);
        }
        while(space<ifc.ifc_len);
    }
    return 1;
}
#endif
