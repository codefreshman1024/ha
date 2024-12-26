#pragma once
#include<queue>
#include<mutex>
#include<condition_variable>
using namespace std;
#include "MyConnection.h"
class ConnectionPool
{
public:
	static ConnectionPool* getConnectionPool();
	ConnectionPool(const ConnectionPool& obj) = delete; //ɾ��Ĭ�Ϲ��캯��������ģʽ����ֻ����һ��
	ConnectionPool& operator= (const ConnectionPool & obj) = delete;	//ͬ��ɾ��Ĭ�Ͽ������캯��
	shared_ptr<MysqlConn> getConnection();
	~ConnectionPool();
private:
	ConnectionPool();
	bool loadjson();
	void producecon();
	void recyclecon();
	void addconnection();

	string m_ip;
	string m_user;
	string m_password;
	string m_dbName;
	unsigned short m_port;

	int m_minsize;
	int m_maxsize;
	int timeout;
	int maxIdleTime;
	queue<MysqlConn*> m_connQ;
	mutex m_mutexQ;
	condition_variable m_cond;
};

