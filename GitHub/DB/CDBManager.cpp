#include "pch.h"
#include "CDBManager.h"
#include <atlconv.h>
#include <string>
#include <vector>

CDBManager::CDBManager()
{
	m_pConn = nullptr;
}

CDBManager::~CDBManager()
{
	Disconnect();
}


//PQexec() : SQL 쿼리를 실행합니다.

//PQntuples() : 조회된 행의 개수를 반환합니다.

//PQgetvalue() : 지정한 행과 열의 값을 가져옵니다.

//PQclear() : 쿼리 결과에 할당된 메모리를 해제합니다.

//postgredsql DB 접속 함수 
BOOL CDBManager::Connect()
{
	const char* connInfo =
		"host=localhost "
		"port=1234 "
		"dbname=test_db "
		"user=test "
		"password=1234"
		;

	m_pConn = PQconnectdb(connInfo);

	if (PQstatus(m_pConn) != CONNECTION_OK)
	{
		CString error;
		error = CA2T(PQerrorMessage(m_pConn));

		AfxMessageBox(error);

		PQfinish(m_pConn);
		m_pConn = nullptr;

		return FALSE;
	}
	return TRUE;
}
// Utf8 -> CString 형변환 
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

//sql 삽입 함수
BOOL CDBManager::InsertData(CString name, CString id, CString password)
{
	

	// CString -> UTF-8 변환 인코딩의 차이 떄문에 
	CT2A nameA(name, CP_UTF8);
	CT2A idA(id, CP_UTF8);
	CT2A passwordA(password, CP_UTF8);


	const char* paramValues[3] =
	{
		nameA,
		idA,
		passwordA
	};
	PGresult* res = nullptr;
	
	std::lock_guard<std::mutex> lock(m_mtx);

	if (m_pConn == nullptr ||
		PQstatus(m_pConn) != CONNECTION_OK)
	{
		TRACE(_T("DB 연결이 되어 있지 않습니다.\n"));
		return FALSE;
	}
		// SQL 실행
	res = PQexecParams(
		m_pConn,
		"INSERT INTO test_table (name, id, password) VALUES ($1, $2, $3)",
		3,              // 매개변수 개수
		nullptr,        // 자료형 자동 추론
		paramValues,    // 매개변수 값
		nullptr,        // 길이
		nullptr,        // 텍스트 형식
		0               // 결과 형식: 텍스트
	);
	
	if (res == nullptr)
	{
		TRACE(_T("PQexecParams 결과가 NULL입니다.\n"));
		return FALSE;
	}

	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		// AfxMessageBox 대신 로그만
		TRACE("DB INSERT 실패: %s\n", PQresultErrorMessage(res));
		PQclear(res);
		return FALSE;
	}
	PQclear(res);

	return TRUE;
}

//sql 조회 함수
 BOOL CDBManager::SelectData(CString id, CString password,
CString& LoginName, CString& LoginId, CString& LoginPassword)
{
	 std::string idA = CStringToUtf8(id);
	 std::string passwordA = CStringToUtf8(password);

	 const char* paramValues[2] = { idA.c_str(), passwordA.c_str() };

	 PGresult* res = nullptr;

	 std::lock_guard<std::mutex> lock(m_mtx); 

	if (m_pConn == nullptr || PQstatus(m_pConn) != CONNECTION_OK)
	{
		TRACE(_T("DB 연결이 되어 있지 않습니다.\n"));
		return FALSE;
	}

	res = PQexecParams(
		m_pConn,
		"SELECT name, id, password FROM test_table WHERE id=$1 AND password=$2",
		2, nullptr, paramValues, nullptr, nullptr, 0);

	if (res == nullptr) return FALSE;

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		TRACE("DB SELECT 실패: %s\n", PQresultErrorMessage(res));
		PQclear(res);
		return FALSE;
	}

	int rowCount = PQntuples(res);
	for (int row = 0; row < rowCount; row++)
	{
		LoginName = Utf8ToCString(PQgetvalue(res, row, 0));
		LoginId = Utf8ToCString(PQgetvalue(res, row, 1));
		LoginPassword = Utf8ToCString(PQgetvalue(res, row, 2));
	}

	PQclear(res);
	return TRUE;
}

 // 연결 종료시 자원 정리 
void CDBManager::Disconnect()
{
	if (m_pConn != nullptr)
	{
		PQfinish(m_pConn);
		m_pConn = nullptr;
	}
}