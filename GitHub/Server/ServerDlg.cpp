
// ServerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include <string>
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include <atlconv.h>
#include <Ws2tcpip.h>
#include "DBManager.h"
#include <vector>

#define WM_ADD_MESSAGE (WM_USER + 1)
#define WM_REMOVE_CLIENT  (WM_USER + 2) 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CServerDlg 대화 상자


extern CDBManager g_DB;

CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	server_socket = INVALID_SOCKET;
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
	ON_MESSAGE(WM_REMOVE_CLIENT, &CServerDlg::OnRemoveClient)
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

// UTF-8 바이트열 -> CString 
static CString Utf8ToCString(const std::string& s)
{
	if (s.empty()) return CString();
	int wlen = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
	CStringW w;
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), w.GetBuffer(wlen), wlen);
	w.ReleaseBuffer(wlen);
	return CString(w);   // 멀티바이트 빌드면 CP949로 변환됨
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

	if (m_pBcastThread == nullptr)   // 연결 버튼 중복 클릭 방지
	{
		m_running = true;
		m_pBcastThread = AfxBeginThread(BroadcastThreadFunc, this,
			THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		m_pBcastThread->m_bAutoDelete = FALSE;   // 종료 시 직접 대기하려고
		m_pBcastThread->ResumeThread();
	}
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
		if (m_msg_ListBox.GetCount() > 1000)   // 오래된 줄 삭제
			m_msg_ListBox.DeleteString(0);
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


//클라이언트 접속 함수
UINT CServerDlg::AcceptFunc(LPVOID pParam) {
	 
	CServerDlg* pDlg = (CServerDlg*)pParam;
	while (1)
	{
		SOCKET client_socket;
		SOCKADDR_IN cln_addr;
		CString strIp;
		int sockaddr_in_size = sizeof(sockaddr_in);

		// accept() : 클라이언트 접속 요청 확인, 클라이언트 소켓 반환
		client_socket = accept(pDlg->server_socket, (SOCKADDR*)&cln_addr, &sockaddr_in_size);// 2,3 번쨰 인자 NULL 가능 

		if (client_socket == INVALID_SOCKET) {
			int err = WSAGetLastError();
			CString m;
			m.Format(_T("accept 실패: %d"), err);
			pDlg->InsertMsg(m);
			break;
		}// 클라이언트 소켓이 유효하지않으면
		else
		{
			pDlg->userList.AddTail(client_socket); // 클라이언트 리스트에 추가

			MyData* data = new MyData;
			data->m_pDlg = pDlg; // 자기자신
			data->c_sock = client_socket; //접속한 클라이언트 정보

			AfxBeginThread(CheckPacket, data); // 로그인/회원가입 스레드 시작
			pDlg->InsertMsg(strIp); // 메시지 ListBox에 클라이언트 접속 메시지 추가
		}
	}
	closesocket(pDlg->server_socket); // 오류 발생 시, 소켓 종료
	return 0;
}

// 구분자(|)로 나눠진 문자열 배열에 담기
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

//해당 유저에게 데이터 전송
void CServerDlg::SendToUser(const CString& name, const std::string& data)
{
	CSingleLock lock(&m_cs, TRUE);
	POSITION pos = m_clientList.GetHeadPosition();
	while (pos)
	{
		Clientinfo& c = m_clientList.GetNext(pos);
		if (c.Clientname == name)
			send(c.sock, data.c_str(), (int)data.size(), 0);
	}
}
// 클라이언트에서 데이터 받고 클라이언트로 전달
UINT CServerDlg::CheckPacket(LPVOID pParam) {
	MyData* data = (MyData*)pParam;
	CServerDlg* pDlg = data->m_pDlg;
	SOCKET client_socket = data->c_sock;

	CString loginName;
	std::string acc;
	std::string recvBuf;
	char buffer[1024];

	while (true)
	{

		int result = recv(client_socket,buffer,sizeof(buffer),0);

		if (result <= 0){break;}

		recvBuf.append(buffer, result);


		if (recvBuf.size() > 64 * 1024) break;

		size_t pos;
		while ((pos = recvBuf.find('\n')) != std::string::npos)
		{
			std::string line = recvBuf.substr(0, pos);
			recvBuf.erase(0, pos + 1);
		//std::string recvData(buffer, result);
		/*while (!recvData.empty() &&
			(recvData.back() == '\n' || recvData.back() == '\r' || recvData.back() == ' '))
			recvData.pop_back();*/
			while (!line.empty() && (line.back() == '\r' || line.back() == ' '))
				line.pop_back();
			if (line.empty()) continue;

			std::vector<std::string> p = SplitPipe(line);
			const std::string& cmd = p[0];

			/*std::vector<std::string> p = SplitPipe(recvData);
			const std::string& cmd = p[0];*/

			if (cmd == "JOIN")
			{
				if (p.size() != 4)
				{
					pDlg->InsertMsg(_T("JOIN 패킷 형식 오류"));
					send(client_socket, "JOIN_FAIL", (int)strlen("JOIN_FAIL"), 0);
					continue;
				}

				CString name = Utf8ToCString(p[1]);
				CString id = Utf8ToCString(p[2]);
				CString password = Utf8ToCString(p[3]);

				CString str;
				str.Format(_T("name : %s, id : %s, passwd : %s"), name, id, password);

				if (g_DB.InsertData(name, id, password))
				{
					pDlg->InsertMsg(str);
					send(client_socket, "JOIN_OK\n", (int)strlen("JOIN_OK"), 0);
				}
				else
				{
					pDlg->InsertMsg(_T("JOIN_FAIL"));
					send(client_socket, "JOIN_FAIL\n", (int)strlen("JOIN_FAIL"), 0);
				}
			}
			else if (cmd == "LOGIN")
			{
				if (p.size() != 3)
				{
					send(client_socket, "LOGIN_FAIL", (int)strlen("LOGIN_FAIL"), 0);
					continue;
				}

				CString id = Utf8ToCString(p[1]);
				CString password = Utf8ToCString(p[2]);

				CString name, dbId, dbPw;
				g_DB.SelectData(id, password, name, dbId, dbPw);

				if (dbId == id && dbPw == password)
				{

					loginName = name;
					CString str;
					CString struser;
					str.Format(_T("%s 님이 입장 하셨습니다 "), name);
					struser.Format(_T("%s"), name);
					pDlg->InsertMsg(str);

					{   // 로그인한 유저만 목록에 추가
						CSingleLock lock(&pDlg->m_cs, TRUE);
						Clientinfo info;
						info.sock = client_socket;
						info.Clientname = name;
						pDlg->m_clientList.AddTail(info);
					}
					pDlg->InsertClient(struser);
					pDlg->m_listDirty = true;

					std::string ok = "LOGIN_OK|" + CStringToUtf8(name)+"\n";
					send(client_socket, ok.c_str(), (int)ok.size(), 0);


				}
				else
				{
					pDlg->InsertMsg(_T("LOGIN_FAIL"));
					send(client_socket, "LOGIN_FAIL\n", (int)strlen("LOGIN_FAIL"), 0);
				}
			}
			else if (cmd == "LIST_REQ")
			{
				pDlg->SendListTo(client_socket);
				//pDlg->BroadcastClientList();
			}
			else if (cmd == "MSG") {
				if (p.size() < 3 || loginName.IsEmpty()) continue;
				std::string body = p[2];
				for (size_t i = 3; i < p.size(); ++i) body += "|" + p[i];

				CString target = Utf8ToCString(p[1]);
				CString text = Utf8ToCString(body);
				CString log;
				log.Format(_T("%s -> %s : %s"), loginName, target, text);
				pDlg->InsertMsg(log);

				std::string out = "MSG|" + CStringToUtf8(loginName) + "|" + body + "\n";
				pDlg->SendToUser(target, out);
				}
			}
		}
	

	CSingleLock lock(&pDlg->m_cs, TRUE);
	POSITION pos = pDlg->m_clientList.GetHeadPosition();
	while (pos)
	{
		POSITION cur = pos;
		Clientinfo& c = pDlg->m_clientList.GetNext(pos);
		if (c.sock == client_socket)
		{
			pDlg->m_clientList.RemoveAt(cur);
			break;
		}
	}
	POSITION up = pDlg->userList.Find(client_socket);
	if (up != NULL)
		pDlg->userList.RemoveAt(up);

	closesocket(client_socket);
	if (!loginName.IsEmpty() && ::IsWindow(pDlg->m_hWnd))
	{
		pDlg->PostMessage(WM_REMOVE_CLIENT, 0, (LPARAM)new CString(loginName));
		pDlg->m_listDirty = true;
		//pDlg->BroadcastClientList();   // 남은 사람들의 목록 갱신
	}
	delete data;

	return 0;

}
// 클라이언트 목록 추가 
void CServerDlg::BroadcastClientList()
{
	CString strList;
	std::vector<SOCKET> socks;
	{
		CSingleLock lock(&m_cs, TRUE);
		POSITION pos = m_clientList.GetHeadPosition();
		while (pos)
		{
			Clientinfo& c = m_clientList.GetNext(pos);
			if (!strList.IsEmpty()) strList += _T(",");
			strList += c.Clientname;
			socks.push_back(c.sock);
		}

	}
	CString packet = _T("USERLIST|") + strList + _T("\n");
	std::string packetA = CStringToUtf8(packet);

	for (SOCKET s : socks)
		send(s, packetA.c_str(), (int)packetA.size(), 0);
	/*while (pos)
	{
		Clientinfo& c = m_clientList.GetNext(pos);
		send(c.sock, packetA.c_str(), (int)packetA.size(), 0);
	}*/
}
//클라이언트 목록 제거
LRESULT CServerDlg::OnRemoveClient(WPARAM, LPARAM lParam)
{
	CString* pName = reinterpret_cast<CString*>(lParam);
	if (pName == nullptr) return 0;

	int idx = m_Client_ListBox.FindStringExact(-1, *pName);
	if (idx != LB_ERR)
		m_Client_ListBox.DeleteString(idx);

	CString msg;
	msg.Format(_T("%s 님이 퇴장 하셨습니다"), *pName);
	InsertMsg(msg);

	delete pName;
	return 0;
}

// 0.5초마다 변경 여부를 확인해서 한 번만 전송
UINT CServerDlg::BroadcastThreadFunc(LPVOID pParam)
{
	CServerDlg* pDlg = (CServerDlg*)pParam;
	while (pDlg->m_running)
	{
		Sleep(500);
		if (pDlg->m_listDirty.exchange(false))   // true였으면 false로 바꾸고 true 반환
			pDlg->BroadcastClientList();
	}
	return 0;
}

// LIST_REQ를 보낸 클라이언트 한 명에게만 목록 전송
void CServerDlg::SendListTo(SOCKET sock)
{
	CString strList;
	{
		CSingleLock lock(&m_cs, TRUE);
		POSITION pos = m_clientList.GetHeadPosition();
		while (pos)
		{
			Clientinfo& c = m_clientList.GetNext(pos);
			if (!strList.IsEmpty()) strList += _T(",");
			strList += c.Clientname;
		}
	}   // 락 해제 후 전송
	std::string pkt = CStringToUtf8(_T("USERLIST|") + strList + _T("\n"));
	send(sock, pkt.c_str(), (int)pkt.size(), 0);
}

//다이어로그 소멸시 자원정리
void CServerDlg::OnDestroy()
{
	m_running = false;
	if (m_pBcastThread)
	{
		WaitForSingleObject(m_pBcastThread->m_hThread, 2000);
		delete m_pBcastThread;
		m_pBcastThread = nullptr;
	}
	CDialogEx::OnDestroy();
	WSACleanup();

}
