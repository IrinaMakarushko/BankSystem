#include "sqlite3.h"
#include <iostream>
#include <stdio.h>
#include <string>
using namespace std;

void createClient();
void deleteClient();
void addAccount();
void deleteAccount();
void select_all_deleted_client();
void select_all_client();
void searchClient();

int addMoney();
int withdrawMoney();

//#define add_money "add money"
//#define withdraw_money "withdraw money"
#define view_accounts "view accounts"
#define login_string "Log In"
#define exit "Exit "

int admin_actions_count = 7;
void(*admin_actions[])() =
{ 
	createClient,
	deleteClient,
	addAccount,
	deleteAccount,
	select_all_client,
	select_all_deleted_client,
	searchClient,
};
char admin_actions_names[][128] =
{
	"create client",
	"delete client",
	"add account",
	"delete account",
	"watch list of client",
	"watch list of deleted client",
	"search client",
};

int operator_actions_count = 2;
int(*operator_actions[])() =
{
	addMoney,
	withdrawMoney,
};
char operator_actions_names[][128] =
{
	"add money",
	"withdraw money"
};

/*#define operator_add_money 1
#define operator_withdraw_money 2
#define operator_exit 3*/
#define login_action 1
#define exit_action 2
sqlite3 *conn;
sqlite3 *conn_del;
sqlite3_stmt * stmt;
char* insert = "INSERT INTO client (CLIENT_ID,FIRST_NAME,LAST_NAME) VALUES (?, ?, ?);";
char* insertAccount = "INSERT INTO ACCOUNT (CLIENT_ID) VALUES (?);";
char* selectAccountById = "SELECT * FROM account WHERE account_id=?;";
char* deleteAccountById = "DELETE FROM account WHERE account_id=?;";
char* selectClientById = "SELECT * FROM client WHERE client_id=?;";
char* selectAllClient = "SELECT * FROM client;";
char* deleteClientById = "DELETE FROM client WHERE client_id=?;";
char* deleteAccountByClientId = "DELETE FROM account WHERE client_id=?;";
char* insertDeletedUser = "INSERT INTO deleted_user (FIRST_NAME, LAST_NAME) VALUES (?,?);";
char* selectAllDeleted = "SELECT * FROM deleted_user;";
char* selectAccountInfById = "SELECT client_id, balance, current_transactions FROM account WHERE account_id = ?;";
char* insertDeletedAccount = "INSERT INTO deleted_account (ACCOUNT_ID, CLIENT_ID, BALANCE, TOTAL_TRANSACTIONS) VALUES (?,?,?,?);";
enum roleInSystem{UNKNOWN,ADMIN,OPERATOR} roleIdentified;
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
	 } */
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
bool insertDeletedClient (char* firstName, char* lastName) {
	if(sqlite3_prepare(conn, insertDeletedUser, strlen (insertDeletedUser), &stmt, NULL) == SQLITE_OK) 
		if(sqlite3_bind_text(stmt, 1, firstName, -1, 0)== SQLITE_OK &&
			sqlite3_bind_text(stmt, 2, lastName, -1, 0)== SQLITE_OK) 
				if(sqlite3_step(stmt)==SQLITE_DONE)
					return true;
	return false;
}
bool deleteRelationship(char* client_id) { 
	int rc;
	if((rc=sqlite3_prepare(conn, deleteClientById, strlen (deleteClientById), &stmt, NULL)) == SQLITE_OK)
		if(sqlite3_bind_text(stmt, 1, client_id, -1, 0)== SQLITE_OK)
			if(sqlite3_step(stmt)==SQLITE_DONE)
				if((rc=sqlite3_prepare(conn, deleteAccountByClientId, strlen (deleteAccountByClientId), &stmt, NULL)) == SQLITE_OK)
					if(sqlite3_bind_text(stmt, 1, client_id, -1, 0)== SQLITE_OK)
						if(sqlite3_step(stmt)==SQLITE_DONE)
							return true;
	return false;
}
void deleteClient() {
	int rc;
	char* client_id = new char[10];
	std::cout<<"Please input client id!\nadmin->";
	std::cin>>client_id;
	char* firstN = new char[50];
	char* lastN = new char[50];
	if(sqlite3_prepare(conn, selectClientById, strlen (selectClientById), &stmt, NULL) == SQLITE_OK)
		if(sqlite3_bind_text(stmt, 1, client_id, -1, 0)== SQLITE_OK) {
			int cols = sqlite3_column_count(stmt);
			rc = sqlite3_step(stmt);//executing the statement
			firstN = (char*)sqlite3_column_text(stmt, 1);
			lastN = (char*)sqlite3_column_text(stmt, 2);
			if(rc == SQLITE_ROW) {
				if(deleteRelationship(client_id))
					if(insertDeletedClient(firstN,lastN))
						printf("client deleted!\n");
					else printf("client didn't deleted, try again!\n");
				else printf("client didn't deleted, try again!\n");
			}
			else
				printf("client didn't deleted, try again!\n");
		}
}
void select_all_client() {
	int rc;
	if(sqlite3_prepare(conn, selectAllClient, strlen (selectAllClient), &stmt, NULL) == SQLITE_OK) {
		rc = sqlite3_step(stmt);
		printf("CLIENT_ID   First_Name   Last_Name\n");
		while(rc == SQLITE_ROW) {
			printf("   %s      ",(char*)sqlite3_column_text(stmt, 0));
			printf("   %s      ",(char*)sqlite3_column_text(stmt, 1));
			printf("   %s      ",(char*)sqlite3_column_text(stmt, 2));
			rc = sqlite3_step(stmt);
			printf("\n");
		}	
	}
}
void select_all_deleted_client() {
	int rc;
	if(sqlite3_prepare(conn, selectAllDeleted, strlen (selectAllDeleted), &stmt, NULL) == SQLITE_OK) {
		rc = sqlite3_step(stmt);
		printf("CLIENT_ID  First_Name  Last_Name\n");
		while(rc == SQLITE_ROW) {
			printf("   %s      ",(char*)sqlite3_column_text(stmt, 0));
			printf("   %s      ",(char*)sqlite3_column_text(stmt, 1));
			printf("   %s      ",(char*)sqlite3_column_text(stmt, 2));
			rc = sqlite3_step(stmt);
			printf("\n");
		}	
	}
}

void searchClient()
{
	int rc;
	char* first_name = new char[50];
	char* last_name = new char[50];
	char* buffer = new char[50];
	printf("Please input first name of client\nadmin->");
	std::cin >> buffer;
	sprintf(first_name, "%%%s%%", buffer);
	printf("Please input last name of client\nadmin->");
	std::cin >> buffer;
	sprintf(last_name, "%%%s%%", buffer);
	char *search_and = "SELECT * FROM client WHERE first_name LIKE ? AND last_name LIKE ?;";
	char *search_xor = "SELECT * FROM client WHERE (first_name LIKE ? OR last_name LIKE ?) AND NOT (first_name LIKE ? AND last_name LIKE ?);";

	bool flag = false;

	if (sqlite3_prepare(conn, search_and, strlen(search_and), &stmt, NULL) == SQLITE_OK) {
		if (sqlite3_bind_text(stmt, 1, first_name, -1, 0) == SQLITE_OK &&
			sqlite3_bind_text(stmt, 2, last_name, -1, 0) == SQLITE_OK) {
			while (sqlite3_step(stmt) != SQLITE_DONE)
			{
				printf("%d, %s, %s\n", sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1), sqlite3_column_text(stmt, 2));
				flag = true;
			}
		}
	}
	else
		printf("something went wrong!\n");

	sqlite3_reset(stmt);

	if (sqlite3_prepare(conn, search_xor, strlen(search_xor), &stmt, NULL) == SQLITE_OK) {
		if (sqlite3_bind_text(stmt, 1, first_name, -1, 0) == SQLITE_OK &&
			sqlite3_bind_text(stmt, 2, last_name, -1, 0) == SQLITE_OK &&
			sqlite3_bind_text(stmt, 3, first_name, -1, 0) == SQLITE_OK &&
			sqlite3_bind_text(stmt, 4, last_name, -1, 0) == SQLITE_OK) {
			while (sqlite3_step(stmt) != SQLITE_DONE)
			{
				printf("%d, %s, %s\n", sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1), sqlite3_column_text(stmt, 2));
				flag = true;
			}
		}
	}
	else
		printf("something went wrong!\n");

	sqlite3_reset(stmt);

	if (!flag){
		printf("Nothing found.\n");
	}
}


void addAccount()
{
	int rc;
	int client_id;

	printf("Please input client id:\nadmin-> ");
	scanf("%d", &client_id);
	
	char query[50];
	_itoa(client_id, query, 10);

	if (sqlite3_prepare(conn, selectClientById, strlen(selectClientById), &stmt, NULL) == SQLITE_OK) {
		if (sqlite3_bind_text(stmt, 1, query, -1, 0) == SQLITE_OK) {
			rc = sqlite3_step(stmt);
		}
	}
	else{
		printf("input error, try again!\n\n");
		}
	if (rc != SQLITE_ROW)
	{
		printf("no such client\n\n");
		return;
	}
	if (sqlite3_prepare(conn, insertAccount, strlen(insertAccount), &stmt, NULL) == SQLITE_OK) {
		if (sqlite3_bind_text(stmt, 1, query, -1, 0) == SQLITE_OK) {
			
			rc = sqlite3_step(stmt);
			printf("account added\n\n");
		}
	}
	else{
		printf("input error, try again!\n\n");
		}
	sqlite3_reset(stmt);
}

void moveToDeletedAccount(char * account_id)
{
	int rc;
	char clientId[50];
	char balance[50];
	char currentTransactions[50];
	

	if (sqlite3_prepare(conn, selectAccountInfById, strlen(selectAccountInfById), &stmt, NULL) == SQLITE_OK) {
		if (sqlite3_bind_text(stmt, 1, account_id, -1, 0) == SQLITE_OK) {
			rc = sqlite3_step(stmt);

			if (rc == SQLITE_ROW)
			{
				strcpy(clientId, (char*)sqlite3_column_text(stmt, 0));
				strcpy(balance, (char*)sqlite3_column_text(stmt, 1));
				strcpy(currentTransactions, (char*)sqlite3_column_text(stmt, 2));
				
				if (sqlite3_prepare(conn, insertDeletedAccount, strlen(insertDeletedAccount), &stmt, NULL) == SQLITE_OK)
				if (sqlite3_bind_text(stmt, 1, account_id, -1, 0) == SQLITE_OK && sqlite3_bind_text(stmt, 2, clientId, -1, 0) == SQLITE_OK 
					&& sqlite3_bind_text(stmt, 3, balance, -1, 0) == SQLITE_OK && sqlite3_bind_text(stmt, 4, currentTransactions, -1, 0) == SQLITE_OK)
						rc = sqlite3_step(stmt);
				
			}
			
					}
	}
	else {
		printf("input error, try again!\n\n");

	}
}

void deleteAccount()
{
	int rc;
	int account_id;

	printf("Please input account id:\nadmin-> ");
	scanf("%d", & account_id);

	char query[50];
	_itoa(account_id, query, 10);

	moveToDeletedAccount(query);

	if (sqlite3_prepare(conn, selectAccountById, strlen(selectAccountById), &stmt, NULL) == SQLITE_OK) {
		if (sqlite3_bind_text(stmt, 1, query, -1, 0) == SQLITE_OK) {
			rc = sqlite3_step(stmt);
		}
	}
	else {
		printf("input error, try again!\n\n");
		
	}
	if (rc != SQLITE_ROW)
	{
		printf("no such account\n\n");
		
		return;
	}
	if (sqlite3_prepare(conn, deleteAccountById, strlen(deleteAccountById), &stmt, NULL) == SQLITE_OK) {
		if (sqlite3_bind_text(stmt, 1, query, -1, 0) == SQLITE_OK) {
			rc = sqlite3_step(stmt);
			printf("account deleted\n\n");
			
		}
	}
	else{
		printf("input error, try again!\n\n");
		
	}
	sqlite3_reset(stmt);
}
int balance = 0;
int addMoney(){
	
	int fee = (currentTransaction + 1 - totalTransaction) * monthlyQuota;
	if(currentTransaction + 1 > totalTransaction && balance + currentBalance - fee < 0){
		printf("It is not possible to commit the transaction\n");
		return -1;
	}
	currentTransaction++;
	if(currentTransaction > totalTransaction){
		balance = balance + currentBalance - fee;
	} else{
		balance = balance + currentBalance;
	}
	return 0;
}
int withdrawMoney(){
	
	int fee = (currentTransaction + 1 - totalTransaction) * monthlyQuota;
	if(currentTransaction + 1 > totalTransaction && currentBalance - balance - fee < 0){
		printf("It is not possible to commit the transaction\n");
		return -1;
	}
	currentTransaction++;
	if(currentTransaction > totalTransaction){
		balance = currentBalance - balance - fee;
	} else{
		balance = currentBalance - balance;
	}
	return 0;
}
void viewAccounts(){}
void adminActions(){
	connection();
	bool isExit = false;
	int numberOfOperation = 0;
	while (!isExit){
		printf("Operations:\n");
		for (short i = 0; i < admin_actions_count; ++i){
			printf("%d. %s\n", i, admin_actions_names[i]);
		}
		printf("%d. %s\n", admin_actions_count, exit);
		scanf("%d", &numberOfOperation);
		if (numberOfOperation == admin_actions_count){
			printf("Good buy!\n");
			roleIdentified = UNKNOWN;
			isExit = true;
			break;
		}
		if (numberOfOperation >= 0 && numberOfOperation < admin_actions_count)
		{
			admin_actions[numberOfOperation]();
		}
		else
		{
			printf("Unknown operation.Please, try again.\n");
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

	for (short i = 0; i < operator_actions_count; ++i){
		printf("%d. %s\n", i, operator_actions_names[i]);
	}
	printf("%d. %s\n", operator_actions_count, exit);
	scanf("%d", &numberOfOperation);
	if (numberOfOperation == operator_actions_count){
		printf("Good buy!\n");
		roleIdentified = UNKNOWN;
		isExit = true;
	}

	int result = 0;

	if (numberOfOperation >= 0 && numberOfOperation < operator_actions_count)
	{
		result = operator_actions[numberOfOperation]();
	}
	else
	{
		printf("Unknown operation.Please, try again.\n");
	}

	/*printf("%d %s\n",operator_add_money,add_money);
	printf("%d %s\n",operator_withdraw_money ,withdraw_money);
	printf("%d %s\n",operator_exit, exit);
	scanf("%d",&numberOfOperation);
	int result = 0;
	balance=0;
	printf("Enter sum: ");
	scanf("%d", &balance);

	switch(numberOfOperation){
	case operator_add_money:
		result = addMoney();
		break;
	case operator_withdraw_money:
		result = withdrawMoney();
		break;
	case operator_exit:
		printf("Good bye!\n");
		roleIdentified=UNKNOWN;
		isExit=true;
		break;
	default:
		printf("Unknown operation.Please, try again.\n");
		break;
	}*/

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
