
// ChatDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Chat.h"
#include "ChatDlg.h"
#include "afxdialogex.h"
#include "DBManager.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Loginpage.h"
#include <String>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



extern Loginpage g_log;
extern HWND g_hChat;
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
	m_sock = INVALID_SOCKET;
}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT2, msgEdit);
	DDX_Control(pDX, IDC_LIST1, m_msg_list);
}

BEGIN_MESSAGE_MAP(CChatDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_SELCHANGE(IDC_LIST1, &CChatDlg::OnLbnSelchangeList1)
	ON_EN_CHANGE(IDC_EDIT2, &CChatDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON4, &CChatDlg::OnBnClickedButton4)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_DESTROY()
	ON_STN_CLICKED(IDC_STATIC_TEXT, &CChatDlg::OnStnClickedStaticText)
	ON_MESSAGE(WM_CHAT_RECV, &CChatDlg::OnChatRecv)
END_MESSAGE_MAP()


// CChatDlg 메시지 처리기

BOOL CChatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetDlgItemText(IDC_STATIC_TEXT, m_strTarget);

	if (!m_bShowMain)   // 처음 실행되는 메인 창일 때만 로그인 창 생성
	{

		m_subDlg3.Create(Login, this);
		m_subDlg3.ShowWindow(SW_SHOW);
	} 

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
	if (m_bShowMain) g_hChat = m_hWnd;
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}
void CChatDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (!m_bShowMain)
		lpwndpos->flags &= ~SWP_SHOWWINDOW;   // 표시 차단

	CDialogEx::OnWindowPosChanging(lpwndpos);
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

// CString -> UTF-8 바이트열
static std::string CStringToUtf8(const CString& s)
{
	CStringW w(s);       // 멀티바이트 빌드면 CP949 -> 유니코드
	if (w.IsEmpty()) return std::string();
	int len = WideCharToMultiByte(CP_UTF8, 0, w, w.GetLength(), nullptr, 0, nullptr, nullptr);
	std::string out(len, '\0');
	WideCharToMultiByte(CP_UTF8, 0, w, w.GetLength(), &out[0], len, nullptr, nullptr);
	return out;
}

// 전송 버튼 클릭 함수
void CChatDlg::SendBtnClicked()
{
	CString msg;
	msgEdit.GetWindowText(msg); // 사용자가 입력한 메시지
	if (msg.IsEmpty()) return;

	CT2A msgA(msg, CP_UTF8);
	const char* message = msgA;

	msg.Replace(_T("\r"), _T(""));
	msg.Replace(_T("\n"), _T(" "));

	int len = (int)strlen(message);
	// "MSG|받는사람|내용\n"
	std::string out = "MSG|" + CStringToUtf8(m_strTarget) + "|" + CStringToUtf8(msg) + "\n";

	//서버로 메세지 전송
	if (send(m_sock, out.c_str(), (int)out.size(), 0) == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		CString s;
		s.Format(_T("send error: %d"), err);
		InsertMsg(s);
		return;
	}
	else {
		InsertMsg(_T("나: ") + msg);   // 내 화면에는 직접 표시 (서버는 보낸 사람에게 되돌려주지 않음)
		msgEdit.SetWindowText(_T(""));
	}	

}

// 메시지 ListBox 추가 함수
void CChatDlg::InsertMsg(CString str)
{
	m_msg_list.InsertString(-1, str);
	m_msg_list.SetTopIndex(m_msg_list.GetCount() - 1);
	if (m_pList)                               
		m_pList->AddHistory(m_strTarget, str);
}

// 대화 상대에게 메시지 표시 함수
LRESULT CChatDlg::OnChatRecv(WPARAM, LPARAM lParam)
{
	CString* p = reinterpret_cast<CString*>(lParam);
	if (!p) return 0;

	int pos = p->Find(_T('|'));
	if (pos < 0) { delete p; return 0; }
	CString from = p->Left(pos);
	CString text = p->Mid(pos + 1);
	delete p;

	if (from == m_strTarget)              
		InsertMsg(from + _T(": ") + text);
	return 0;
}

void CChatDlg::PostNcDestroy()
{
	if (m_pList)
		m_pList->m_chatMap.erase(m_strTarget);
	CDialogEx::PostNcDestroy();
	delete this;
}
//창의 X 버튼을 눌렀을 때 자원정리
void CChatDlg::OnCancel()
{ 
	DestroyWindow();
 }


void CChatDlg::OnDestroy()
{

	g_hChat = nullptr;

	CDialogEx::OnDestroy();
}
void CChatDlg::OnStnClickedStaticText()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
//엔터 누를시 전송 
void CChatDlg::OnOK()
{
	SendBtnClicked();
}