#ifndef DATABASEMYSQL_H_
#define DATABASEMYSQL_H_

#include<winsock.h>
#include<mysql.h>
#include<iostream>

using namespace std;


//��Ҫ������c++���������������ڴ治��й¶����result��Ϊ˽�г�Աֻ��ͨ��set�ı䣬ͨ��get��ȡ
class SelectResult
{
private:
	MYSQL_RES* result = NULL;//������Ž����
public:
	

	SelectResult(MYSQL_RES* result);
	SelectResult();
	~SelectResult();
	MYSQL_RES* getResult();

	//�ж�result�ǲ���Ϊ�գ����Ƚ�result�ͷ��ٽ�result��ֵ
	void setResult(MYSQL_RES* result);


};




class MySQLC
{
private:
	
	char host[200];
	char user[40];
	char password[40];
	char database[40];
	unsigned int port;
	bool state;			//�ж�����״̬��1��ʾmysqlCon���ӳɹ���0��mysqlCon����ʧ��
	MYSQL* mysqlCon;

	bool connectMysql();


public:

	MySQLC(char* host, char* user, char* password,char* databaseName, unsigned int port);

	bool query(string sql);

	//result��Ҫ�ֶ��ͷ�
	MYSQL_RES* select(string sql);

	//result���Լ��ͷ�
	bool select(string sql, SelectResult& res);
	
	~MySQLC();



};



#endif