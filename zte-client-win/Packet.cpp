
/**************************************************************************************
			The Luzj's Zte Project
			//////////////////////
			Copyleft ? 2009 Luzj
		Author:Luzj		QQ:86829232
		http://blog.csdn.net/luzjqq
		Email: luzjcn@gmail.com
	///////////////////////////////////
关于Luzj's Zte认证端的声明：

1、本软件所有涉及的中兴认证的功能的实现均是通过黑盒分析得来，并未通过任何不正当方法获得。

2、本软件仅供研究学习之用，不得使用本软件损害中兴公司商业利益。

3、本软件不可用于任何商业和非法用途，否则责任自负。

4、本软件在发布前均通过一般性应用测试，但不保证任何情况下对机器无害，
由于未知的使用环境或不当的使用对计算机造成的损害，责任由使用者全部承担。

5.本软件版权没有，翻印不究，但请协助改进本作品。

6.本软件属开源软件，如果需要修改本软件源码以进行二次发布，也请继续公开源代码。

由于任何不遵守上叙条例引起的纠纷，均与本人无关，如不同意该声明请不要使用该软件，谢谢合作。
**************************************************************************************/


// Packet.cpp: implementation of the CPacket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Luzj_ZTE.h"
#include "Packet.h"
#include "md5.h"
#include "rc4.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPacket::CPacket()
{

}

CPacket::~CPacket()
{

}

//======================packet_start======================================
int CPacket::send_packet_start(pcap_t * adapterHandle,u_char* MacAdd)
{
	u_char packet_start[]={
					0x01,0x80,0xc2,0x00,0x00,0x03,			//对方MAC
					0x00,0x00,0x00,0x00,0x00,0x00,			//自己MAC
					0x88,0x8e,0x01,0x01,0x00,0x00	};

	memcpy(packet_start+6,MacAdd,6);

	if(pcap_sendpacket(adapterHandle, packet_start,18)!=0)
    {
        return 0;
    }
	return 1;
}
//======================packet_start======================================


//======================packet_logoff======================================
int CPacket::send_packet_logoff(pcap_t * adapterHandle,u_char* MacAdd)
{
	u_char packet_logoff[100]={
		0x01,0x80,0xc2,0x00,0x00,0x03,			//对方MAC
		0x00,0x00,0x00,0x00,0x00,0x00,						//自己MAC
		0x88,0x8e,0x01,0x02,0x00,0x00	};

	memcpy(packet_logoff+6,MacAdd,6);

	if(pcap_sendpacket(adapterHandle, packet_logoff,18)!=0)
    {
        return 0;
    }
	return 1;
}
//======================packet_logoff======================================


//======================packet_response_Identity======================================
int CPacket::send_packet_response_Identity(pcap_t * adapterHandle,const u_char * captured,u_char* MacAdd
											,u_char* m_username,int m_usernameLen)
{
	u_char packet_response_Identity[100]={	
					/*0*/0x01,0x80,0xc2,0x00,0x00,0x03,			//对方MAC
					/*6*/0x00,0x00,0x00,0x00,0x00,0x00,			//自己MAC
					/*12*/0x88,0x8e,								//网络协议类型
					/*14*/0x01,									//Version: 1
					/*15*/0x00,									//Type: EAP Packet (0)
					/*16*/0x00,0x00,								//长度，十六进制11代表10进制的17
					/*18*/0x02,									//Code: Response (2)
					/*19*/0x00,									//Id: 由发来的包决定
					/*20*/0x00,0x00,								//Length: 17
					/*22*/0x01,									//Type: Identity [RFC3748] (1)
					/*23*/0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00  //存放自己的账号，是ascii码
				};
	packet_response_Identity[17] = (u_char)m_usernameLen + 5;
	packet_response_Identity[21] = (u_char)m_usernameLen + 5;

	memcpy(packet_response_Identity+6,MacAdd,6);

	//来自request包的id
	packet_response_Identity[19]=captured[19];

	//设置登录的账号
	for (int i=0;i<m_usernameLen;i++)
	{
		packet_response_Identity[23+i]=m_username[i];
	}

	if(pcap_sendpacket(adapterHandle, packet_response_Identity,23+m_usernameLen)!=0)
    {
        return 0;
    }
	return 1;
}
//======================packet_response_Identity======================================


//======================packet_response_MD5======================================
int CPacket::send_packet_response_MD5(pcap_t * adapterHandle,const u_char* captured,u_char* MacAdd
									   ,u_char*	m_username,int m_usernameLen,u_char* m_password,int	m_passwordLen)
{	
	u_char packet_response_MD5[100]={	
					/*0*/0x01,0x80,0xc2,0x00,0x00,0x03,			//对方MAC
					/*6*/0x00,0x00,0x00,0x00,0x00,0x00,			//自己MAC
					/*12*/0x88,0x8e,								//类型
					/*14*/0x01,									//Version: 1
					/*15*/0x00,									//Type: EAP Packet (0)
					/*16*/0x00,0x22,								//长度，22代表10进制的34
					/*18*/0x02,									//Code: Response (2)
					/*19*/0x36,									//Id: 54
					/*20*/0x00,0x22,								//Length: 34
					/*22*/0x04,									//Type: MD5-Challenge [RFC3748] (4)
					/*23*/0x10,									//Value-Size: 16
					/*24*/0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16字节的MD5的前8字节
					/*32*/0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16字节的MD5的后8字节
					/*40*/0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00//存放自己的账号，是ascii码，我校账号长度为11
				};
	packet_response_MD5[17] = (u_char)m_usernameLen + 22;
	packet_response_MD5[21] = (u_char)m_usernameLen + 22;

	memcpy(packet_response_MD5+6,MacAdd,6);

	//MD5-Challenge的包ID
	packet_response_MD5[19]=captured[19];


/////////////////////////////////////////////////////////////////////
//生成MD5
	u_char MD5Res[16];
	MD5_CTX context;
	BYTE	msgbuf[128]={0};
	int i=0,j=0;
	
	//以下是MD5-Challenge的包ID
	msgbuf[i++]=captured[19];
	
	//以下是登陆密码
	while (j<m_passwordLen)
	{
		msgbuf[i++]=m_password[j++];
	}	
	
	//以下是常量zte142052
	u_char temp[]="zte142052";	
	memcpy(msgbuf+i,temp,9);
	i+=9;
	
	//服务器发来的MD5-Challenge
	memcpy(msgbuf+i,captured+24,16);
	i+=16;
	
	//取得MD5信息摘要，存放在MD5Res中
	MD5Init(&context);
	MD5Update(&context, msgbuf, i);
	MD5Final(MD5Res, &context);
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


	//MD5-Challenge:
	memcpy(packet_response_MD5+24,MD5Res,16);

	//以下是登陆的账号
	for (i=0;i<m_usernameLen;i++)
	{
		packet_response_MD5[40+i]=m_username[i];
	}


	if(pcap_sendpacket(adapterHandle, packet_response_MD5,40+m_usernameLen)!=0)
    {
        return 0;
    }
	return 1;
}
//======================packet_response_MD5======================================


//======================packet_key1======================================
int CPacket::send_packet_key1(pcap_t * adapterHandle,const u_char* captured,u_char* MacAdd)
{
	u_char packet_key1[100]={	
					0x01,0x80,0xc2,0x00,0x00,0x03,				//对方MAC
					0x00,0x00,0x00,0x00,0x00,0x00,				//自己MAC
					0x88,0x8e,									//类型
					0x01,										//Version: 1
					0x03,										//Type: EAP Packet (3)
					0x00,0x3c,									//长度，3c代表10进制的60
					0x01,										//Descriptor Type: RC4 Descriptor (1)
					0x00,0x10,									//key length
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//Replay Counter: 8字节
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16字节的Key IV 前8字节
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16字节的Key IV 后8字节
					0x00,										//index	
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16字节的Key Signature 前8字节
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16字节的Key Signature 后8字节
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16字节的Key 前8字节
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16字节的Key 后8字节
			};


	//MAC
	memcpy(packet_key1+6,MacAdd,6);

	//Replay Counter  +  Key IV 原样复制，24字节
	memcpy(packet_key1+21,captured+21,24);

	//此包的index值，直接从接收包复制过来
	packet_key1[45]=captured[45];
	
	//////////////////////////////////////////////////////////////////////////
	//使用rc4算法生成Key，基于（Key IV + Key IV最后四个字节）==20字节
	u_char enckey[]={0x02,0x0E,0x05,0x04,0x66,0x40,0x19,0x75
					,0x06,0x06,0x00,0x16,0xD3,0xF3,0xAC,0x02
	};
	// 	u_char enckey[]={0x02,0x0E,0x05,0x04,0xD5,0x40,0x19,0x75
	// 		,0x06,0x06,0x00,0x13,0x77,0x4C,0x4E,0xAB
	// 	};
	u_char wholekey[20];
	memcpy(wholekey,captured+29,16);
	memcpy(wholekey+16,captured+41,4);
	int keylen=16;
	struct rc4_state s;
	rc4_setup( &s, wholekey, 20 );
    rc4_crypt( &s, enckey, keylen );
	memcpy(&packet_key1[62],enckey,16);

	//////////////////////////////////////////////////////////////////////////
	//使用hmac_md5算法生成Key Signature，此用于包的校验
	u_char deckey[64]={0};
	u_char encDat[64];
	memcpy(encDat,packet_key1+14,64);
	enckey[0]=packet_key1[45];
	hmac_md5(encDat,64,enckey,1,deckey);
	memcpy(packet_key1+46,deckey,16);

	if(pcap_sendpacket(adapterHandle, packet_key1,78)!=0)
    {
        return 0;
    }
	return 1;
}
//======================packet_key1======================================


//======================packet_key2======================================
int CPacket::send_packet_key2(pcap_t * adapterHandle,const u_char* captured,u_char* MacAdd)
{
	u_char packet_key2[100]={
					0x01,0x80,0xc2,0x00,0x00,0x03,				//对方MAC
					0x00,0x00,0x00,0x00,0x00,0x00,				//自己MAC
					0x88,0x8e,									//类型
					0x01,										//Version: 1
					0x03,										//Type: EAP Packet (3)
					0x00,0x30,									//长度，3c代表10进制的48
					0x01,										//Descriptor Type: RC4 Descriptor (1)
					0x00,0x04,									//key length
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//Replay Counter: 8字节
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16字节的Key IV 前8字节
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16字节的Key IV 后8字节
					0x00,										//index	
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16字节的Key Signature 前8字节
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16字节的Key Signature 后8字节
					0x00,0x00,0x00,0x00							//4字节的Key
			};
	
	//MAC
	memcpy(packet_key2+6,MacAdd,6);
	
	//Replay Counter  +  Key IV 原样复制，24字节
	memcpy(packet_key2+21,captured+21,24);
	
	//此包的index值，直接从接收包复制过来
	packet_key2[45]=captured[45];

	//////////////////////////////////////////////////////////////////////////
	//使用rc4算法生成Key，基于（Key IV + Key IV最后四个字节）==20字节
	u_char enckey[]={0x02,0x02,0x14,0x00};
	u_char wholekey[20];
	memcpy(wholekey,captured+29,16);
	memcpy(wholekey+16,captured+41,4);
	int keylen=4;
	u_char deckey[64]={0};
	struct rc4_state s;
	rc4_setup( &s, wholekey, 20 );
    rc4_crypt( &s, enckey, keylen );
	memcpy(packet_key2+62,enckey,4);

	//////////////////////////////////////////////////////////////////////////
	//使用hmac_md5算法生成Key Signature，此用于包的校验	
	u_char encDat[64];
	memcpy(encDat,packet_key2+14,52);
	enckey[0]=packet_key2[45];
	hmac_md5(encDat,52,enckey,1,deckey);
	memcpy(packet_key2+46,deckey,16);


	if(pcap_sendpacket(adapterHandle, packet_key2,66)!=0)
    {
        return 0;
    }
	return 1;
}
//======================packet_key2======================================
