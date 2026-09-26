#pragma once
#include "afxdialogex.h"


// TCP_Client 대화 상자

class TCP_Client : public CDialogEx
{
	DECLARE_DYNAMIC(TCP_Client)
private:
	SOCKET server_socket;//서버 소켓 
public:
	TCP_Client(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~TCP_Client();

	CListBox m_msg_list; // 메시지 ListBox
	CEdit ipEdit; // 서버 ip 입력 EditBox
	CEdit portEdit; // 서버 port 입력 EditBox
	CEdit msgEdit; // 메시지 입력 EditBox

	void InsertMsg(CString str); // 메시지 ListBox 메시지 추가
	afx_msg void ConnectBtnClicked(); // 서버 연결 버튼 클릭
	afx_msg void SendBtnClicked(); // 메시지 송신 버튼
	static UINT RecvFunc(LPVOID pParam); // 메시지 수신 버튼

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();//다이어로그 생성시 
	afx_msg void OnDestroy();//다이어로그 소멸시

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit2();
};
