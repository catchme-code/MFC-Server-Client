
// ServerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include <atlconv.h>
#include <Ws2tcpip.h>


#define WM_ADD_MESSAGE (WM_USER + 1)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerDlg 대화 상자



CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT2, portEdit);
	DDX_Control(pDX, IDC_LIST1, m_Client_ListBox);
	DDX_Control(pDX, VS_VERSION_INFO, m_msg_ListBox);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_SELCHANGE(VS_VERSION_INFO, &CServerDlg::OnLbnSelchangeVersionInfo)
	ON_LBN_SELCHANGE(IDC_LIST1, &CServerDlg::OnLbnSelchangeList1)
	ON_EN_CHANGE(IDC_EDIT2, &CServerDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON1, &CServerDlg::OnBnClickedButton1)
	ON_MESSAGE(WM_ADD_MESSAGE, &CServerDlg::OnAddMessage)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CServerDlg 메시지 처리기

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0)
	{
		CString msg;
		msg.Format(_T("WSAStartup 실패: %d"), result);
		MessageBox(msg);

		return FALSE;
	}
	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CServerDlg::OnPaint()
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
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CServerDlg::OnLbnSelchangeVersionInfo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CServerDlg::OnLbnSelchangeList1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CServerDlg::OnEnChangeEdit2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CServerDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ConnectBntClicked();
}
//연결 버튼 클릭 함수
void CServerDlg::ConnectBntClicked() {
	UINT port;
	CString strPort;
	portEdit.GetWindowText(strPort); // 사용자가 입력한 포트번호
	port = _tstoi(strPort);

	SOCKADDR_IN srv_addr = {};
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_addr.sin_port = htons(port);

	WSADATA wsaData;

	// 서버 소켓 생성
	server_socket = socket(PF_INET, SOCK_STREAM, 0);

	if (server_socket == INVALID_SOCKET) MessageBox(_T("socket error"));

	// bind(소켓에 ip, port 할당)
	if (bind(server_socket, (SOCKADDR*)&srv_addr, sizeof(srv_addr)) == -1)
		InsertMsg(_T("bind error")); // 채팅창에 알림

	// bind 성공 시, listen(클라이언트 기다림) 시작
	if (listen(server_socket, SOMAXCONN) == -1) InsertMsg(_T("lsten error"));
	else InsertMsg(_T("Listening..."));

	AfxBeginThread(AcceptFunc, this); // 클라이언트 연결 대기 스레드 시작
}
//메세지 ListBox 표출 함수
void CServerDlg::InsertMsg(CString str)
{
	CString* pStr = new CString(str);

	PostMessage(WM_ADD_MESSAGE, 0, reinterpret_cast<LPARAM>(pStr));
}
LRESULT CServerDlg::OnAddMessage(WPARAM wParam, LPARAM lParam)
{
	CString* pStr = reinterpret_cast<CString*>(lParam);

	if (pStr != nullptr)
	{
		m_msg_ListBox.InsertString(-1, *pStr);
		m_msg_ListBox.SetTopIndex(m_msg_ListBox.GetCount() - 1);

		delete pStr;
	}

	return 0;
}

//클라이언트 목록 표출 함수 
void CServerDlg::InsertClient(CString str) {
	m_Client_ListBox.InsertString(-1, str);
	m_Client_ListBox.SetTopIndex(m_Client_ListBox.GetCount() - 1);
}

//클라이언트 메세지 전송 함수
void CServerDlg::SendToAll(SOCKET socket, CString str) {
	SOCKET client_socket;
	POSITION pos = userList.GetHeadPosition();


	while (pos) // 클라이언트 리스트 처음부터
	{
		client_socket = userList.GetNext(pos);
		if (client_socket != INVALID_SOCKET)
			if (client_socket != socket) // 메시지를 보낸 클라이언트 제외
			{	// 클라이언트로 메시지 송신
				CT2A strA(str, CP_UTF8);

				int len = (int)strlen(strA);
				if (send(client_socket, strA, len, 0) == SOCKET_ERROR) {
					InsertMsg(_T("send error"));
				}
			}
	}
}

//클라이언트 접속 함수
UINT CServerDlg::AcceptFunc(LPVOID pParam) {
	// 자기 자신을 매개로 넘김
	CServerDlg* pDlg = (CServerDlg*)pParam;

	while (1)
	{
		SOCKET client_socket;
		SOCKADDR_IN cln_addr;
		CString strIp;
		int sockaddr_in_size = sizeof(sockaddr_in);

		// accept() : 클라이언트 접속 요청 확인, 클라이언트 소켓 반환
		client_socket = accept(pDlg->server_socket, (SOCKADDR*)&cln_addr, &sockaddr_in_size);

		if (client_socket == INVALID_SOCKET) break; // 클라이언트 소켓이 유효하지않으면
		else
		{
			pDlg->userList.AddTail(client_socket); // 클라이언트 리스트에 추가
			TCHAR ip[INET_ADDRSTRLEN];

			InetNtop(AF_INET, &cln_addr.sin_addr, ip, INET_ADDRSTRLEN);

			strIp.Format(
				_T("- Client (%s : %d)"),
				ip,
				ntohs(cln_addr.sin_port)
			);
			pDlg->InsertClient(strIp); // 클라이언트 ListBox에 추가
			strIp += " 입장하셨습 니다";
			pDlg->InsertMsg(strIp); // 메시지 ListBox에 클라이언트 접속 메시지 추가
			pDlg->SendToAll(client_socket, strIp); // 모든 클라이언트에 접속 메시지 송신

			MyData* data = new MyData;
			data->m_pDlg = pDlg; // 자기자신
			data->c_sock = client_socket;
			data->c_addr = cln_addr; // 접속한 클라이언트 정보

			AfxBeginThread(RecvFunc, data); // 메시지 수신 스레드 시작
		}
	}
	closesocket(pDlg->server_socket); // 오류 발생 시, 소켓 종료
	return 0;
}

//클라이언트 메세지 수신 함수 
UINT CServerDlg::RecvFunc(LPVOID pParam) {
	MyData* data = (MyData*)pParam;
	CServerDlg* pDlg = (CServerDlg*)data->m_pDlg;
	SOCKET client_socket;
	SOCKADDR_IN cln_addr;

	while (1)
	{
		char temp[1024] = ""; // 메시지를 담을 배열
		int strlen;
		client_socket = data->c_sock;
		cln_addr = data->c_addr;

		// 메시지 수신, 수신한 메시지 길이 반환
		strlen = recv(client_socket, temp, 1024, NULL);

		if (strlen == -1) // 오류
		{
			// 클라이언트 리스트에 있다면 해당 클라이언트 퇴장 처리 
			if (pDlg->userList.Find(client_socket) != NULL)
			{
				POSITION pos = pDlg->userList.Find(client_socket);
				pDlg->userList.RemoveAt(pos);
				CString user;
				TCHAR ip[INET_ADDRSTRLEN];

				InetNtop(AF_INET, &cln_addr.sin_addr, ip, INET_ADDRSTRLEN);

				user.Format(
					_T("- Client (%s : %d)"),
					ip,
					ntohs(cln_addr.sin_port)
				);
				int index = pDlg->m_Client_ListBox.FindString(pDlg->m_Client_ListBox.GetCurSel(), user);
				pDlg->m_Client_ListBox.DeleteString(index);
				user += " has exited -";
				pDlg->InsertMsg(user);
				pDlg->SendToAll(client_socket, user);
			}
			break;
		}
		else // 메시지 정상 수신
		{
			CString str;
			TCHAR ip[INET_ADDRSTRLEN];

			InetNtop(AF_INET, &cln_addr.sin_addr, ip, INET_ADDRSTRLEN);

			str.Format(
				_T("- Client (%s : %d)"),
				ip,
				ntohs(cln_addr.sin_port)
			);

			CString message(CA2T(temp, CP_UTF8));

			str += message;
			pDlg->InsertMsg(str); // 메시지 ListBox에 수신 메시지 추가
			pDlg->SendToAll(client_socket, str); // 모든 클라이언트에 수신 메시지 송신
			str.Empty();
		}
	}
	closesocket(client_socket);
	return 0;
}

//다이어로그 소멸시 자원정리
void CServerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	WSACleanup();
}

