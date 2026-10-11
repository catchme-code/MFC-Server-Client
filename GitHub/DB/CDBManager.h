#pragma once

#include <libpq-fe.h>
#include <afx.h>
#include <afxwin.h> 
#include <atlconv.h>
#include <mutex>

class CDBManager
{
private:
	PGconn* m_pConn;
	std::mutex m_mtx;
public:
	CDBManager();
	~CDBManager();
	CString m_name; // 닉네임 
	CString m_id; // 아이디
	CString m_password; // 비밀번호 

	BOOL Connect();
	void Disconnect();

	//PGconn* GetConnection();

	BOOL InsertData(CString m_name, CString m_id, CString m_password);
	BOOL SelectData(CString m_id, CString m_password, CString& LoginName, CString& LoginId, CString& LoginPassword);

};

