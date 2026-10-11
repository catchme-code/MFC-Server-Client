// Loginpage.cpp: 구현 파일
//

#include "pch.h"
#include "Chat.h"
#include "ChatDlg.h"
#include "afxdialogex.h"
#include "Loginpage.h"
#include "Adduserpage.h"
#include "DBManager.h"
#include "CClientList.h"
#include <atlconv.h>
#include <Ws2tcpip.h>
#include <String>
#include <vector>

extern DBManager g_DB;
extern CChatApp theApp;
HWND g_hChat = nullptr;

// Loginpage 대화 상자

IMPLEMENT_DYNAMIC(Loginpage, CDialogEx)

Loginpage::Loginpage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(Login, pParent)
{

}

Loginpage::~Loginpage()
{
}

void Loginpage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT1, m_id);
	DDX_Control(pDX, IDC_EDIT3, m_password);

}


BEGIN_MESSAGE_MAP(Loginpage, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &Loginpage::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT1, &Loginpage::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT3, &Loginpage::OnEnChangeEdit3)
	ON_BN_CLICKED(IDC_BUTTON2, &Loginpage::OnBnClickedButton2)
	ON_MESSAGE(WM_LOGIN_RESULT, &Loginpage::OnLoginResult)
END_MESSAGE_MAP()


// Loginpage 메시지 처리기
BOOL Loginpage::OnInitDialog() {
	CDialogEx::OnInitDialog();



	return TRUE;

};

void Loginpage::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	AddUserpage dig;
	EndDialog(IDOK);
	dig.DoModal();


}

void Loginpage::OnEnChangeEdit1()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void Loginpage::OnEnChangeEdit3()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
// Utf8 > CString 변경 함수
static CString Utf8ToCString(const std::string& s)
{
	if (s.empty()) return CString();
	int wlen = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
	CStringW w;
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), w.GetBuffer(wlen), wlen);
	w.ReleaseBuffer(wlen);
	return CString(w);   // 멀티바이트 빌드면 CP949로 변환됨
}

void Loginpage::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItemText(IDC_EDIT1, id);
	GetDlgItemText(IDC_EDIT3, passwd);

		// 서버 소켓 생성
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) AfxMessageBox(_T("socket error"));
	else AfxMessageBox(_T("socket 생성 완료"));

	sockaddr_in server_addr{};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(5000);

	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

	//서버에 연결
	if (connect(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == -1) {
		int errorCode = WSAGetLastError();
		AfxMessageBox(_T("Connecting fail?? : %d"), errorCode);

		closesocket(server_socket);
		server_socket = INVALID_SOCKET;
	}
	else {
		AfxMessageBox(_T("서버 접속 성공"));

	}
	if (server_socket != INVALID_SOCKET) {
		AfxBeginThread(RecvThread, this);    // 수신 스레드 시작
	}
	CT2A idA(id, CP_UTF8);
	CT2A passwordA(passwd, CP_UTF8);

	std::string sendDataLogin = "LOGIN|";
	sendDataLogin += (const char*)idA;
	sendDataLogin += '|';
	sendDataLogin += (const char*)passwordA;
	sendDataLogin += '\n';

	if (server_socket == INVALID_SOCKET)
	{
		AfxMessageBox(_T("서버 소켓이 유효하지 않습니다."));
		return;
	}


	int result = send(server_socket, sendDataLogin.c_str(), (int)sendDataLogin.size(), 0);

}

void Loginpage::OnEnChangeEdit2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void Loginpage::OnEnChangeEdit4()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

//구분자('|')로 문자열 나눠 배열로 저장하는함수
std::vector<std::string> SplitPipe(const std::string& line)
{
	std::vector<std::string> parts;
	size_t start = 0;
	while (true)
	{
		size_t pos = line.find('|', start);
		if (pos == std::string::npos)
		{
			parts.push_back(line.substr(start));
			break;
		}
		parts.push_back(line.substr(start, pos - start));
		start = pos + 1;
	}
	return parts;
}

// 서버에서 전송하는 패킷 받는 함수
UINT Loginpage::RecvThread(LPVOID pParam)
{

	Loginpage* pDlg = (Loginpage*)pParam;
	HWND hWnd = pDlg->m_hWnd;
	char buf[256];

	while (true)
	{
		int len = recv(pDlg->server_socket, buf, sizeof(buf), 0);
		if (len <= 0) break;

		std::string* pResp = new std::string(buf, len);
		while (!pResp->empty() && (pResp->back() == '\n' || pResp->back() == '\r'))
			pResp->pop_back();

		bool bOk = (pResp->compare(0, 8, "LOGIN_OK") == 0);

		if (::IsWindow(hWnd)) ::PostMessage(hWnd, WM_LOGIN_RESULT, 0, (LPARAM)pResp);
		else delete pResp;

		if (bOk) break;

	}
	return 0;
}
//로그인 성공 시 목록 창 전환 함수
LRESULT Loginpage::OnLoginResult(WPARAM wParam, LPARAM lParam)
{

	std::string* pResp = (std::string*)lParam;
	std::string resp = *pResp;
	delete pResp;                      

	std::string head = resp, nameA;
	size_t pos = resp.find('|');
	if (pos != std::string::npos)
	{
		head = resp.substr(0, pos);
		nameA = resp.substr(pos + 1);
	}
	if (head == "LOGIN_OK")
	{
		CClientList* pCLient =new CClientList(this);;
		pCLient->m_sock = server_socket;
		pCLient->m_strName = Utf8ToCString(nameA);   // UTF-8 -> CString
		EndDialog(IDOK);
		pCLient->DoModal();
		delete pCLient;
		
	}

	else if (resp == "LOGIN_FAIL")  AfxMessageBox(_T("LOGIN_FAIL"));
	else if (resp == "JOIN_OK")     AfxMessageBox(_T("JOIN_OK"));
	else if (resp == "JOIN_FAIL")   AfxMessageBox(_T("JOIN_FAIL"));
	return 0;
}

