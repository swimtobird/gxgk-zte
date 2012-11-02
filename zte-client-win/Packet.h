// Packet.h: interface for the CPacket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACKET_H__AE293802_9E7F_4666_8323_49ECE7F9A8D0__INCLUDED_)
#define AFX_PACKET_H__AE293802_9E7F_4666_8323_49ECE7F9A8D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000






class CPacket  
{
public:
	CPacket();
	virtual ~CPacket();

	//以下为数据包初始化并发送的函数
	int send_packet_start(pcap_t * adapterHandle,u_char* MacAdd);						//此为开始的包

	int send_packet_logoff(pcap_t * adapterHandle,u_char* MacAdd);						//此为退出的包

	int send_packet_response_Identity(pcap_t * adapterHandle,const u_char * captured,u_char* MacAdd
									,u_char* m_username,int m_usernameLen);				//此为回应系统的request包

	int send_packet_response_MD5(pcap_t * adapterHandle,const u_char* captured,u_char* MacAdd
		,u_char* m_username,int m_usernameLen,u_char* m_password,int m_passwordLen);	//此为回应系统的MD5-Challenge包

	int send_packet_key1(pcap_t * adapterHandle,const u_char* captured,u_char* MacAdd);		//连接维持包1

	int send_packet_key2(pcap_t * adapterHandle,const u_char* captured,u_char* MacAdd);		//连接维持包2

};

#endif // !defined(AFX_PACKET_H__AE293802_9E7F_4666_8323_49ECE7F9A8D0__INCLUDED_)
