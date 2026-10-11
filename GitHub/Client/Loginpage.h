#pragma once
#include "afxdialogex.h"
#include <libpq-fe.h>
#include "CClientList.h"

#define WM_LOGIN_RESULT (WM_USER + 1)
#define WM_CHAT_RECV (WM_USER + 10)
#define WM_USERLIST  (WM_USER + 11)
// Loginpage 대화 상자

class Loginpage : public CDialogEx
{
	DECLARE_DYNAMIC(Loginpage)
private:
	
	CList<SOCKET> userList;
public:
	Loginpage(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	SOCKET server_socket;//서버 접속 소켓 
	
	HWND   m_hListWnd = NULL;

	virtual ~Loginpage();
	
	CString id; // 아이디
	CString passwd;// 비밀번호
	CEdit m_id; //아이디 입력 EditBox
	CEdit m_password; //비밀번호 입력 EditBox

	SOCKET m_sock;

	virtual BOOL OnInitDialog();
	static UINT RecvThread(LPVOID p);
	void InsertClient(CString str);
	static UINT AcceptFunc(LPVOID pParam);
	

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = Login };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit5();
	afx_msg void OnEnChangeEdit4();
	afx_msg LRESULT OnLoginResult(WPARAM wParam, LPARAM lParam);
};
