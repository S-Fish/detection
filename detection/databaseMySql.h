#ifndef DATABASEMYSQL_H_
#define DATABASEMYSQL_H_

#include<winsock.h>
#include<mysql.h>
#include<iostream>
#include<vector>
#include<string>

using namespace std;

//������id
struct WoodFeature
{
	string id;
	vector<float> features;
};


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

//����ʹ��
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

	//������ӣ���Ҫ��������һЩ������Ҫ����״̬
	MYSQL* getConnect();

	//result��Ҫ�ֶ��ͷ�
	MYSQL_RES* select(string sql);

	//result���Լ��ͷ�
	bool select(string sql, SelectResult& res);
	
	~MySQLC();



};


//����ľ������
class InsertWood{
private:
	string id;//�����ʱ��õ���ľ��id

public:

	//ľ�ĵĳ���ߣ���ľ����Ƭ��·��,���ر������ľ��id,����û������id��ֱ�������������ӵ���������,����ɹ��򷵻�ľ��id���򷵻�""��
	//path���Բ��ùܣ�����ǿ����˺���Ҫ�Ǽ���ľ����Ƭ
	string insertWood(MySQLC& mysqlc, float length, float width, float height,string id="" ,string path = "");


	//����һ��woodId��ľ�ĵ����������Ϊ�����ǲ�����һ����ľ��id
	bool insertFeature(MySQLC& mysqlc, string featureName, const vector<float>& features,const string& woodId="");
	




	//��������û��ʵ��
	//����ж��ǲ�����ȱ��,���������ľ��ȱ��
	bool updateWood(MySQLC& mysqlc, bool isDefctive, const string& woodId = "");

	//����ȱ��
	bool insertDefects(MySQLC& mysqlc, const string& woodId = "");
	InsertWood();
	~InsertWood();


};


//�����ݿ������ȡ����
class  ReadWood
{

private:

public:
	 ReadWood();
	~ ReadWood();

	//������е�ľ�ĵ�featureName����
	vector<WoodFeature> getAllWood(MySQLC& mysqlc, const string& featureName);
};



#endif