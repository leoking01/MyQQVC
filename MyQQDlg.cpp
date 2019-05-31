// MyQQDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MyQQ.h"
#include "MyQQDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <dlgs.h>
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
// No message handlers
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMyQQDlg dialog



#define PORT		34567
#define FLAG		2
#define SIZEFILE	1024

const int SOCK_TCP	= 0;	//TCPģʽ
const int SOCK_UDP  = 1;	//UPDģʽ

CWinThread	*pThreadSendFile;	//�����ļ��߳�-->_SendFileThread
CWinThread	*pThreadSendMsg;		//������Ϣ�߳�
CWinThread	*pThreadLisen;		//�����߳�-->_ListenTcpThread
CWinThread	*pReceiveThread;		//�����߳�-->_ReceiveThread
CWinThread  *pThreadCaputre;		//ץȡ��Ļ�߳�


//////////////////////////////////////////////////////////////////////////////


CMyQQDlg::CMyQQDlg(CWnd* pParent /*=NULL*/)
: CDialog(CMyQQDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyQQDlg)
	m_MsgSend = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nSockType=0;//TCP
	m_WorkType=2;//����
	m_client=0;
	m_server=0;
	FileWork=false;
	FileStop=false;
	StopServer=false;
}

void CMyQQDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyQQDlg)
	DDX_Control(pDX, IDC_PROGRESS_SEND_FILE, m_Progress);
	DDX_Control(pDX, IDC_LIST_BOX_ADDMSG, m_AddMsgLIst);
	DDX_Control(pDX, IDC_IPADDRESS, m_You_IP);
	DDX_Text(pDX, IDC_EDIT_SENDMSG, m_MsgSend);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyQQDlg, CDialog)
//{{AFX_MSG_MAP(CMyQQDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnButtonConnect)
ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, OnButtonDisconnect)
ON_BN_CLICKED(IDC_BUTTON_SEND_MSG, OnButtonSendMsg)
ON_BN_CLICKED(IDC_BUTTON_SEND_FILE, OnButtonSendFile)
ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
ON_BN_CLICKED(IDC_RADIO_TCP, OnRadioTcp)
ON_BN_CLICKED(IDC_RADIO_UDP, OnRadioUdp)
ON_BN_CLICKED(IDC_RADIO_SERVER, OnRadioServer)
ON_BN_CLICKED(IDC_RADIO_CLIENT, OnRadioClient)
ON_BN_CLICKED(IDC_RADIO_BOTH, OnRadioBoth)
ON_BN_CLICKED(IDC_BUTTON_STOP_FILE, OnButtonStopFile)
ON_MESSAGE(WM_KSEND,OnKSend)
ON_BN_CLICKED(IDC_BUTTON_CAPUTER, OnButtonCaputer)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyQQDlg message handlers

BOOL CMyQQDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Add "About..." menu item to system menu.
	
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	/////////////////////////////////////////////////////////
	CString strLocalName;
	GetLocalHostName(strLocalName);
	CString strLocalIP;
	GetIpAddress(strLocalName,strLocalIP);
	m_You_IP.SetWindowText(strLocalIP);	//����Ĭ��IPΪ����
	/////////////////////////////////////////////////////////
	((CButton*)GetDlgItem(IDC_RADIO_BOTH))->SetCheck(BST_CHECKED);//Ĭ��Ϊ���������ͻ���һ��
	SetWindowText("MyQQ TCP��ʽ");

	GetDlgItem(IDC_BUTTON_CONNECT)->SetWindowText("��    ��");
	GetDlgItem(IDC_BUTTON_DISCONNECT)->SetWindowText("��    ��");
	
	GetDlgItem(IDC_BUTTON_CAPUTER)->EnableWindow(false);//Ĭ��Ϊ������
	((CButton*)GetDlgItem(IDC_RADIO_TCP))->SetCheck(BST_CHECKED);//Ĭ��ΪTCP
	GetDlgItem(IDC_BUTTON_SEND_MSG)->EnableWindow(false);//������Ϣ������
	GetDlgItem(IDC_BUTTON_SEND_FILE)->EnableWindow(false);//�����ļ�������
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(false);//���������
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(false);//�Ͽ����Ӳ�����
	GetDlgItem(IDC_PROGRESS_SEND_FILE)->ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMyQQDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyQQDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMyQQDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
//////////////////////////////////�߳�///////////////////////////////////////////
//////////////////////////////////�߳�///////////////////////////////////////////



//////////////////////////////////�������߳̿�ʼ///////////////////////////////////////////

//TCP�����߳�
UINT _ListenTcpThread(LPVOID lparam)
{
	CMyQQDlg *pDlg=(CMyQQDlg *)lparam;
	if(pDlg->StopServer==true)	return -1;
	
	CSocket sockSrvr;
	pDlg->m_Potr=PORT+pDlg->m_server;//���浱ǰʹ�ö˿ڣ����ڹر�
	int createSucceed=sockSrvr.Create(pDlg->m_Potr);
	if(createSucceed==0)
	{
		AfxMessageBox("_ListenTcpThread Create����!"+pDlg->GetError(GetLastError()));
		return -1;
	}
	
	int listenSucceed=sockSrvr.Listen();	//��ʼ����
	if(listenSucceed==0)
	{
		AfxMessageBox("_ListenTcpThread Listen����!"+pDlg->GetError(GetLastError()));
		return -1;
	}
	
	CSocket recSo;
	SOCKADDR_IN client;
	int iAddrSize=sizeof(client);
	
	int acceptSucceed=sockSrvr.Accept(recSo,(SOCKADDR *)&client,&iAddrSize);	//�������Ӳ�ȡ�öԷ�IP
	if(acceptSucceed==0)
	{
		AfxMessageBox("_ListenTcpThread Accept����!"+pDlg->GetError(GetLastError()));
		return -1;
	}
	sockSrvr.Close();
	char flag[FLAG]={0};		
	if(recSo.Receive(flag,FLAG)!=2) 
	{
		return -1;	
	}
	pDlg->m_type=flag[0];
	if(pDlg->m_type=='D') return 0;
	
	pThreadLisen=::AfxBeginThread(_ListenTcpThread,pDlg);
	pDlg->ReceiveFileMsg(recSo,client);
	return 0;
	
}




UINT _UDPThread(LPVOID lparam)	/////UDP������Ϣ�߳̿�ʼ
{
	
	CMyQQDlg *pDlg=(CMyQQDlg *)lparam;
	if(pDlg->StopServer==true)	return -1;
	
	CSocket sockSrvrUdp;
	sockSrvrUdp.Create(PORT+pDlg->m_client,SOCK_DGRAM);
	char buff[100]={0};
	int ret=0;
	CString ipStr;
	CString msg;
	UINT port;
	for(;;)
	{
		
		ret=sockSrvrUdp.ReceiveFrom(buff,100,ipStr,port);//IP��port��Ϊ����ֵ
		
		if(buff[0]=='D') return 0;
		if(ret==SOCKET_ERROR) 
		{
			break;
		}
		msg.Format(buff);
		pDlg->AddMsgList(ipStr,msg);
	}
	sockSrvrUdp.Close();
	return 0;
}





////////////////////////////////////�������߳̽���//////////////////////////////////////


////////////////////////////////////�ͻ����߳̿�ʼ//////////////////////////////////////

//�����ļ��߳�
UINT _SendFileThread(LPVOID lparam)
{
	CMyQQDlg *pDlg=(CMyQQDlg *)lparam;
	if(pDlg->StopServer==true)	return -1;
	
	CSocket sockClient;
	sockClient.Create();
	CString ip;
	pDlg->m_You_IP.GetWindowText(ip);
	sockClient.Connect(ip, PORT+pDlg->m_client); 
	//���ȷ��ͱ��FΪ�ļ�,2
	int end=0;
	end=sockClient.Send("F",FLAG);
	///////////////////////////////////////////////////////////////////���ͱ�־�Ƿ�ɹ�
	if(end==SOCKET_ERROR)										
	{
		AfxMessageBox("_SendFileThread Send����!"+pDlg->GetError(GetLastError()));
		return -1;
	}
	
	else if(end!=2) 
	{
		AfxMessageBox("�ļ�ͷ����");
		return -1;
	}
	///////////////////////////////////////////////////////////////////
	CFile myFile;
	FILEINFO myFileInfo;
	if(!myFile.Open(pDlg->m_fileName, CFile::modeRead | CFile::typeBinary))
		return -1;
	myFileInfo.fileLength=myFile.GetLength();		  //�õ��ļ���С
	strcpy(myFileInfo.fileName,myFile.GetFileName());  //�õ��ļ�����
	
	sockClient.Send(&myFileInfo,sizeof(myFileInfo));	  //�����ļ���Ϣ
	
	pDlg->m_Progress.SetRange32(0,myFileInfo.fileLength);
	
	myFile.Seek(0,CFile::begin);
	char m_buf[SIZEFILE]={0};
	CString strError;
	int num=0;
	end=0;
	int temp=0;
	pDlg->GetDlgItem(IDC_BUTTON_STOP_FILE)->EnableWindow(true);
	
	for(;;)
	{
		if(pDlg->FileWork==false)
		{
			pDlg->FileWork=true;
			pDlg->GetDlgItem(IDCANCEL)->EnableWindow(false);
			pDlg->GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(false);
		}
		num=myFile.Read(m_buf, SIZEFILE);
		if(num==0) break;
		end=sockClient.Send(m_buf, num); 
		temp+=end;
		pDlg->m_Progress.SetPos(temp);
		if(pDlg->FileStop==true) 
		{
			pDlg->FileStop=false;
			pDlg->FileWork=false;
			break;
		}
		
		if(end==SOCKET_ERROR)
		{
			AfxMessageBox("_SendFileThread Send����!"+pDlg->GetError(GetLastError()));
			break;
			
		}
	}
	pDlg->m_Progress.SetPos(0);
	CString strLocalName;
	pDlg->GetLocalHostName(strLocalName);
	CString strLocalIP;
	pDlg->GetIpAddress(strLocalName,strLocalIP);
	if(temp==myFileInfo.fileLength)
		pDlg->AddMsgList(strLocalIP+"->"+strLocalName,"�ļ����ͳɹ�");
	else 
		pDlg->AddMsgList(strLocalIP+"->"+strLocalName,"�ļ�����ʧ��");
	myFile.Close();
	sockClient.Close();
	pDlg->FileWork=false;
	pDlg->GetDlgItem(IDC_PROGRESS_SEND_FILE)->ShowWindow(SW_HIDE);
	pDlg->GetDlgItem(IDC_BUTTON_STOP_FILE)->EnableWindow(false);

	pDlg->GetDlgItem(IDCANCEL)->EnableWindow(true);
	pDlg->GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(true);
	
	return 0;
}



UINT _SendMsgThread(LPVOID lparam)	//TCP������Ϣ�߳�
{
	
	CMyQQDlg *pDlg=(CMyQQDlg *)lparam;
	if(pDlg->StopServer==true)	
		return -1;
	
	CSocket sockClient;
	sockClient.Create();
	CString ip,strError;
	pDlg->m_You_IP.GetWindowText(ip);
	int conn=sockClient.Connect(ip, PORT+pDlg->m_client);
	if(conn==0)	///////////////////////////////////
	{
		AfxMessageBox("_SendMsgThread Connect����!"+pDlg->GetError(GetLastError()));
		sockClient.ShutDown(2);
		sockClient.Close();
		AfxEndThread(1L);
		return 0;
	}
	//���ȷ��ͱ��MΪ��Ϣ,2
	int end=0;
	end=sockClient.Send("M",FLAG); 
	if(end==SOCKET_ERROR)
	{
		AfxMessageBox("_SendMsgThread Send����!"+pDlg->GetError(GetLastError()));
		return -1;
	}
	else if(end!=2)
	{
		AfxMessageBox("��Ϣͷ����");
		return -1;
	}
	CString strMsg=pDlg->m_MsgSend;
	end=sockClient.Send(strMsg,strMsg.GetLength()); 
	if(end==SOCKET_ERROR)
	{
		AfxMessageBox("_SendMsgThread Send����!"+pDlg->GetError(GetLastError()));
		return -1;
	}
	CString strLocalName;
	pDlg->GetLocalHostName(strLocalName);
	CString strLocalIP;
	pDlg->GetIpAddress(strLocalName,strLocalIP);
	pDlg->AddMsgList(strLocalIP+"->"+strLocalName,strMsg);
	
	int i=0;
	sockClient.Close();

	return 0;
}
UINT _ThreadCapture(LPVOID lparam)	//ץȡ�Է���Ļ�߳�
{
	
	CMyQQDlg *pDlg=(CMyQQDlg *)lparam;
	if(pDlg->StopServer==true)	return -1;
	
	CSocket sockClient;
	sockClient.Create();
	CString ip;
	pDlg->m_You_IP.GetWindowText(ip);
	sockClient.Connect(ip, PORT+pDlg->m_client); 
	//���ȷ��ͱ��CΪץȡ,2
	int end=0;
	end=sockClient.Send("C",FLAG);
	///////////////////////////////////////////////////////////////////���ͱ�־�Ƿ�ɹ�
	if(end==SOCKET_ERROR)										
	{
		AfxMessageBox("_ThreadCapture Send����!"+pDlg->GetError(GetLastError()));
		return -1;
	}
	
	else if(end!=2) 
	{
		AfxMessageBox("����ͷ����");
		return -1;
	}
	return 0;
}
///////////////////////////////////////////////////////////////////
UINT _SendMsgUdpThread(LPVOID lparam)	//UDP������Ϣ
{
	
	CMyQQDlg *pDlg=(CMyQQDlg *)lparam;
	if(pDlg->StopServer==true)	return -1;
	
	CSocket sockClientUdp;
	pDlg->m_type=PORT+pDlg->m_client+10;
	sockClientUdp.Create(pDlg->m_type,SOCK_DGRAM);
	CString strMsg=pDlg->m_MsgSend;
	int ret=0;
	CString ipStr;
	pDlg->m_You_IP.GetWindowText(ipStr);
	UINT port=PORT+pDlg->m_server;
	ret=sockClientUdp.SendTo(strMsg,strMsg.GetLength(),port,ipStr);
	if(ret==SOCKET_ERROR) 
	{
		DWORD error=GetLastError();
		
	}
	CString strLocalName;
	pDlg->GetLocalHostName(strLocalName);
	CString strLocalIP;
	pDlg->GetIpAddress(strLocalName,strLocalIP);
	pDlg->AddMsgList(strLocalIP+"->"+strLocalName,strMsg);
	sockClientUdp.Close();
	return 0;
	
}

////////////////////////////////////�ͻ����߳̽���//////////////////////////////////////



///////////////////////////////////����//////////////////////////////////////////

int CMyQQDlg::ReceiveFileMsg(CSocket &recSo,SOCKADDR_IN &client)//���ܺ���
{
//	if(StopServer==true)	return -1;
	if(m_type=='C')		//ץȡ
	{
		SaveYouScreen(CopyScreenToBitmap(),client);//���͵�ַ
	}
	
	else if(m_type=='F')			//�ļ�
	{
		SaveYouFile(recSo,client);
	}
	
	else if(m_type=='M')	//��Ϣ
	{
		char buff[100]={0};
		CString msg;
		int ret=0;
		for(;;)
		{
			ret=recSo.Receive(buff,100);
			if(ret==0)
				break;
			msg+=buff;
		}
		CString strOut,strIn;
		m_You_IP.GetWindowText(strIn);
		GetNamebyAddress(strIn,strOut);
		CString youName;
		youName.Format(inet_ntoa(client.sin_addr));
		CString str=youName+"<-"+strOut;
		AddMsgList(str,msg);
	}
	recSo.Close();	
	return 0;
}



HBITMAP CMyQQDlg::CopyScreenToBitmap()
{
	CRect rect(0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
	HDC hScrDC,hMemDC;	// ��Ļ���ڴ��豸������
	HBITMAP hBitmap, hOldBitmap;// λͼ���
	int xScrn, yScrn;	// ��Ļ�ֱ���
	hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);	//Ϊ��Ļ�����豸������
	hMemDC = CreateCompatibleDC(hScrDC);//Ϊ��Ļ�豸�����������ݵ��ڴ��豸������
	xScrn = GetDeviceCaps(hScrDC, HORZRES);// �����Ļ�ֱ���
	yScrn = GetDeviceCaps(hScrDC, VERTRES);
	hBitmap = CreateCompatibleBitmap(hScrDC, rect.Width(), rect.Height());// ����һ������Ļ�豸��������ݵ�λͼ
	hOldBitmap =(HBITMAP)SelectObject(hMemDC, hBitmap);// ����λͼѡ���ڴ��豸��������
	BitBlt(hMemDC, 0, 0, rect.Width(), rect.Height(),hScrDC,rect.left,rect.top, SRCCOPY);// ����Ļ�豸�����������ڴ��豸��������
	hBitmap =(HBITMAP)SelectObject(hMemDC, hOldBitmap);//�õ���Ļλͼ�ľ��
	DeleteDC(hScrDC);//��� 
	DeleteDC(hMemDC);
	return hBitmap;// ����λͼ���
}


void CMyQQDlg::SaveYouScreen(HBITMAP hBitmap,SOCKADDR_IN &client)//������Ļ
{
	HDC hDC; //�豸������
	int iBits;//��ǰ��ʾ�ֱ�����ÿ��������ռ�ֽ���
	WORD wBitCount;//λͼ��ÿ��������ռ�ֽ���
	DWORD dwPaletteSize=0,//�����ɫ���С
		  dwBmBitsSize,//λͼ�������ֽڴ�С
		  dwDIBSize;//λͼ�ļ���С 
	BITMAP Bitmap;//λͼ���Խṹ
	BITMAPFILEHEADER bmfHdr;//λͼ�ļ�ͷ�ṹ
	BITMAPINFOHEADER bi;//λͼ��Ϣͷ�ṹ 
	LPBITMAPINFOHEADER lpbi;//ָ��λͼ��Ϣͷ�ṹ
	HANDLE hDib, hPal,hOldPal=NULL;//�����ļ��������ڴ�������ɫ����
	hDC = CreateDC("DISPLAY",NULL,NULL,NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * 
		GetDeviceCaps(hDC, PLANES);//����λͼ�ļ�ÿ��������ռ�ֽ���
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	if (wBitCount <= 8)
		dwPaletteSize = (1<<wBitCount)*sizeof(RGBQUAD);//�����ɫ���С
	
	//����λͼ��Ϣͷ�ṹ
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize            = sizeof(BITMAPINFOHEADER);
	bi.biWidth           = Bitmap.bmWidth;
	bi.biHeight          = Bitmap.bmHeight;
	bi.biPlanes          = 1;
	bi.biBitCount         = wBitCount;
	bi.biCompression      = BI_RGB;
	bi.biSizeImage        = 0;
	bi.biXPelsPerMeter     = 0;
	bi.biYPelsPerMeter     = 0;
	bi.biClrUsed         = 0;
	bi.biClrImportant      = 0;
	
	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount+31)/32)* 4 *Bitmap.bmHeight ;
	//Ϊλͼ���ݷ����ڴ�
	hDib  = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	// �����ɫ��   
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC=::GetDC(NULL);
		hOldPal = SelectPalette(hDC,(HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	// ��ȡ�õ�ɫ�����µ�����ֵ
	GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
		+dwPaletteSize,
		(BITMAPINFO*)
		lpbi, DIB_RGB_COLORS);
	//�ָ���ɫ��   
	if (hOldPal)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}
	// ����λͼ�ļ�ͷ
	bmfHdr.bfType = 0x4D42;  // "BM"
	dwDIBSize    = sizeof(BITMAPFILEHEADER) 
		+ sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize + dwBmBitsSize;  
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof
		(BITMAPFILEHEADER) 
		+ (DWORD)sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize;
	
	CSocket sockClient;
	sockClient.Create();
	sockClient.Connect(inet_ntoa(client.sin_addr), PORT+m_client); //���ӵ����뷽

	//���ȷ��ͱ��FΪ�ļ�,2
	FILEINFO myFileInfo;
	myFileInfo.fileLength=sizeof(BITMAPFILEHEADER)+dwDIBSize;//�õ��ļ���С
	strcpy(myFileInfo.fileName,"�Է���Ļ.bmp");//�õ��ļ�����
	sockClient.Send("F",FLAG);
	sockClient.Send(&myFileInfo,sizeof(FILEINFO));
	sockClient.Send(&bmfHdr, sizeof(BITMAPFILEHEADER)); 
	sockClient.Send(lpbi, dwDIBSize); 
	GlobalUnlock(hDib);
	GlobalFree(hDib);
}


int CMyQQDlg::SaveYouFile(CSocket &recSo, SOCKADDR_IN &client)//�����ļ�
{
	CString fname;
	CFileDialog dlg(false);	//����ļ�
	FILEINFO myFileInfo;
	recSo.Receive(&myFileInfo,sizeof(FILEINFO));
	int fileLength=myFileInfo.fileLength;
	CString strfileIp,strfileName,strfileLength;
	strfileIp.Format(inet_ntoa(client.sin_addr));
	strfileName.Format(myFileInfo.fileName);
	strfileLength.Format("%f",myFileInfo.fileLength/1024.0);
	CString title="�ļ�"+strfileName+" ��С"+strfileLength+"KB "+"����"+strfileIp+" �Ƿ����";
	dlg.m_ofn.lpstrTitle=title;//������
	char fileme[500]={0};//�����㹻��С
	strcpy(fileme,strfileIp+strfileName);
	dlg.m_ofn.lpstrFile=fileme;	//�ļ�����
	if(dlg.DoModal()==IDOK)
	{
		fname=dlg.GetPathName();	//�õ��ļ������ơ�·��
		GetDlgItem(IDC_PROGRESS_SEND_FILE)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_PROGRESS_SEND_FILE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_STOP_FILE)->EnableWindow(false);
		
		GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(true);
		GetDlgItem(IDCANCEL)->EnableWindow(true);
		
		recSo.Close();	
		return 0;	
		
	}
	char buf[SIZEFILE]={0};
	CFile f(fname,CFile::modeCreate|CFile::modeWrite);	//���ļ�
	
	m_Progress.SetRange32(0,fileLength);
	
	int n=0;	//���ܵ��ֽ��� 0��ʾ����
	int temp=0;
	GetDlgItem(IDC_BUTTON_STOP_FILE)->EnableWindow(true);
	
	GetDlgItem(IDCANCEL)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(false);
	
	
	for(;;)
	{
		n=recSo.Receive(buf,SIZEFILE);	//����
		if(n==0)		//0��ʾ����
			break;		//�������
		f.Write(buf,n);
		temp+=n;
		m_Progress.SetPos(temp);
		if(FileWork==false) FileWork=true;
		if(FileStop==true) 
		{
			FileStop=false;
			FileWork=false;
			break ;
		}
		
	}
	f.Close();
	m_Progress.SetPos(0);
	if(temp==fileLength)
		AddMsgList(inet_ntoa(client.sin_addr),"�ļ����ܳɹ�");
	else
		AddMsgList(inet_ntoa(client.sin_addr),"�ļ�����ʧ��");
	FileWork=false;
	GetDlgItem(IDC_PROGRESS_SEND_FILE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_STOP_FILE)->EnableWindow(false);
	
	GetDlgItem(IDCANCEL)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(true);
	return 0;
}

////////////////////////////////��ť///////////////////////////////////////////////
////////////////////////////////��ť///////////////////////////////////////////////
////////////////////////////////��ť///////////////////////////////////////////////

void CMyQQDlg::OnButtonConnect() //��ʼ����
{
	// TODO: Add your control notification handler code here
	CString str;
	m_You_IP.GetWindowText(str);
	CString strOut,strIn;
	m_You_IP.GetWindowText(strIn);
	if(GetNamebyAddress(strIn,strOut)==-1)
	{
		GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(true);//���Ӳ�����
		return;
	}
	m_MsgSend="���"+strOut+"!";
	UpdateData(false);

	if(m_nSockType==SOCK_TCP)
	{
		
		pThreadLisen=::AfxBeginThread(_ListenTcpThread,this);	//��ʼTCP�߳�
		GetDlgItem(IDC_BUTTON_SEND_MSG)->EnableWindow(true);//������Ϣ����
		GetDlgItem(IDC_BUTTON_SEND_FILE)->EnableWindow(true);//�ļ�����
		GetDlgItem(IDC_BUTTON_CAPUTER)->EnableWindow(true);//ץȡ
	}
	else
	{
		pThreadLisen=::AfxBeginThread(_UDPThread,this);	//��ʼUDP�߳�
		GetDlgItem(IDC_BUTTON_SEND_MSG)->EnableWindow(true);//���Ϳ���
		GetDlgItem(IDC_BUTTON_SEND_FILE)->EnableWindow(false);//�ļ�������
	}

	GetDlgItem(IDC_RADIO_TCP)->EnableWindow(false);//��ѡ������
	GetDlgItem(IDC_RADIO_UDP)->EnableWindow(false);//��ѡ������
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(true);//�Ͽ�����
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(false);//���Ӳ�����
	GetDlgItem(IDC_RADIO_SERVER)->EnableWindow(false);
	GetDlgItem(IDC_RADIO_CLIENT)->EnableWindow(false);
	GetDlgItem(IDC_RADIO_BOTH)->EnableWindow(false);
	GetDlgItem(IDC_IPADDRESS)->EnableWindow(false);

	
	
}

void CMyQQDlg::OnButtonDisconnect() //�ر�
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(true);
	GetDlgItem(IDC_RADIO_TCP)->EnableWindow(true);
	((CButton*)GetDlgItem(IDC_RADIO_UDP))->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_SEND_MSG)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_SEND_FILE)->EnableWindow(false);
	m_AddMsgLIst.ResetContent();
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(false);
	((CButton*)GetDlgItem(IDC_RADIO_SERVER))->EnableWindow(true);
	((CButton*)GetDlgItem(IDC_RADIO_CLIENT))->EnableWindow(true);
	((CButton*)GetDlgItem(IDC_RADIO_BOTH))->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_CAPUTER)->EnableWindow(false);
	GetDlgItem(IDC_IPADDRESS)->EnableWindow(true);
	m_AddMsgLIst.SendMessage(LB_SETHORIZONTALEXTENT,0,0);

/////////////////////////////////////////���ͽ���
	if(m_nSockType==SOCK_TCP)
	{
		DWORD   dwStatus;
		if (pThreadLisen != NULL)
		{
			if(::GetExitCodeThread(pThreadLisen->m_hThread, &dwStatus)==0)
			{
				int errror=GetLastError();
				return;
			}
			if (dwStatus == STILL_ACTIVE)
			{
				CSocket sockClient;
				sockClient.Create();
				CString ip,strError;
				ip="127.0.0.1";
				int conn=sockClient.Connect(ip, m_Potr);
				if(conn==0)	///////////////////////////////////
				{
					AfxMessageBox("�رմ���!"+GetError(GetLastError()));
					sockClient.ShutDown(2);
					sockClient.Close();
					return;
					
				}
				sockClient.Send("D",FLAG); //����
				
			}
			else
			{
				delete pThreadLisen;
				pThreadLisen = NULL;
			}
		}
	}
	else
	{
		CSocket sockClientUdp;
		int succeedCreate=sockClientUdp.Create(m_type,SOCK_DGRAM);
		CString strMsg="D";
		int ret=0;
		CString ipStr;
		m_You_IP.GetWindowText(ipStr);
		UINT port=PORT+m_server;
		ret=sockClientUdp.SendTo(strMsg,1,port,ipStr);
		
	}
	
}

void CMyQQDlg::OnButtonSendMsg() //������Ϣ
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	if(m_MsgSend.GetLength()==0) return;
	if(m_nSockType==SOCK_TCP)
		::AfxBeginThread(_SendMsgThread,this);
	else
		::AfxBeginThread(_SendMsgUdpThread,this);

}


void CMyQQDlg::OnButtonSendFile() //�����ļ�
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(true);
	CString ip;
	m_You_IP.GetWindowText(ip);
	CString title="�ļ�����"+ip+"��ѡ��";
	dlg.m_ofn.lpstrTitle=title;//������
	if(dlg.DoModal()==IDOK)
	{
		m_fileName=dlg.GetPathName();
		GetDlgItem(IDC_PROGRESS_SEND_FILE)->ShowWindow(SW_SHOW);
		pThreadSendFile=::AfxBeginThread(_SendFileThread,this);	//��ʼ�����ļ��߳�
	}
	
}

void CMyQQDlg::OnButtonCaputer() //ץȡ��Ļ
{
	// TODO: Add your control notification handler code here
	pThreadCaputre=::AfxBeginThread(_ThreadCapture,this);	//��ʼ�����ļ��߳�
	
}

void CMyQQDlg::AddMsgList(CString IP,CString str) //�����Ϣ��LISTBOX �ؼ���
{
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	CString time;
	time.Format(_T(" %d:%02.2d"), tm.wHour, tm.wMinute);
	m_AddMsgLIst.AddString(IP+"��"+time+"˵��: "+str);
	int numList=m_AddMsgLIst.GetCount()-1;
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(true);
	m_AddMsgLIst.SetTopIndex(numList);
	m_AddMsgLIst.SetCurSel(numList);
	//ˮƽ����
	int max_width=0;
	CSize sz;
	CClientDC dc(this);
	for(int i=0;i<m_AddMsgLIst.GetCount();i++)
	{
		m_AddMsgLIst.GetText(i,str);
		sz=dc.GetTextExtent(str);
		if(max_width<sz.cx)
			max_width=sz.cx;
	}
	m_AddMsgLIst.SendMessage(LB_SETHORIZONTALEXTENT,max_width,0);

}

void CMyQQDlg::OnButtonClear() //�����������
{
	// TODO: Add your control notification handler code here
	m_AddMsgLIst.ResetContent();
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(false);
	m_AddMsgLIst.SendMessage(LB_SETHORIZONTALEXTENT,0,0);

}

void CMyQQDlg::OnRadioTcp()		//ѡ��TCPģʽ
{
	// TODO: Add your control notification handler code here
	m_nSockType=SOCK_TCP;
	CString text;
	if(m_WorkType==0) text="MyQQ������";
	else if(m_WorkType==1) text="MyQQ�ͻ���";
	else text="MyQQ";
	SetWindowText(text+" TCP��ʽ");

}

void CMyQQDlg::OnRadioUdp()		//ѡ��UPDģʽ
{
	// TODO: Add your control notification handler code here
	m_nSockType=SOCK_UDP;
	CString text;
	if(m_WorkType==0) text="MyQQ������";
	else if(m_WorkType==1) text="MyQQ�ͻ���";
	else text="MyQQ";
	SetWindowText(text+" UDP��ʽ");

	
}

CString CMyQQDlg::GetError(DWORD error)	//���ش�����Ϣ
{
	CString strError;
	switch(error)
	{
	case WSANOTINITIALISED:
		strError="��ʼ������";
		break;
	case WSAENOTCONN:
		strError="�Է�û������";
		break;
	case WSAEWOULDBLOCK :
		strError="�Է��Ѿ��ر�";
		break;
	case WSAECONNREFUSED:
		strError="���ӵĳ��Ա��ܾ�";
		break;
	case WSAENOTSOCK:
		strError="��һ�����׽����ϳ�����һ������";
		break;
	case WSAEADDRINUSE:
		strError="�ض��ĵ�ַ����ʹ����";
		break;
	case WSAECONNRESET:
		strError="�����������ӱ��ر�";
		break;
	default:
		strError="һ�����";	
	}
	return strError;
	
}

int CMyQQDlg::GetLocalHostName(CString &sHostName)	//��ñ��ؼ��������
{
	char szHostName[256];
	int nRetCode;
	nRetCode=gethostname(szHostName,sizeof(szHostName));
	if(nRetCode!=0)
	{
		//��������
		sHostName=_T("û��ȡ��");
		return GetLastError();
	}
	sHostName=szHostName;
	return 0;
}

int CMyQQDlg::GetIpAddress(const CString &sHostName, CString &sIpAddress)//��ñ���IP
{
	struct hostent FAR * lpHostEnt=gethostbyname(sHostName);
	if(lpHostEnt==NULL)
	{
		//��������
		sIpAddress=_T("");
		return GetLastError();
	}
	//��ȡIP
	LPSTR lpAddr=lpHostEnt->h_addr_list[0];
	if(lpAddr)
	{
		struct in_addr inAddr;
		memmove(&inAddr,lpAddr,4);
		//ת��Ϊ��׼��ʽ
		sIpAddress=inet_ntoa(inAddr);
		if(sIpAddress.IsEmpty())
			sIpAddress=_T("û��ȡ��");
	}
	return 0;
}
int CMyQQDlg::GetNamebyAddress(const CString &IpAddress,CString &sYouName)//��öԷ����������
{
	unsigned long addr;
	addr=inet_addr(IpAddress);
	struct hostent FAR * lpHostEnt=gethostbyaddr((char *)&addr,4,AF_INET);
	if(lpHostEnt==NULL)
	{
		//��������
		sYouName=_T("");

		AfxMessageBox("���Ӳ���");//Ӧ��ȡ�������
		return -1;
	}
	CString name=lpHostEnt->h_name;
	sYouName=name;
	return 0;

}

void CMyQQDlg::OnRadioServer() 
{
	// TODO: Add your control notification handler code here
	CString text;
	if(m_nSockType==SOCK_TCP) text="TCP��ʽ";
	else 
		text="UDP��ʽ";
	m_server=1;
	m_client=2;
	m_WorkType=0;
	SetWindowText("MyQQ������ "+text);
	GetDlgItem(IDC_BUTTON_CONNECT)->SetWindowText("��������");
	GetDlgItem(IDC_BUTTON_DISCONNECT)->SetWindowText("�رշ���");
	
}

void CMyQQDlg::OnRadioClient() 
{
	// TODO: Add your control notification handler code here
	CString text;
	if(m_nSockType==SOCK_TCP) text="TCP��ʽ";
	else 
		text="UDP��ʽ";
	m_server=2;
	m_client=1;
	m_WorkType=1;
	SetWindowText("MyQQ�ͻ��� "+text);
	GetDlgItem(IDC_BUTTON_CONNECT)->SetWindowText("��    ��");
	GetDlgItem(IDC_BUTTON_DISCONNECT)->SetWindowText("�Ͽ�����");
	
}

void CMyQQDlg::OnRadioBoth() 
{
	// TODO: Add your control notification handler code here
	CString text;
	if(m_nSockType==SOCK_TCP) text="TCP��ʽ";
	else 
		text="UDP��ʽ";
	m_server=m_client=0;
	m_WorkType=2;
	SetWindowText("MyQQ "+text);
	GetDlgItem(IDC_BUTTON_CONNECT)->SetWindowText("��    ��");
	GetDlgItem(IDC_BUTTON_DISCONNECT)->SetWindowText("��    ��");
}

void CMyQQDlg::OnButtonStopFile() 
{
	// TODO: Add your control notification handler code here
	FileStop=true;
	FileWork=false;
	GetDlgItem(IDCANCEL)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(true);
}

LRESULT CMyQQDlg::OnKSend(WPARAM wParam,LPARAM lParam)
{
	OnButtonSendMsg();
	return 0;
}


BOOL CMyQQDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
		
		if(GetFocus()->GetDlgCtrlID()==IDC_EDIT_SENDMSG ||GetFocus()->GetDlgCtrlID()==IDC_BUTTON_SEND_MSG)
		{
			AfxGetMainWnd()->SendMessage(WM_KSEND);
			return TRUE;
		}
		return CDialog::PreTranslateMessage(pMsg);
}
//////////////////////////////////////////////////////////////
