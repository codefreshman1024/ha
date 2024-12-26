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
		//输出每一行所有值
		cout << conn.value(0) << "," <<
			conn.value(1) << "," <<
			conn.value(2) << "," <<
			conn.value(3) << endl;
	}
}

//测试单线程和多线程，有无连接池的效率
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
	//单线程无数据库连接池所耗时间为：11887334000纳秒,11887毫秒
	steady_clock::time_point start = steady_clock::now();
	function1(0, 2500);
	steady_clock::time_point end = steady_clock::now();
	auto length = end - start;
	cout << "单线程无数据库连接池所耗时间为：" << length.count() << "纳秒," << length.count() / 1000000 << "毫秒" << endl;
#else
	//单线程数据库连接池所耗时间为：3053426500纳秒,3053毫秒
	ConnectionPool* pool = ConnectionPool::getConnectionPool();
	steady_clock::time_point start = steady_clock::now();
	function2(pool, 0, 2500);
	steady_clock::time_point end = steady_clock::now();
	auto length = end - start;
	cout << "单线程数据库连接池所耗时间为：" << length.count() << "纳秒," << length.count() / 1000000 << "毫秒" << endl;
#endif
}

void test2() {
#if 0
	//多线程无数据库连接池所耗时间为：3161198800纳秒,3161毫秒
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
	cout << "多线程无数据库连接池所耗时间为：" << length.count()<< "纳秒," << length.count() / 1000000 << "毫秒" << endl;
#else
	//多线程数据库连接池所耗时间为：963901700纳秒,963毫秒
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
	cout << "多线程数据库连接池所耗时间为：" << length.count() << "纳秒," << length.count() / 1000000 << "毫秒" << endl;
#endif

}

int main() {
	//teat1();
	//test2();
	query();
	return 0;
}