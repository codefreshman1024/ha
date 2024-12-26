#include<json/json.h>
#include<fstream>
#include<thread>

#include "ConnectionPool.h"
using namespace Json;

ConnectionPool* ConnectionPool::getConnectionPool()
{
    static ConnectionPool pool;
    return &pool;
}

bool ConnectionPool::loadjson()
{
    ifstream ifs("dbconnection.json");
    Reader rd;
    Value root;
    rd.parse(ifs, root);
    if (root.isObject()) {
        m_ip = root["ip"].asString();
        m_port = root["port"].asInt();
        m_user = root["userName"].asString();
        m_password = root["password"].asString();
        m_dbName = root["dbName"].asString();
        m_minsize = root["minsize"].asInt();
        m_maxsize = root["maxsize"].asInt();
        maxIdleTime = root["maxIdleTime"].asInt();
        timeout = root["timeout"].asInt();
        return true;
    }
    return false;
}

void ConnectionPool::producecon()
{
    while (true) {
        unique_lock<mutex> locker(m_mutexQ);
        while (m_connQ.size() >= m_minsize) {
            m_cond.wait(locker);
        }
        addconnection();
        m_cond.notify_all(); //唤醒消费者
    }
}

void ConnectionPool::recyclecon()
{
    while (true) {
        this_thread::sleep_for(chrono::milliseconds(500));
        lock_guard<mutex> locker(m_mutexQ);
        while (m_connQ.size() > m_minsize) {
            MysqlConn* conn1 = m_connQ.front();
            if (conn1->get_relive() >= maxIdleTime) {
                m_connQ.pop();
                delete conn1;
            }
            else {
                break;
            }
        }
    }
}

void ConnectionPool::addconnection()
{
    MysqlConn* conn = new MysqlConn;
    conn->connect(m_ip, m_user, m_password, m_dbName, m_port);
    conn->refresh_alive();
    m_connQ.push(conn);
}


shared_ptr<MysqlConn> ConnectionPool::getConnection()
{
    unique_lock<mutex> locker(m_mutexQ);
    while (m_connQ.empty()) {
        if (cv_status::timeout == m_cond.wait_for(locker, chrono::milliseconds(timeout))) {
            if (m_connQ.empty()) {
                continue;
            }
        }
    }
    shared_ptr<MysqlConn> connptr(m_connQ.front(), [this](MysqlConn* conn) {
        lock_guard<mutex> locker(m_mutexQ);
        conn->refresh_alive();
        m_connQ.push(conn);
    });
    m_connQ.pop();
    m_cond.notify_all();
    return connptr;
}

ConnectionPool::~ConnectionPool()
{
    while (!m_connQ.empty()) {
        MysqlConn* conn = m_connQ.front();
        m_connQ.pop();
        delete conn;
    }
}

ConnectionPool::ConnectionPool() {

    //加载配置文件
    if (!loadjson()) {
        return;
    }
    for (int i = 0; i < m_minsize; i++) {
        addconnection();
    }
    thread producer(&ConnectionPool::producecon, this);
    thread recycler(&ConnectionPool::recyclecon, this);
    producer.detach();
    recycler.detach();

}
