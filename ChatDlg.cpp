
// ChatDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Chat.h"
#include "ChatDlg.h"
#include "afxdialogex.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CChatDlg 대화 상자



CChatDlg::CChatDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHAT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT3, portEdit);
	DDX_Control(pDX, IDC_EDIT1, ipEdit);
	DDX_Control(pDX, IDC_EDIT2, msgEdit);
	DDX_Control(pDX, IDC_LIST1, m_msg_list);
}

BEGIN_MESSAGE_MAP(CChatDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_SELCHANGE(IDC_LIST1, &CChatDlg::OnLbnSelchangeList1)
	ON_EN_CHANGE(IDC_EDIT1, &CChatDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON1, &CChatDlg::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT2, &CChatDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON4, &CChatDlg::OnBnClickedButton4)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CChatDlg 메시지 처리기

BOOL CChatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_subDlg2.Create(IDD_DIALOG2, this);

	m_subDlg2.ShowWindow(SW_SHOW);
	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CChatDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CChatDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CChatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CChatDlg::OnLbnSelchangeList1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CChatDlg::OnEnChangeEdit1()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CChatDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ConnectBtnClicked();
}

void CChatDlg::OnEnChangeEdit2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
void CChatDlg::OnBnClickedButton4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	SendBtnClicked();
}

// 연결 버튼 클릭 함수
void CChatDlg::ConnectBtnClicked()
{
	// 서버 소켓 생성
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) InsertMsg(_T("socket error"));

	UINT port;
	CString strPort;
	portEdit.GetWindowText(strPort); // 사용자가 입력한 서버 포트 
	port = _tstoi(strPort);

	CString strIp;
	ipEdit.GetWindowText(strIp); // 사용자가 입력한 서버 ip

	CT2A ipA(strIp);

	SOCKADDR_IN srv_addr;

	srv_addr.sin_family = AF_INET;
	//srv_addr.sin_addr.s_addr = inet_addr(ipA);
	srv_addr.sin_port = htons(port);

	inet_pton(AF_INET, ipA, &srv_addr.sin_addr);

	// 서버에 연결
	if (connect(server_socket, (SOCKADDR*)&srv_addr, sizeof(srv_addr)) == -1)
		InsertMsg(_T("Connecting fail"));
	else InsertMsg(_T("Server Connected"));

	AfxBeginThread(RecvFunc, this); // 메시지 수신 스레드 시작
}

// 메시지 수신 스레드
UINT CChatDlg::RecvFunc(LPVOID pParam) {

	CChatDlg* pDlg = (CChatDlg*)pParam;

	while (true)
	{
		char temp[1024];

		int recvLen = recv(
			pDlg->server_socket,
			temp,
			sizeof(temp) - 1,
			0
		);

		if (recvLen > 0)
		{
			temp[recvLen] = '\0';

			CString msg(CA2T(temp, CP_UTF8));

			pDlg->InsertMsg(msg);
		}
		else if (recvLen == 0)
		{
			pDlg->InsertMsg(_T("server disconnected"));
			break;
		}
		else
		{
			pDlg->InsertMsg(_T("recv error"));
			break;
		}
	}

	closesocket(pDlg->server_socket);

	return 0;
}

// 전송 버튼 클릭 함수
void CChatDlg::SendBtnClicked()
{
	CString msg;
	msgEdit.GetWindowText(msg); // 사용자가 입력한 메시지

	CT2A msgA(msg, CP_UTF8);
	const char* message = msgA;

	int len = (int)strlen(message);

	// 서버로 메시지 송신
	if (send(server_socket, message, len, 0) == -1)
	{
		InsertMsg(_T("send error"));
	}
	else
	{
		CString displayMsg;
		displayMsg.Format(_T("[Me] : %s"), msg);

		InsertMsg(displayMsg);

		msgEdit.SetWindowText(_T(""));
	}
}

// 메시지 ListBox 추가 함수
void CChatDlg::InsertMsg(CString str)
{
	m_msg_list.InsertString(-1, str);
	m_msg_list.SetTopIndex(m_msg_list.GetCount() - 1);
}

void CChatDlg::OnDestroy()
{
	if (server_socket != INVALID_SOCKET)
	{
		closesocket(server_socket);
		server_socket = INVALID_SOCKET;
	}

	WSACleanup();

	CDialogEx::OnDestroy();
}