#include"databaseMySql.h"



//MySQLC
MySQLC::MySQLC(char* host, char* user, char* password, char* databaseName, unsigned int port){
	strcpy_s(this->host, host);
	strcpy_s(this->user, user);
	strcpy_s(this->password, password);
	strcpy_s(this->database, databaseName);
	this->port = port;

	this->state = 0;
	this->connectMysql();
}
MySQLC::~MySQLC(){
	if (state == 1){
		mysql_close(mysqlCon);
	}

}

bool MySQLC::connectMysql(){
	
	bool result = 1;

	//��ʾ��������Լ������˲��ý������Ӳ���
	if (state == 1){
		return result;
	}

	//�ж������ǲ�����ȷ��
	if (strlen(this->host) == 0){
		cout << "please check host,user,password" << endl;
	}

	//��ʼ�����ݿ�
	if (0 == mysql_library_init(0, NULL, NULL)){
		cout << "mysql_library_init() succeed"<<endl;
	}else{
		cout << "mysql_library_init() succeed" << endl;
		return 0;
	}

	//��ʼ������
	mysqlCon = mysql_init((MYSQL*)0);

	//����ѡ��
	if (0 == mysql_options(mysqlCon, MYSQL_SET_CHARSET_NAME, "gbk")) {
		cout << "mysql_options() succeed" << endl;
	}
	else {
		cout << "mysql_options() failed" << endl;
		return 0;
	}
	

	//�������ݿ�
	if (NULL!= mysql_real_connect(mysqlCon,host,user,password,database,port,NULL,0)){
		cout << "mysql_real_connect() succeed" << endl;
		this->state = 1;
	}
	else {
		cout << "mysql_real_connect() failed,please check user��host��dbname,port" << endl;
		this->host[0] = '\0';								//�������һ������������host��user��password��port��database������
		return 0;
	}


	return result;

}

bool MySQLC::query(string sql){
	
	if (state == 0){
		if (0 == connectMysql()){
			cout << "���ݿ����Ӵ���" << endl;
			return 0;
		}
	}

	if (0 == mysql_query(mysqlCon, sql.c_str())) {
		cout << sql.c_str()<<" excute sucess" << endl;
	}else {
		cout << sql.c_str()<<" excute failed" << endl;
		mysql_close(mysqlCon);
		state = 0;
		return 0;
	}
	return 1;

	
}


MYSQL_RES* MySQLC::select(string sql){

	MYSQL_RES *result = NULL;

	if (state == 0){
		if (0 == connectMysql()){
			cout << "���ݿ����Ӵ���" << endl;
			return 0;
		}
	}

	if (0 == mysql_query(mysqlCon, sql.c_str())) {
		cout << sql.c_str() << " excute sucess" << endl;
		result = mysql_store_result(mysqlCon);
	}
	else {
		cout << sql.c_str() << " excute failed" << endl;
		mysql_close(mysqlCon);
		state = 0;
	}

	return result;
}




bool MySQLC::select(string sql,SelectResult& res){


	MYSQL_RES *result = NULL;

	if (state == 0){
		if (0 == connectMysql()){
			cout << "���ݿ����Ӵ���" << endl;
			return 0;
		}
	}

	if (0 == mysql_query(mysqlCon, sql.c_str())) {
		cout << sql.c_str() << " excute sucess" << endl;
		result = mysql_store_result(mysqlCon);
		res.setResult(result);
	}
	else {
		cout << sql.c_str() << " excute failed" << endl;
		mysql_close(mysqlCon);
		state = 0;
	}

	return 1;
}

//SelctResult
SelectResult::~SelectResult(){

	cout << "result is destory ing..."<<endl;

	if (NULL!= this->result ){

		cout << "result is freed" << endl;
		mysql_free_result(result);

	}
	
}

SelectResult::SelectResult(MYSQL_RES* result){
	this->result = result;
}

SelectResult::SelectResult(){
	this->result = NULL;
}

void SelectResult::setResult(MYSQL_RES* result){
	if (NULL != this->result){
		mysql_free_result(this->result);
		this->result = NULL;
	}
	this->result =  result;
}