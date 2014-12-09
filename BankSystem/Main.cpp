#include "sqlite3.h"
#include <iostream>
#include <stdio.h>
#include <string>
using namespace std;
#define create_client "create client"
#define delete_client "delete client"
#define add_account "add account"
#define delete_account "delete account"
#define add_account "add account"
#define add_money "add money"
#define withdraw_money "withdraw money"
#define view_accounts "view accounts"
#define login_string "Log In"
#define exit "Exit "

#define admin_create_client 1
#define admin_delete_client 2
#define admin_add_account 3
#define admin_delete_account 4
#define admin_exit 5

#define operator_add_money 1
#define operator_withdraw_money 2
#define operator_exit 3

#define login_action 1
#define exit_action 2

sqlite3 *conn;
sqlite3_stmt * stmt;
char* insert = "INSERT INTO client (CLIENT_ID,FIRST_NAME,LAST_NAME) VALUES (?, ?, ?);";
char* insertAccount = "INSERT INTO ACCOUNT (CLIENT_ID) VALUES (?);";
char* selectAccountById = "SELECT * FROM account WHERE account_id=?;";
char* deleteAccountById = "DELETE FROM account WHERE account_id=?;";
char* selectClientById = "SELECT * FROM client WHERE client_id=?;";
char* selectAllClient = "SELECT * FROM client;";
char* deleteClientById = "DELETE FROM client WHERE client_id=?;";
enum roleInSystem{UNKNOWN,ADMIN,OPERATOR};
roleInSystem roleIdentified;
int currentBalance;
int currentTransaction;
int totalTransaction;
int monthlyQuota;

void connection() {	
    char *zErrMsg = 0;
    int rc; 
    rc = sqlite3_open("12.db", &conn);
    if(rc){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(conn));
    }/*else{ 
      fprintf(stderr, "Opened database successfully\n");
    }  */ 
}
void createClient() {
	int rc;
	char* first_name = new char[50];
	char* last_name = new char[50];
	printf("Please input first name of client\nadmin->");
	std::cin>>first_name;
	printf("Please input last name of client\nadmin->");
	std::cin>>last_name;
	if(sqlite3_prepare(conn, insert, strlen (insert), &stmt, NULL) == SQLITE_OK) {
		if(sqlite3_bind_text(stmt, 2, first_name, -1, 0)== SQLITE_OK &&
			sqlite3_bind_text(stmt, 3, last_name, -1, 0)== SQLITE_OK) {
				rc = sqlite3_step(stmt);
				printf("client created\n");
		}
	}
	else
		printf("client didn't create, try again!\n");
	sqlite3_reset(stmt);
}
void deleteClient() {
	int rc;
	char* client_id = new char[10];
	printf("Please input client id!\nadmin->");
	std::cin>>client_id;
	if(sqlite3_prepare(conn, selectClientById, strlen (selectClientById), &stmt, NULL) == SQLITE_OK) 
		if(sqlite3_bind_text(stmt, 1, client_id, -1, 0)== SQLITE_OK) {
			int cols = sqlite3_column_count(stmt);
			rc = sqlite3_step(stmt);//executing the statement
			if(rc == SQLITE_ROW) {
				sqlite3_reset(stmt);
				if((rc=sqlite3_prepare(conn, deleteClientById, strlen (deleteClientById), &stmt, NULL)) == SQLITE_OK) 
					if(sqlite3_bind_text(stmt, 1, client_id, -1, 0)== SQLITE_OK) 
						if(sqlite3_step(stmt)==SQLITE_DONE)
							printf("client deleted!\n");
			}	
			else
				printf("client didn't deleted, try again!\n");
		}
}
void addAccount()
{
	int rc;
	int client_id;
	
	printf("Please input client id:\nadmin->");
	scanf("%d", &client_id);
//	std::cin >> client_id;
	char query[50];
	_itoa(client_id, query, 10);

	if (sqlite3_prepare(conn, selectClientById, strlen(selectClientById), &stmt, NULL) == SQLITE_OK) {
		if (sqlite3_bind_text(stmt, 1, query, -1, 0) == SQLITE_OK)  {
			rc = sqlite3_step(stmt);
			
		}
	}
	else{
		printf("input error, try again!\n");
		printf("\n");
	}

	if (rc != 100)
	{
		printf("no such client\n");
		printf("\n");
		return;
	}
	if (sqlite3_prepare(conn, insertAccount, strlen(insertAccount), &stmt, NULL) == SQLITE_OK) {
		if (sqlite3_bind_text(stmt, 1, query, -1, 0) == SQLITE_OK)  {
			rc = sqlite3_step(stmt);
			printf("account added\n");
			printf("\n");
		}
	}
	else{
		printf("input error, try again!\n");
		printf("\n");
	}
	sqlite3_reset(stmt);
}

void deleteAccount()
{
	int rc;
	int account_id;

	printf("Please input account id:\nadmin->");
	scanf("%d", & account_id);
//	std::cin >> account_id;
	char query[50];
	_itoa(account_id, query, 10);

	if (sqlite3_prepare(conn, selectAccountById, strlen(selectAccountById), &stmt, NULL) == SQLITE_OK) {
		if (sqlite3_bind_text(stmt, 1, query, -1, 0) == SQLITE_OK)  {
			rc = sqlite3_step(stmt);

		}
	}
	else {
		printf("input error, try again!\n");
		printf("\n");
	}

	if (rc != 100)
	{
		printf("no such account\n");
		printf("\n");
		return;
	}
	if (sqlite3_prepare(conn, deleteAccountById, strlen(deleteAccountById), &stmt, NULL) == SQLITE_OK) {
		if (sqlite3_bind_text(stmt, 1, query, -1, 0) == SQLITE_OK)  {
			rc = sqlite3_step(stmt);
			printf("account deleted\n");
			printf("\n");
		}
	}
	else{
		printf("input error, try again!\n");
		printf("\n");
	}
	sqlite3_reset(stmt);

}

int addMoney(int & balance1, int totalTransaction1, int currentBalance1, int & currentTransaction1,int monthlyQuota1 ){
	
	int fee = (currentTransaction1 + 1 - totalTransaction1) * monthlyQuota1;
	if(currentTransaction1 + 1 > totalTransaction1 && balance1 + currentBalance1 - fee < 0){
		printf("It is not possible to commit the transaction\n");
		return -1;
	}
	currentTransaction1++;
	if(currentTransaction1 > totalTransaction1){
		balance1 = balance1 + currentBalance1 - fee;
	} else{
		balance1 = balance1 + currentBalance1;
	}
	return 0;
}
int withdrawMoney(int & balance1, int totalTransaction1, int currentBalance1, int & currentTransaction1,int monthlyQuota1 ){
	
	int fee = (currentTransaction1 + 1 - totalTransaction1) * monthlyQuota1;
	if(currentTransaction1 + 1 > totalTransaction1 && currentBalance1 - balance1 - fee < 0){
		printf("It is not possible to commit the transaction\n");
		return -1;
	}
	currentTransaction1++;
	if(currentTransaction1 > totalTransaction1){
		balance1 = currentBalance1 - balance1 - fee;
	} else{
		balance1 = currentBalance1 - balance1;
	}
	return 0;
}
void viewAccounts(){}
void adminActions(){
	connection();
	bool isExit=false;
	int numberOfOperation=0;
	while(!isExit){
		printf( "Operations:\n" );
		printf("%d %s\n",admin_create_client, create_client );
		printf("%d %s\n",admin_delete_client, delete_client);
		printf("%d %s\n",admin_add_account, add_account );
		printf("%d %s\n",admin_delete_account, delete_account);
		printf("%d %s\n",admin_exit, exit);
		printf("Please, enter a code of operation.\n");
		scanf("%d",&numberOfOperation);
		
		switch(numberOfOperation){
			case admin_create_client:
				createClient();
			break;
			case admin_delete_client:
				deleteClient();
			break;
			case admin_add_account:
				addAccount();
			break;
			case admin_exit:
				printf("Good buy!\n");
				roleIdentified=UNKNOWN;
				isExit=true;
			break;
			default:
				cout << "Unknown operation.Please, try again." << endl;
			break;
		}
	}
}
static int showList(void *data, int argc, char **argv, char **azColName){
	for(int i=0; i<argc; i++){
	printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}
static int getCurrentBalance(void *data, int argc, char **argv, char **azColName){
	currentBalance=atoi(argv[0]);
	currentTransaction=atoi(argv[1]);
	return 0;
}
static int setSystemConfig(void *data, int argc, char **argv, char **azColName){
	totalTransaction=atoi(argv[0]);
	monthlyQuota=atoi(argv[1]);
	return 0;
}

void operatorActions(){
	char *zErrMsg;
	const char* data;
	int rc;
	int idClient=0;
	int idAccount=0;
	zErrMsg=0;
	data = "Callback function called\n";
	rc = sqlite3_open("12.db",&conn);
	if(rc){
		printf("Error\n");
		sqlite3_close(conn);
		return;
	}
	//system data
	rc = sqlite3_exec(conn,"select total_transactions, monthly_quota from system_config",setSystemConfig, (void*)data, &zErrMsg);
	if(rc){
		printf("Error!\n");
		sqlite3_close(conn);
		return;
	}
	char bufferSelect[500];
	sprintf(bufferSelect,"select client_id,first_name, last_name from client");
	rc = sqlite3_exec(conn,bufferSelect,showList, (void*)data, &zErrMsg);
	if(rc){
		printf("Error!\n");
		sqlite3_close(conn);
		return;
	}
	printf("Enter id of client\n");
	scanf("%d",&idClient);
	char bufferAccount[500];
	sprintf(bufferAccount,"select account_id,balance, current_transactions from account where client_id='%d'",idClient);
	rc = sqlite3_exec(conn,bufferAccount,showList, (void*)data, &zErrMsg);
	if(rc){
		printf("Error!\n");
		sqlite3_close(conn);
		return;
	}
	printf("Enter id of client`s account\n");
	scanf("%d",&idAccount);
	char bufferGetBalance[500];
	sprintf(bufferGetBalance,"select balance, current_transactions from account where account_id='%d'",idAccount);
	rc = sqlite3_exec(conn,bufferGetBalance,getCurrentBalance, (void*)data, &zErrMsg);
	if(rc){
		printf("Error!\n");
		sqlite3_close(conn);
		return;
	}
	int numberOfOperation=0;
	bool isExit=false;
	printf("Operations:\n");
	printf("%d %s\n",operator_add_money,add_money);
	printf("%d %s\n",operator_withdraw_money ,withdraw_money);
	printf("%d %s\n",operator_exit, exit);
	scanf("%d",&numberOfOperation);
	int result = 0;
	int balance = 0;
	printf("Enter sum: ");
	scanf("%d", &balance);
	
	switch(numberOfOperation){
		case operator_add_money:
		result = addMoney(balance, totalTransaction, currentBalance, currentTransaction, monthlyQuota);
		break;
		case operator_withdraw_money:
		result = withdrawMoney(balance, totalTransaction, currentBalance, currentTransaction, monthlyQuota);
		break;
		case operator_exit:
		printf("Good bye!\n");
		roleIdentified=UNKNOWN;
		isExit=true;
		break;
		default:
		printf("Unknown operation.Please, try again.\n");
		break;
	}
	//------------
	if(!isExit && result == 0){
		char bufferUpdate[500];
		sprintf(bufferUpdate,"update account set balance='%d', current_transactions='%d' where account_id='%d'",balance, currentTransaction, idAccount);
		sqlite3_exec(conn, "BEGIN TRANSACTION;", NULL, NULL, NULL);
		int rc = sqlite3_exec(conn,bufferUpdate,NULL,NULL,NULL);
		sqlite3_exec(conn, "END TRANSACTION;", NULL, NULL, NULL);
		if(rc){
		printf("Error");
		}else{
			printf("Operation is executed successfully!\n");
		}
	}
	sqlite3_close(conn);
//--------------------
}

void chooseOperations(){
	switch(roleIdentified){
			case(ADMIN):
				printf("You are admin.\n");
				adminActions();
				break;
			case(OPERATOR):
				printf("You are operator.\n");
				operatorActions();
				break;
			case(UNKNOWN):
				printf("ERROR!You are not registred in system!\n");
				break;
		}
}

static int authorization(void *data, int argc, char **argv, char **azColName){
	string role=argv[0];
	if(role=="admin")
		roleIdentified=ADMIN;
	if(role=="operator")
		roleIdentified=OPERATOR;
	return 0;
}

void authentication(){
	connection();
	char *zErrMsg = 0;
	const char* data = "Callback function called\n";
   	
	char login[20];
	char password[20];
	printf("Enter login: ");
	scanf("%s", login);
	printf("Enter password: ");
	scanf("%s", password);
	
	char buffer [100];
	sprintf(buffer,"select role from bank_personal where login='%s' and password='%s'",login, password);

	int rc = sqlite3_exec(conn,buffer,authorization, (void*)data, &zErrMsg);
	if(rc){
		printf("Error");
		return ;
	}
	sqlite3_close(conn);
	chooseOperations();
}



void main(){
	bool isExit=false;
	int numberOfOperation;
	while(!isExit){
		printf("Welcome to banking system!\nActions:\n");
		printf("%d %s\n",login_action, login_string);
		printf("%d %s\n",exit_action,exit);
		printf("Please, enter a number of action.\n") ;
		scanf("%d",&numberOfOperation);

		switch(numberOfOperation){
		case login_action:
			authentication();
			break;
		case exit_action:
			isExit=true;
			printf("Good buy!\n");
			break;
		default:
			printf("Unknown operation.Please, try again.\n");
			break;
		}
	}
	system("pause");
}
