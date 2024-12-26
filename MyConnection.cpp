#include "MyConnection.h"

MysqlConn::MysqlConn() {
	m_conn = mysql_init(nullptr);
	mysql_set_character_set(m_conn, "utf8");	//设置统一编码
}

MysqlConn::~MysqlConn() {
	if (m_conn != nullptr) {
		mysql_close(m_conn);
	}
	freeres();
}

bool MysqlConn::connect(string ip, string user, string password, string dbName, unsigned short port)
{
	MYSQL* ptr = mysql_real_connect(m_conn, ip.c_str(), user.c_str(), password.c_str(), dbName.c_str(), port, nullptr, 0);

	return ptr!=nullptr;
}

bool MysqlConn::update(string sql)
{
	if (mysql_query(m_conn, sql.c_str())) {
		return false;
	}
	return true;
}

bool MysqlConn::query(string sql)
{
	freeres();
	if (mysql_query(m_conn, sql.c_str())) {
		return false;
	}
	m_result = mysql_store_result(m_conn);
	return true;
}

bool MysqlConn::next()
{
	if (m_result != nullptr) {
		m_row = mysql_fetch_row(m_result);
		if (m_row != nullptr) {
			return true;
		}
	}
	return false;
}

string MysqlConn::value(int index)
{
	int rownum = mysql_num_fields(m_result);
	if (index < 0 || index > rownum) {
		return string();
	}
	char* res = m_row[index];
	unsigned long length = mysql_fetch_lengths(m_result)[index];
	return string(res, length);	//避免以'\0'结尾
}

bool MysqlConn::transaction()
{
	return mysql_autocommit(m_conn, false);
}

bool MysqlConn::commit()
{
	return mysql_commit(m_conn);
}

bool MysqlConn::rollback()
{
	return mysql_rollback(m_conn);
}

void MysqlConn::refresh_alive()
{
	m_alivetime = steady_clock::now();
}

long long MysqlConn::get_relive()
{
	nanoseconds nanotime = steady_clock::now() - m_alivetime;
	milliseconds res = duration_cast<milliseconds>(nanotime);
	return res.count();
}

void MysqlConn::freeres()
{
	if (m_result) {
		mysql_free_result(m_result);
		m_result = nullptr;
	}
}

