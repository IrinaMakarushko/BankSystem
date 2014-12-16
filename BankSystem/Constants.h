#define operator_add_money 1
#define operator_withdraw_money 2
#define operator_exit 3
#define login_action 1
#define exit_action 2

char* insert = "INSERT INTO client (CLIENT_ID,FIRST_NAME,LAST_NAME) VALUES (?, ?, ?);";
char* insertAccount = "INSERT INTO ACCOUNT (CLIENT_ID) VALUES (?);";
char* selectAccountById = "SELECT * FROM account WHERE account_id=?;";
char* deleteAccountById = "DELETE FROM account WHERE account_id=?;";
char* selectClientById = "SELECT * FROM client WHERE client_id=?;";
char* selectAllClient = "SELECT * FROM client;";
char* deleteClientById = "DELETE FROM client WHERE client_id=?;";
char* deleteAccountByClientId = "DELETE FROM account WHERE client_id=?;";
char* insertDeletedUser = "INSERT INTO deleted_clients (FIRST_NAME, LAST_NAME) VALUES (?,?);";
char* selectAllDeleted = "SELECT * FROM deleted_clients;";
char* selectAccountInfById = "SELECT client_id, balance, current_transactions FROM account WHERE account_id = ?;";
char* insertDeletedAccount = "INSERT INTO deleted_account (ACCOUNT_ID, CLIENT_ID, BALANCE, TOTAL_TRANSACTIONS) VALUES (?,?,?,?);";
