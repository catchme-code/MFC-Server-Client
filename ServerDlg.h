
// ServerDlg.h: 헤더 파일
//

#pragma once


// CServerDlg 대화 상자
class CServerDlg : public CDialogEx
{
// 생성입니다.

private :
	SOCKET server_socket;//서버 소켓 
	CList<SOCKET> userList;//접속한 클라이언트 리스트 
public:
	CServerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	CEdit portEdit;// 포트 입력 BOX
	CListBox m_msg_ListBox;//메세지 전시 List Box
	CListBox m_Client_ListBox; // 클라이언트 접속 List Box 


	afx_msg void ConnectBntClicked();
	void InsertMsg(CString str); // 메시지 ListBox에 메시지 추가
	void InsertClient(CString str); // 클라이언트 ListBox에 유저 추가
	void SendToAll(SOCKET socket, CString str); // 연결된 클라이언트에 메시지 송신
	static UINT AcceptFunc(LPVOID pParam); // 클라이언트 연결 대기 스레드
	static UINT RecvFunc(LPVOID pParam); // 메시지 수신 대기 스레드
	afx_msg LRESULT OnAddMessage(WPARAM wParam, LPARAM lParam);

	typedef struct _MyData {
		CServerDlg* m_pDlg;
		SOCKET c_sock;
		SOCKADDR_IN c_addr;
	} MyData; // 메시지 수신 스레드에 매개로 넘겨줄 구조체

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
	
	


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeVersionInfo();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedButton1();
};
