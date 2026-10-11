// AddUserpage.cpp: 구현 파일
//

#include "pch.h"
#include "Chat.h"
#include "afxdialogex.h"
#include "AddUserpage.h"
#include "DBManager.h"
#include "Loginpage.h"
#include <atlconv.h>
#include <Ws2tcpip.h>
#include<String>


extern DBManager g_DB;
// AddUserpage 대화 상자

IMPLEMENT_DYNAMIC(AddUserpage, CDialogEx)

AddUserpage::AddUserpage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(AddUser, pParent)
{

}

AddUserpage::~AddUserpage()
{
}

void AddUserpage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT1, m_name);
	DDX_Control(pDX, IDC_EDIT3, m_id);
	DDX_Control(pDX, IDC_EDIT4, m_password);
}


BEGIN_MESSAGE_MAP(AddUserpage, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &AddUserpage::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT4, &AddUserpage::OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT3, &AddUserpage::OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT1, &AddUserpage::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON2, &AddUserpage::OnBnClickedButton2)
END_MESSAGE_MAP()


// AddUserpage 메시지 처리기

void AddUserpage::OnBnClickedButton1()
{
	

	GetDlgItemText(IDC_EDIT1,name);
	GetDlgItemText(IDC_EDIT3, id);
	GetDlgItemText(IDC_EDIT4, password);

	CT2A nameA(name, CP_UTF8);
	AfxMessageBox(CString(CA2T((LPCSTR)nameA, CP_UTF8)));
	const char* pName = nameA;

	if (pName == nullptr)
	{
		AfxMessageBox(_T("UTF-8 변환 실패"));
	}
	else
	{
		int byteLength = (int)strlen(pName);

		CString msg;
		msg.Format(_T("UTF-8 byte length = %d"), byteLength);
		AfxMessageBox(msg);

		CString hex, tmp;
		for (const unsigned char* p = (const unsigned char*)pName; *p; ++p)
		{
			tmp.Format(_T("%02X "), *p);
			hex += tmp;
		}
		AfxMessageBox(hex);
		AfxMessageBox(name);   // 변환 전 원본도 확인

	}

	CT2A idA(id, CP_UTF8);
	CT2A passwordA(password, CP_UTF8);

	// 1) 임시 소켓 생성 + 접속
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET) { AfxMessageBox(_T("socket error")); return; }

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(5000);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

	if (connect(s, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		CString msg;
		msg.Format(_T("접속 실패: %d"), WSAGetLastError());
		AfxMessageBox(msg);
		closesocket(s);
		return;
	}
	else
	{
		CString msg;
		msg.Format(_T("접속 성공"));
		AfxMessageBox(msg);

	}
	// 전송할 데이터 구성

	std::string sendDataAddUser = "JOIN|";
	sendDataAddUser += (const char*)nameA;
	sendDataAddUser += '|';
	sendDataAddUser += (const char*)idA;
	sendDataAddUser += '|';
	sendDataAddUser += (const char*)passwordA;
	sendDataAddUser += '\n';
	// 서버로 전송

	int result =send(s, sendDataAddUser.c_str(), (int)sendDataAddUser.size(), 0);

	// 4) 결과 수신 (스레드 없이 바로 recv)
	char buf[64] = {};
	int len = recv(s, buf, sizeof(buf) - 1, 0);
	if (len == SOCKET_ERROR)
	{
		int error = WSAGetLastError();

		CString msg;
		msg.Format(_T("서버 응답 오류: %d"), error);

		AfxMessageBox(msg);

		closesocket(s);
		return;
	}
	closesocket(s);

	if (result == SOCKET_ERROR)
	{
		int error = WSAGetLastError();

		CString msg;
		msg.Format(_T("전송 실패: %d"), error);
		AfxMessageBox(msg);
	}
	else
	{
		CString msg;
		msg.Format(_T("전송 성공: %d바이트"), result);
		AfxMessageBox(msg);
	}

	if (len <= 0)
	{
		AfxMessageBox(_T("서버 응답이 없습니다."));
		return;
	}

	buf[len] = '\0';
	CStringA resp(buf);

	if (resp == "JOIN_OK")
	{
		AfxMessageBox(_T("회원가입 완료"));
		Loginpage dig;
		EndDialog(IDOK);
		dig.DoModal();                  // 이후 로그인창으로 돌아가는 처리
	}
	else
	{
		AfxMessageBox(_T("회원가입 실패"));
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
}

void AddUserpage::OnEnChangeEdit4()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void AddUserpage::OnEnChangeEdit3()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void AddUserpage::OnEnChangeEdit1()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void AddUserpage::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Loginpage dig;
	EndDialog(IDOK);
	dig.DoModal();
}
