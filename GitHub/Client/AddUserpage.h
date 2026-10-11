#pragma once
#include "afxdialogex.h"
#include <libpq-fe.h>


// AddUserpage 대화 상자

class AddUserpage : public CDialogEx
{
	DECLARE_DYNAMIC(AddUserpage)
public:
	AddUserpage(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~AddUserpage();
	CString name; // 닉네임
	CString id; // 아이디
	CString password; //비밀번호

	CEdit m_name; // 닉네임 EditBox UI
	CEdit m_id; // 아이디 EditBox UI
	CEdit m_password; // 비밀번호 EditBox UI


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = AddUser };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeEdit4();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButton2();
};
