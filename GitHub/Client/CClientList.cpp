// CClientList.cpp: 구현 파일
//

#include "pch.h"
#include "Chat.h"
#include "ChatDlg.h"
#include "afxdialogex.h"
#include "CClientList.h"
#include "Loginpage.h"
#include <string>
#include <vector>

// CClientList 대화 상자




IMPLEMENT_DYNAMIC(CClientList, CDialogEx)

CClientList::CClientList(CWnd* pParent /*=nullptr*/)
    : CDialogEx(ClientList, pParent)
{

}

CClientList::~CClientList()
{
    m_sock = INVALID_SOCKET;
}



void CClientList::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_ClientList);
}


BEGIN_MESSAGE_MAP(CClientList, CDialogEx)
    ON_LBN_SELCHANGE(IDC_LIST1, &CClientList::OnLbnSelchangeList1)
    ON_LBN_DBLCLK(IDC_LIST1, &CClientList::OnLbnDblclkList1)
    ON_MESSAGE(WM_USER_LIST, &CClientList::OnUserList)
    ON_BN_CLICKED(IDC_BUTTON1, &CClientList::OnBnClickedButton1)
    ON_STN_CLICKED(IDC_STATIC_NAME, &CClientList::OnStnClickedStaticName)
    ON_MESSAGE(WM_SERVER_LINE, &CClientList::OnServerLine)
END_MESSAGE_MAP()


// CClientList 메시지 처리기
void CClientList::OnLbnDblclkList1()
{

    int idx = m_ClientList.GetCurSel();
    if (idx == LB_ERR) return;

    CString target;
    m_ClientList.GetText(idx, target);
    target = StripBadge(target);    

    ClearUnread(target);             // 읽음 처리
    OpenChat(target);

}

void CClientList::OnLbnSelchangeList1()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

}

BOOL CClientList::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetDlgItemText(IDC_STATIC_NAME, _T("현재 사용자 :  ") + m_strName);

    if (m_sock == INVALID_SOCKET) return TRUE;
    AfxBeginThread(RecvThread, this);             // 수신 스레드 시작

    RequestList();

    return TRUE;
}
// UTF-8 -> CString 형변환
static CString Utf8ToCString(const std::string& s)
{
    if (s.empty()) return CString();
    int wlen = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
    CStringW w;
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), w.GetBuffer(wlen), wlen);
    w.ReleaseBuffer(wlen);
    return CString(w);   // 멀티바이트 빌드면 CP949로 변환됨
}

// CString -> UTF-8 형변환
static std::string CStringToUtf8(const CString& s)
{
    CStringW w(s);       // 멀티바이트 빌드면 CP949 -> 유니코드
    if (w.IsEmpty()) return std::string();
    int len = WideCharToMultiByte(CP_UTF8, 0, w, w.GetLength(), nullptr, 0, nullptr, nullptr);
    std::string out(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w, w.GetLength(), &out[0], len, nullptr, nullptr);
    return out;
}
//유저 목록 추가 UI
LRESULT CClientList::OnUserList(WPARAM, LPARAM lParam)
{
    std::string* pLine = (std::string*)lParam;
    std::string line = *pLine;
    delete pLine;

    size_t pos = line.find('|');
    if (pos == std::string::npos) return 0;

    std::string head = line.substr(0, pos);
    std::string bodyA = line.substr(pos + 1);

    if (head == "USERLIST")
    {
        m_ClientList.ResetContent();

        size_t start = 0;
        while (true)
        {
            size_t comma = bodyA.find(',', start);
            std::string tok = bodyA.substr(
                start, comma == std::string::npos ? std::string::npos : comma - start);

            CString token = Utf8ToCString(tok);   // UTF-8 -> CString
            token.Trim();
            if (!token.IsEmpty())
                m_ClientList.AddString(token);

            if (comma == std::string::npos) break;
            start = comma + 1;
        }
    }
    return 0;
}
//   LIST_REQ 패킷 전송
void CClientList::RequestList()
{
    if (m_sock == INVALID_SOCKET) { 
        AfxMessageBox(_T("소켓이 생성 되지 않았습니다"));
        return; 
    }
    else {
        const char req[] = "LIST_REQ\n";
        send(m_sock, req, (int)strlen(req), 0);
    }

}
//  F5키로 새로고침 하기위한 함수
BOOL CClientList::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F5)
    {
        RequestList();
        return TRUE;      
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}
void CClientList::OnBnClickedButton1()
{
    std::vector<CChatDlg*> chats;
    for (auto& kv : m_chatMap) chats.push_back(kv.second);
    for (CChatDlg* c : chats) c->DestroyWindow();
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_sock != INVALID_SOCKET)
    {
        shutdown(m_sock, SD_BOTH);   // 블로킹 중인 recv()를 깨워서 RecvThread 종료 유도
        closesocket(m_sock);         // 서버 쪽에 연결 종료(FIN) 전달
        m_sock = INVALID_SOCKET;
    }
    Loginpage dlg;
    EndDialog(IDOK);
    dlg.DoModal();
}
//1대1 채팅창 열기 함수
CChatDlg* CClientList::OpenChat(const CString& name)
{
    auto it = m_chatMap.find(name);
    if (it != m_chatMap.end()) { it->second->SetForegroundWindow(); return it->second;; }
    
    CChatDlg* dlg = new CChatDlg(this);
    dlg->m_pList = this;
    dlg->m_strTarget = name;
    dlg->m_sock = m_sock;
    dlg->m_bShowMain = true;
    dlg->Create(IDD_CHAT_DIALOG, this);   // 모달리스


    auto h = m_history.find(name);
    if (h != m_history.end())
        for (auto& line : h->second)
            dlg->m_msg_list.InsertString(-1, line);

    dlg->m_msg_list.SetTopIndex(dlg->m_msg_list.GetCount() - 1);
    dlg->ShowWindow(SW_SHOW);
    m_chatMap[name] = dlg;
    ClearUnread(name);

    return dlg;
}
//서버에서 전달한 데이터 받기 
UINT CClientList::RecvThread(LPVOID pParam)
{
    CClientList* pList = (CClientList*)pParam;
    HWND   hWnd = pList->m_hWnd;
    SOCKET sock = pList->m_sock;          // 로그인에 쓴 소켓
    char buf[1024];
    std::string acc;

    while (true)
    {
        int n = recv(sock, buf, sizeof(buf), 0);
        if (n <= 0) break;

        acc.append(buf, n);
        size_t nl;
        while ((nl = acc.find('\n')) != std::string::npos)
        {
            std::string line = acc.substr(0, nl);
            acc.erase(0, nl + 1);
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue;

            CString* p = new CString(Utf8ToCString(line));
            if (::IsWindow(hWnd)) ::PostMessage(hWnd, WM_SERVER_LINE, 0, (LPARAM)p);
            else delete p;
        }
    }
    CString* pEnd = new CString(_T("DISCONNECTED"));
    if (::IsWindow(hWnd)) ::PostMessage(hWnd, WM_SERVER_LINE, 0, (LPARAM)pEnd);
    else delete pEnd;
    return 0;
}
//
LRESULT CClientList::OnServerLine(WPARAM, LPARAM lParam)
{
    CString* p = reinterpret_cast<CString*>(lParam);
    CString line = *p;
    delete p;
    if (line == _T("DISCONNECTED"))
    {
        AfxMessageBox(_T("서버와 연결이 끊어졌습니다."));
        return 0;
    }

    int pos = line.Find(_T('|'));
    CString cmd = (pos < 0) ? line : line.Left(pos);
    CString rest = (pos < 0) ? _T("") : line.Mid(pos + 1);

    if (cmd == _T("USERLIST"))
    {
        m_users.clear();
        int idx = 0;
        CString token;
        while (AfxExtractSubString(token, rest, idx++, _T(',')))
        {
            token.Trim();
            if (!token.IsEmpty()) m_users.push_back(token);
        }
        RefreshList();
    }
    else if (cmd == _T("MSG"))
    {
        int p2 = rest.Find(_T('|'));
        if (p2 < 0) return 0;
        CString from = rest.Left(p2); //보낸사람
        CString text = rest.Mid(p2 + 1);//보낸 메세지

        auto it = m_chatMap.find(from);
        if (it != m_chatMap.end())
            it->second->PostMessage(WM_CHAT_RECV, 0, (LPARAM)new CString(rest));
        else
        {
            AddHistory(from, from + _T(": ") + text);
            m_unread[from]++;
            RefreshList();

        }
    }
    return 0;
}

// 안읽음 개수 처리 함수 ex) "asd" -> "asd (3)"
CString CClientList::MakeLabel(const CString& name)
{
    auto it = m_unread.find(name);
    if (it == m_unread.end() || it->second <= 0) return name;
    CString s;
    s.Format(_T("%s (%d)"), (LPCTSTR)name, it->second);
    return s;
}
// 읽음 개수 처리 함수  ex ) asd (3)" -> "asd"
CString CClientList::StripBadge(const CString& s)
{
    int p = s.ReverseFind(_T(' '));
    if (p > 0 && s.Right(1) == _T(")") && s.Mid(p + 1, 1) == _T("("))
        return s.Left(p);
    return s;
}
// 안 읽음/읽음 재반영 함수
void CClientList::RefreshList()
{
    int sel = m_ClientList.GetCurSel();
    int top = m_ClientList.GetTopIndex();

    m_ClientList.SetRedraw(FALSE);
    m_ClientList.ResetContent();
    for (auto& name : m_users)
        m_ClientList.AddString(MakeLabel(name));
    if (sel != LB_ERR && sel < m_ClientList.GetCount())
        m_ClientList.SetCurSel(sel);
    m_ClientList.SetTopIndex(top);
    m_ClientList.SetRedraw(TRUE);
    m_ClientList.Invalidate();
}
//메세지 기억함수
void CClientList::AddHistory(const CString& name, const CString& line)
{
    m_history[name].push_back(line);
}

//채팅창 열때 안읽음 개수 제거 함수 
void CClientList::ClearUnread(const CString& name)
{
    if (m_unread.erase(name)) RefreshList();
}

void CClientList::OnStnClickedStaticName()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
void CClientList::OnDestroy()
{
    CDialogEx::OnDestroy();
}