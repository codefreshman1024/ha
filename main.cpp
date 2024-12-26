#include<iostream>
#include<memory>
#include"MyConnection.h"
#include"ConnectionPool.h"

void query() {
	MysqlConn conn;
	conn.connect("127.0.0.1", "root", "123456", "testdb");
	string sql = "insert into person values(123, 'wukong', 500, '0')";
	bool flag = conn.update(sql);
	cout << "flag value : " << flag << endl;

	sql = "select * from person";
	conn.query(sql);
	while (conn.next()) {
		//���ÿһ������ֵ
		cout << conn.value(0) << "," <<
			conn.value(1) << "," <<
			conn.value(2) << "," <<
			conn.value(3) << endl;
	}
}

//���Ե��̺߳Ͷ��̣߳��������ӳص�Ч��
void function1(int begin, int end) {
	for (int i = begin; i <= end; i++) {
		MysqlConn conn;
		conn.connect("127.0.0.1", "root", "123456", "testdb");
		char sql[1024] = {0};
		sprintf(sql, "insert into person values(%d, 'xiaoming', 12, '0')", i);
		conn.update(sql);

	}
}

void function2(ConnectionPool* pool, int begin, int end) {
	for (int i = begin; i <= end; i++) {
		shared_ptr<MysqlConn> conn = pool->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into person values(%d, 'xiaoming', 12, '0')", i);
		conn->update(sql);
	}
}

void test1() {
#if 0
	//���߳������ݿ����ӳ�����ʱ��Ϊ��11887334000����,11887����
	steady_clock::time_point start = steady_clock::now();
	function1(0, 2500);
	steady_clock::time_point end = steady_clock::now();
	auto length = end - start;
	cout << "���߳������ݿ����ӳ�����ʱ��Ϊ��" << length.count() << "����," << length.count() / 1000000 << "����" << endl;
#else
	//���߳����ݿ����ӳ�����ʱ��Ϊ��3053426500����,3053����
	ConnectionPool* pool = ConnectionPool::getConnectionPool();
	steady_clock::time_point start = steady_clock::now();
	function2(pool, 0, 2500);
	steady_clock::time_point end = steady_clock::now();
	auto length = end - start;
	cout << "���߳����ݿ����ӳ�����ʱ��Ϊ��" << length.count() << "����," << length.count() / 1000000 << "����" << endl;
#endif
}

void test2() {
#if 0
	//���߳������ݿ����ӳ�����ʱ��Ϊ��3161198800����,3161����
	MysqlConn conn;
	conn.connect("127.0.0.1", "root", "123456", "testdb");
	steady_clock::time_point start = steady_clock::now();
	thread t1(function1, 0, 500);
	thread t2(function1, 500, 1000);
	thread t3(function1, 1000, 1500);
	thread t4(function1, 1500, 2000);
	thread t5(function1, 2000, 2500);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	steady_clock::time_point end = steady_clock::now();
	auto length = end - start;
	cout << "���߳������ݿ����ӳ�����ʱ��Ϊ��" << length.count()<< "����," << length.count() / 1000000 << "����" << endl;
#else
	//���߳����ݿ����ӳ�����ʱ��Ϊ��963901700����,963����
	ConnectionPool* pool = ConnectionPool::getConnectionPool();
	steady_clock::time_point start = steady_clock::now();
	thread t1(function2, pool, 0, 500);
	thread t2(function2, pool, 500, 1000);
	thread t3(function2, pool, 1000, 1500);
	thread t4(function2, pool, 1500, 2000);
	thread t5(function2, pool, 2000, 2500);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	steady_clock::time_point end = steady_clock::now();
	auto length = end - start;
	cout << "���߳����ݿ����ӳ�����ʱ��Ϊ��" << length.count() << "����," << length.count() / 1000000 << "����" << endl;
#endif

}

int main() {
	//teat1();
	//test2();
	query();
	return 0;
}