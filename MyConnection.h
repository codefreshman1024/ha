#pragma once
#include<iostream>
#include<mysql.h>
#include<chrono>
using namespace std;
using namespace chrono;


class MysqlConn
{
public:
	//初始化数据库
	MysqlConn();
	//释放数据库
	~MysqlConn();
	//连接数据库
	bool connect(string ip, string user, string password, string dbName, unsigned short port = 3306);
	//更新数据库: insert, delete, update
	bool update(string sql);
	//查询数据库
	bool query(string sql);
	//遍历查询的结果集
	bool next();
	//得到结果集中的字段
	string value(int index);
	//事务操作
	bool transaction();
	//提交事务
	bool commit();
	//事务回滚
	bool rollback();
	//刷新起始空闲时间点
	void refresh_alive();
	//记录存活的时间
	long long get_relive();
private:
	void freeres(); //释放结果集指针
	MYSQL* m_conn = nullptr;
	MYSQL_RES* m_result = nullptr;
	MYSQL_ROW m_row = nullptr; //指向每一行的指针，二级指针
	steady_clock::time_point m_alivetime;
};