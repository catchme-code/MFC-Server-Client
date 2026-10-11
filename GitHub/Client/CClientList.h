#pragma once
#include "afxdialogex.h"
#include <map>
#include <vector>


#define WM_USER_LIST (WM_USER + 3)
#define WM_SERVER_LINE (WM_USER + 20)

// CClientList 대화 상자
class CChatDlg;
class Loginpage;

class CClientList : public CDialogEx
{
	DECLARE_DYNAMIC(CClientList)

public:

	CClientList(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	SOCKET m_sock; //로그인 창에서 받을 서버소켓 
	//Loginpage* m_pLogin = nullptr;
	virtual ~CClientList();
	CListBox m_ClientList; // 클라이언트 목록 UI
	CString m_strName; // 회원가입 시 입력한 닉네임
	std::map<CString, CChatDlg*> m_chatMap; // 이름 -> 채팅창 open 유무 기억
	std::map<CString, int> m_unread;          // 이름 -> 안 읽은 개수
	std::vector<CString> m_users;             // 이름 보관
	std::map<CString, std::vector<CString>> m_history;   // 상대 이름 -> 화면에 표시되는 줄 목록
	

	virtual BOOL OnInitDialog();
	void RequestList();     
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CChatDlg* OpenChat(const CString& name);
	static UINT RecvThread(LPVOID pParam);
	LRESULT OnServerLine(WPARAM, LPARAM lParam);
	LRESULT OnChatRecv(WPARAM w, LPARAM l);
	CString MakeLabel(const CString& name); 
	CString StripBadge(const CString& s);    
	void  RefreshList();                  
	void AddHistory(const CString& name, const CString& line);
	void  ClearUnread(const CString& name); 
	void OnDestroy();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = ClientList };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnStnClickedStaticName();
	afx_msg LRESULT OnUserList(WPARAM, LPARAM lParam);
	afx_msg void OnLbnDblclkList1();
};
