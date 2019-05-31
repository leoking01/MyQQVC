; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMyQQDlg
LastTemplate=CListBox
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "MyQQ.h"

ClassCount=4
Class1=CMyQQApp
Class2=CMyQQDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class4=CColorListBox
Resource3=IDD_MYQQ_DIALOG

[CLS:CMyQQApp]
Type=0
HeaderFile=MyQQ.h
ImplementationFile=MyQQ.cpp
Filter=N

[CLS:CMyQQDlg]
Type=0
HeaderFile=MyQQDlg.h
ImplementationFile=MyQQDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_BUTTON_CAPUTER

[CLS:CAboutDlg]
Type=0
HeaderFile=MyQQDlg.h
ImplementationFile=MyQQDlg.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_MYQQ_DIALOG]
Type=1
Class=CMyQQDlg
ControlCount=20
Control1=IDC_IPADDRESS,SysIPAddress32,1342242816
Control2=IDC_BUTTON_CONNECT,button,1342275585
Control3=IDC_BUTTON_SEND_MSG,button,1342275584
Control4=IDC_BUTTON_SEND_FILE,button,1342275584
Control5=IDC_BUTTON_CAPUTER,button,1342275584
Control6=IDC_BUTTON_STOP_FILE,button,1476493312
Control7=IDC_BUTTON_CLEAR,button,1342275584
Control8=IDC_BUTTON_DISCONNECT,button,1342275584
Control9=IDCANCEL,button,1342275584
Control10=IDC_STATIC,button,1342177287
Control11=IDC_RADIO_TCP,button,1342210057
Control12=IDC_RADIO_UDP,button,1342210057
Control13=IDC_STATIC,static,1342308352
Control14=IDC_EDIT_SENDMSG,edit,1350631552
Control15=IDC_LIST_BOX_ADDMSG,listbox,1353711873
Control16=IDC_PROGRESS_SEND_FILE,msctls_progress32,1350565888
Control17=IDC_RADIO_SERVER,button,1342210057
Control18=IDC_RADIO_CLIENT,button,1342210057
Control19=IDC_RADIO_BOTH,button,1342210057
Control20=IDC_STATIC,button,1342308359

[CLS:CColorListBox]
Type=0
HeaderFile=ColorListBox.h
ImplementationFile=ColorListBox.cpp
BaseClass=CListBox
Filter=W
VirtualFilter=bWC
LastObject=CColorListBox

