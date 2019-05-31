// MyQQDlg.h : header file
//

#if !defined(AFX_MYQQDLG_H__F42FE5AC_E2CC_44AB_9D0A_748191BC989F__INCLUDED_)
#define AFX_MYQQDLG_H__F42FE5AC_E2CC_44AB_9D0A_748191BC989F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "ColorListBox.h"
/////////////////////////////////////////////////////////////////////////////
// CMyQQDlg dialog

class CMyQQDlg : public CDialog
{
// Construction
public:
	CMyQQDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMyQQDlg)
	enum { IDD = IDD_MYQQ_DIALOG };
	CProgressCtrl	m_Progress;
	CListBox	m_AddMsgLIst;
	CIPAddressCtrl	m_You_IP;
	CString	m_MsgSend;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyQQDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMyQQDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonConnect();
	afx_msg void OnButtonDisconnect();
	afx_msg void OnButtonSendMsg();
	afx_msg void OnButtonSendFile();
	afx_msg void OnButtonClear();
	afx_msg void OnRadioTcp();
	afx_msg void OnRadioUdp();
	afx_msg void OnRadioServer();
	afx_msg void OnRadioClient();
	afx_msg void OnRadioBoth();
	afx_msg void OnButtonStopFile();
	afx_msg LRESULT OnKSend(WPARAM wParam,LPARAM lParam);//������Ϣ
	afx_msg void OnButtonCaputer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int SaveYouFile(CSocket &recSo,SOCKADDR_IN &client);
	int ReceiveFileMsg(CSocket & socket,SOCKADDR_IN &client);
	int GetIpAddress(const CString & sHostName,CString & sIpAddress);
	int GetLocalHostName(CString &sHostName);
	CString GetError(DWORD error);
	int	m_nSockType;	//��������
	int m_WorkType;		//������ʽ server0,client1,both2
	void AddMsgList(CString IP,CString str); //LISTBOX�ؼ������Ϣ
	int m_client,m_server;
	CString m_fileName;
	bool FileWork,FileStop,StopServer;
	void SaveYouScreen(HBITMAP hBitmap,SOCKADDR_IN &client);
	HBITMAP CopyScreenToBitmap();
	char m_type;//��������/F-�ļ���C-ץƽ��D-�ر�,M-��Ϣ
	int m_Potr;//��ǰʹ�ö˿�
	int GetNamebyAddress(const CString &IpAddress,CString &sYouName);//��öԷ����������

};
struct FILEINFO
{
	int fileLength;
	char fileName[100];

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYQQDLG_H__F42FE5AC_E2CC_44AB_9D0A_748191BC989F__INCLUDED_)
