
// ChatDlg.h: 헤더 파일
//

#pragma once

#include "Loginpage.h"

#define WM_CHAT_RECV (WM_USER + 10)

// CChatDlg 대화 상자
class CChatDlg : public CDialogEx
{
// 생성입니다.
private:
	
public:
	CChatDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	bool m_bShowMain = false; //1대1 채팅창 열기 on/off
	CClientList* m_pList = nullptr;   // 소유 목록창
	CString m_strTarget;// 보내는 사람이름
	SOCKET m_sock; //서버 소켓 
	CListBox m_msg_list; // 메시지 ListBox UI
	CEdit msgEdit; // 메시지 입력 EditBox UI
	CString m_strUser;//유저 이름

	void InsertMsg(CString str); 
	void PostNcDestroy();
	void OnCancel();
	virtual void OnOK();

	static UINT RecvFunc(LPVOID pParam);
	

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHAT_DIALOG };
#endif

	protected:
		Loginpage m_subDlg3;

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedButton4();
	afx_msg void SendBtnClicked(); 
	afx_msg LRESULT OnChatRecv(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnStnClickedStaticText();
};
