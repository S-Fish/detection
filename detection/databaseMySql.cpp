#include"databaseMySql.h"
#include<sstream>


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

	//表示这个连接以及连接了不用进行连接操作
	if (state == 1){
		return result;
	}

	//判断主机是不是正确的
	if (strlen(this->host) == 0){
		cout << "please check host,user,password" << endl;
	}

	//初始化数据库
	if (0 == mysql_library_init(0, NULL, NULL)){
		cout << "mysql_library_init() succeed"<<endl;
	}else{
		cout << "mysql_library_init() succeed" << endl;
		return 0;
	}

	//初始化连接
	mysqlCon = mysql_init((MYSQL*)0);

	//设置选项
	if (0 == mysql_options(mysqlCon, MYSQL_SET_CHARSET_NAME, "gbk")) {
		cout << "mysql_options() succeed" << endl;
	}
	else {
		cout << "mysql_options() failed" << endl;
		return 0;
	}
	

	//连接数据库
	if (NULL!= mysql_real_connect(mysqlCon,host,user,password,database,port,NULL,0)){
		cout << "mysql_real_connect() succeed" << endl;
		this->state = 1;
	}
	else {
		cout << "mysql_real_connect() failed,please check user，host，dbname,port" << endl;
		this->host[0] = '\0';								//如果在这一步出错，可能是host，user，password，port，database有问题
		return 0;
	}


	return result;

}

bool MySQLC::query(string sql){
	
	if (state == 0){
		if (0 == connectMysql()){
			cout << "数据库连接错误" << endl;
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

MYSQL* MySQLC::getConnect(){

	return this->mysqlCon;
}

MYSQL_RES* MySQLC::select(string sql){

	MYSQL_RES *result = NULL;

	if (state == 0){
		if (0 == connectMysql()){
			cout << "数据库连接错误" << endl;
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
			cout << "数据库连接错误" << endl;
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

MYSQL_RES* SelectResult::getResult(){

	return this->result;
}

//InsertWood
InsertWood::InsertWood(){
	this->id = " ";
}

InsertWood::~InsertWood(){
}

//木材的长宽高，和木材照片的路径
string InsertWood::insertWood(MySQLC& mysqlc, float length, float width, float height, string id, string path){

	string result = "";
	string mysql = "";
	if (id.compare("") == 0) {
		mysql = "insert t_woods(length,width,height) values(" + to_string(length) + "," + to_string(width) + "," + to_string(height) + ");";
		//如果执行成功
		if (mysqlc.query(mysql) == 1){
			MYSQL* myconnect = mysqlc.getConnect();
			unsigned int insertId = mysql_insert_id(myconnect);
			this->id = to_string(insertId);
			result = this->id;
		}
	}else
	{
		mysql = "insert t_woods(id,length,width,height) values("+id+","+ to_string(length) + "," + to_string(width) + "," + to_string(height) + ");";
		//如果执行成功
		if (mysqlc.query(mysql) == 1){
			this->id = id;
		}
	}

	cout << "id:" << this->id << endl;

	return result;
}

bool InsertWood::insertFeature(MySQLC& mysqlc, string featureName, const vector<float>& features, const string& woodId ){
	bool result = 0;
	string mysql = "insert t_feature(wood_id,feature_name,feature_value) values(";
	int i = 0;
	string fs;
	ostringstream os;

	//判断id是不是有效
	if (woodId.compare("") != 0) {//是不是传来了woodid 
		os << woodId; 
	}else
	{
		if (id.compare("") != 0) { //上一条的id是不是合法的
			os << id;
		}
		else
		{
			return result;
		}
	}

	os<< ",'" << featureName << "','";
	for (i = 0; i < features.size()-1; i++){
		os << features.at(i) << " ";
	}
	os << features.at(i)<<"')";
	mysql = mysql + os.str();

	//如果执行成功
	if (mysqlc.query(mysql) == 1){
		result = 1;
	}
	else
	{
		id = "";
	}
	
	return result;
}


//isDefctive检测判断是不是有缺陷
bool InsertWood::updateWood(MySQLC& mysqlc, bool isDefctive, const string& woodId){

	return 1;
}




//readWood
ReadWood::ReadWood(){}
ReadWood::~ReadWood(){}



vector<WoodFeature> ReadWood::getAllWood(MySQLC& mysqlc,const string& featureName){
	vector<WoodFeature> features;

	string sql = "select wood_id,feature_value from t_feature where feature_name='"+featureName+"'";
	SelectResult sqlResult;

	mysqlc.select(sql, sqlResult);

	

	//unsigned int fieldcount = mysql_num_fields(sqlResult.getResult());

	//各行数据
	MYSQL_ROW row = NULL;
	row = mysql_fetch_row(sqlResult.getResult());
	while (NULL != row){
		WoodFeature f;
		
		//获得id
		if(NULL!=row[0]) f.id = row[0];

		//获得特征值
		if (NULL != row[1]){
			string s = row[1];
			
			//字符串流创建
			istringstream inputfloat(s);
			vector<float> featurefloat;//特征向量
			float buffer = 0;//缓冲的
			while (inputfloat >> buffer){
			
				featurefloat.push_back(buffer);
			}

			f.features=featurefloat ;
		}

		features.push_back(f);

		row = mysql_fetch_row(sqlResult.getResult());
	}


	return features;
}