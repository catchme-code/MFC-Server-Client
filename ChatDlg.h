
// ChatDlg.h: 헤더 파일
//

#pragma once

#include "TCP_Client.h"


// CChatDlg 대화 상자
class CChatDlg : public CDialogEx
{
// 생성입니다.
private:
	SOCKET server_socket;
public:
	CChatDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	CListBox m_msg_list; // 메시지 ListBox
	CEdit ipEdit; // 서버 ip 입력 EditBox
	CEdit portEdit; // 서버 port 입력 EditBox
	CEdit msgEdit; // 메시지 입력 EditBox
	

	void InsertMsg(CString str); // 메시지 ListBox 메시지 추가
	afx_msg void ConnectBtnClicked(); // 서버 연결 버튼 클릭
	afx_msg void SendBtnClicked(); // 메시지 송신 버튼
	static UINT RecvFunc(LPVOID pParam);
	

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHAT_DIALOG };
#endif

	protected:
		TCP_Client m_subDlg2;

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedButton4();

	
};
