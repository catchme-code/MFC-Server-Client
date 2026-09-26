// TCP_Client.cpp: 구현 파일
//

#include "pch.h"
#include "Chat.h"
#include "afxdialogex.h"
#include "TCP_Client.h"
#include <winsock2.h>
#include <ws2tcpip.h>


// TCP_Client 대화 상자

IMPLEMENT_DYNAMIC(TCP_Client, CDialogEx)

TCP_Client::TCP_Client(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

TCP_Client::~TCP_Client()
{
}

void TCP_Client::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT3, portEdit);
	DDX_Control(pDX, IDC_EDIT1, ipEdit);
	DDX_Control(pDX, IDC_EDIT2, msgEdit);
	DDX_Control(pDX, IDC_LIST1, m_msg_list);
}


BEGIN_MESSAGE_MAP(TCP_Client, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &TCP_Client::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON4, &TCP_Client::OnBnClickedButton4)
	ON_LBN_SELCHANGE(IDC_LIST1, &TCP_Client::OnLbnSelchangeList1)
	ON_EN_CHANGE(IDC_EDIT3, &TCP_Client::OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT1, &TCP_Client::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &TCP_Client::OnEnChangeEdit2)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// TCP_Client 메시지 처리기

void TCP_Client::OnBnClickedButton1()
{
	ConnectBtnClicked();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void TCP_Client::OnBnClickedButton4()
{
	SendBtnClicked();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
// 연결 버튼 클릭 함수
void TCP_Client::ConnectBtnClicked()
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
	srv_addr.sin_port = htons(port);

	inet_pton(AF_INET, ipA, &srv_addr.sin_addr);

	//서버에 연결
	if (connect(server_socket, (SOCKADDR*)&srv_addr, sizeof(srv_addr)) == -1){
		int errorCode = WSAGetLastError();

		CString msg;
		msg.Format(_T("Connecting fail?? : %d"), errorCode);

		InsertMsg(msg);

		closesocket(server_socket);
		server_socket = INVALID_SOCKET;
	}
	else{
		InsertMsg(_T("Server Connected"));

		AfxBeginThread(RecvFunc, this);
		}

}
// 메시지 수신 스레드
UINT TCP_Client::RecvFunc(LPVOID pParam) {

	TCP_Client* pDlg = (TCP_Client*)pParam;

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
void TCP_Client::SendBtnClicked()
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
void TCP_Client::InsertMsg(CString str)
{
	m_msg_list.InsertString(-1, str);
	m_msg_list.SetTopIndex(m_msg_list.GetCount() - 1);
}
void TCP_Client::OnLbnSelchangeList1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void TCP_Client::OnEnChangeEdit3()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void TCP_Client::OnEnChangeEdit1()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void TCP_Client::OnEnChangeEdit2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

//다이어로그 생성시 소켓 초기화 작업
BOOL TCP_Client::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0)
	{
		CString msg;
		msg.Format(_T("WSAStartup 실패 : %d"), result);
		MessageBox(msg);

		return FALSE;
	}

	return TRUE;
}
//창 닫힐때 자원 정리
void TCP_Client::OnDestroy()
{
	if (server_socket != INVALID_SOCKET)
	{
		closesocket(server_socket);
		server_socket = INVALID_SOCKET;
	}

	WSACleanup();

	CDialogEx::OnDestroy();
}