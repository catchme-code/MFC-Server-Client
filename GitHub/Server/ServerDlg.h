
// ServerDlg.h: 헤더 파일
//

#pragma once
#include <string>
#include <vector>
#include <atomic>

struct Clientinfo{
	SOCKET sock;
	CString Clientname;
};

// CServerDlg 대화 상자
class CServerDlg : public CDialogEx
{
// 생성입니다.

private :
	SOCKET server_socket;//서버 소켓 

public:
	std::atomic<bool> m_listDirty{ false };   // 목록 변경 플래그
	std::atomic<bool> m_running{ false };  // 브로드캐스트 스레드 동작 여부
	CServerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	CList<Clientinfo> m_clientList;//목록창 리스트에 표출 할 클라이언트 정보
	CCriticalSection m_cs; //스레드 동시 접근 예방
	CList<SOCKET> userList;//접속한 클라이언트 소켓  리스트
	CEdit portEdit;// 포트 입력 BOX
	CListBox m_msg_ListBox;//메세지 전시 List Box
	CListBox m_Client_ListBox; // 클라이언트 접속 List Box UI
	CWinThread* m_pBcastThread = nullptr;

	
	void InsertMsg(CString str); 
	void InsertClient(CString str); 
	static UINT AcceptFunc(LPVOID pParam);
	static UINT CheckPacket(LPVOID pParam); 
	void SendToUser(const CString& name, const std::string& data); 
	void BroadcastClientList(); 
	static UINT BroadcastThreadFunc(LPVOID pParam);
	void SendListTo(SOCKET sock);             // 요청한 한 명에게만 전송

	typedef struct _MyData {
		CServerDlg* m_pDlg;
		SOCKET c_sock;
		SOCKADDR_IN c_addr;
	} MyData; // 서버 데이터 

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
	afx_msg void ConnectBntClicked();
	afx_msg LRESULT OnAddMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRemoveClient(WPARAM wParam, LPARAM lParam);
};
