#pragma once
#include<iostream>
#include<mysql.h>
#include<chrono>
using namespace std;
using namespace chrono;


class MysqlConn
{
public:
	//��ʼ�����ݿ�
	MysqlConn();
	//�ͷ����ݿ�
	~MysqlConn();
	//�������ݿ�
	bool connect(string ip, string user, string password, string dbName, unsigned short port = 3306);
	//�������ݿ�: insert, delete, update
	bool update(string sql);
	//��ѯ���ݿ�
	bool query(string sql);
	//������ѯ�Ľ����
	bool next();
	//�õ�������е��ֶ�
	string value(int index);
	//�������
	bool transaction();
	//�ύ����
	bool commit();
	//����ع�
	bool rollback();
	//ˢ����ʼ����ʱ���
	void refresh_alive();
	//��¼����ʱ��
	long long get_relive();
private:
	void freeres(); //�ͷŽ����ָ��
	MYSQL* m_conn = nullptr;
	MYSQL_RES* m_result = nullptr;
	MYSQL_ROW m_row = nullptr; //ָ��ÿһ�е�ָ�룬����ָ��
	steady_clock::time_point m_alivetime;
};