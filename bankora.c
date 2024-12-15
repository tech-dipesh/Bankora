#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ATTEMPTS 10
#define MAX_USERS 100
#define FILENAME "accounts.dat"

typedef struct {
    int accountNumber;
    char username[20];
    char password[20];
    float balance;
    char transactionHistory[10][100];
    int transactionCount;
} Account;

Account users[MAX_USERS];
int currentUserIndex = -1;
int loginAttempts = 0;
int userCount = 0;

void loadAccounts() {
    FILE *file = fopen(FILENAME, "rb");
    if (file != NULL) {
        fread(&userCount, sizeof(int), 1, file);
        fread(users, sizeof(Account), userCount, file);
        fclose(file);
    }
}

// remember that this is for save password
void saveAccounts() {
    FILE *file = fopen(FILENAME, "wb");
    if (file != NULL) {
        fwrite(&userCount, sizeof(int), 1, file);
        fwrite(users, sizeof(Account), userCount, file);
        fclose(file);
    }
}

//remember that this is for create a new account
void createAccount() {
    Account newUser;
    newUser.accountNumber = 1000 + userCount + 1;
    printf("Enter Username: ");
    scanf("%s", newUser.username);
    printf("Enter Password: ");
    scanf("%s", newUser.password);
    newUser.balance = 0.0;
    newUser.transactionCount = 0;
//remember that this is for the save account
    users[userCount] = newUser;
    userCount++;
    saveAccounts();
    printf("Account created successfully! Your Account Number is %d\n", newUser.accountNumber);
}
//remember that this is for saying transaction history
void printTransactionHistory(Account *user) {
    printf("\nTransaction History:\n");
    for (int i = 0; i < user->transactionCount; i++) {
        printf("%s\n", user->transactionHistory[i]);
    }
}
//this is for transcition
void logTransaction(Account *user, const char *transaction) {
    if (user->transactionCount < 10) {
        strcpy(user->transactionHistory[user->transactionCount], transaction);
        user->transactionCount++;
    }
}
//this is for add money
void deposit(Account *user, float amount) {
    user->balance += amount;
    printf("Deposited %.2f successfully. New Balance: %.2f\n", amount, user->balance);
    
    char transaction[100];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(transaction, sizeof(transaction), "%Y-%m-%d %H:%M:%S - Deposit: ", tm_info);
    char amountStr[20];
    sprintf(amountStr, "%.2f", amount);
    strcat(transaction, amountStr);
    
    logTransaction(user, transaction);
}
//this is for withdraw money
void withdraw(Account *user, float amount) {
    if (amount > user->balance) {
        printf("Error: Insufficient balance.\n");
        return;
    }
    
    user->balance -= amount;
    printf("Withdrawn %.2f successfully. New Balance: %.2f\n", amount, user->balance);
    
    char transaction[100];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(transaction, sizeof(transaction), "%Y-%m-%d %H:%M:%S - Withdrawal: ", tm_info);
    char amountStr[20];
    sprintf(amountStr, "%.2f", amount);
    strcat(transaction, amountStr);
    
    logTransaction(user, transaction);
}
//this is for transfer money
void transfer(Account *user, Account *recipient, float amount) {
    if (amount > user->balance) {
        printf("Error: Insufficient balance.\n");
        return;
    }
    
    user->balance -= amount;
    recipient->balance += amount;
    printf("Transferred %.2f to Account %d successfully.\n", amount, recipient->accountNumber);
    
    char transaction[100];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(transaction, sizeof(transaction), "%Y-%m-%d %H:%M:%S - Transfer: ", tm_info);
    char amountStr[20];
    sprintf(amountStr, "%.2f", amount);
    strcat(transaction, amountStr);
    
    logTransaction(user, transaction);
    
    sprintf(transaction, "%s transferred %.2f", user->username, amount);
    logTransaction(recipient, transaction);
}
//this is for check balance
void checkBalance(Account *user) {
    printf("\nAvailable Balance: %.2f\n", user->balance);
    printf("Last Transaction: %s\n", user->transactionCount > 0 ? user->transactionHistory[user->transactionCount - 1] : "No transactions yet.");
}
//this is for user authontication
int authenticate() {
    char username[20], password[20];
    
    while (loginAttempts < MAX_ATTEMPTS) {
        printf("Enter Account Number: ");
        int accountNumber;
        scanf("%d", &accountNumber);
        
        printf("Enter Password: ");
        scanf("%s", password);
        
        for (int i = 0; i < userCount; i++) {
            if (users[i].accountNumber == accountNumber && strcmp(users[i].password, password) == 0) {
                currentUserIndex = i;
                return 1;
            }
        }
        
        loginAttempts++;
        printf("Invalid login. You have %d attempts remaining.\n", MAX_ATTEMPTS - loginAttempts);
    }
    
    return 0;
}
//this is for showing main menu.
void displayMenu() {
    printf("\nATM Menu:\n");
    printf("1. Check Balance\n");
    printf("2. Deposit Money\n");
    printf("3. Withdraw Money\n");
    printf("4. Transfer Money\n");
    printf("5. View Transaction History\n");
    printf("6. Exit\n");
}

int main() {
    loadAccounts();

    int choice;
    
    printf("Welcome to the ATM system!\n");
    printf("1. Login\n");
    printf("2. Create New Account\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    
    if (choice == 2) {
        createAccount();
        return 0;
    }
    
    if (!authenticate()) {
        printf("Too many failed login attempts. Exiting...\n");
        return 0;
    }
    
    printf("Login successful!\n");
    
    Account *currentUser = &users[currentUserIndex];
    
    while (1) {
        displayMenu();
        
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        float amount;
        int recipientAccount;
        Account *recipient;
        
        switch (choice) {
            case 1:
                checkBalance(currentUser);
                break;
            case 2:
                printf("Enter deposit amount: ");
                scanf("%f", &amount);
                deposit(currentUser, amount);
                break;
            case 3:
                printf("Enter withdrawal amount: ");
                scanf("%f", &amount);
                withdraw(currentUser, amount);
                break;
            case 4:
                printf("Enter recipient account number: ");
                scanf("%d", &recipientAccount);
                
                for (int i = 0; i < userCount; i++) {
                    if (users[i].accountNumber == recipientAccount) {
                        recipient = &users[i];
                        break;
                    }
                }
                
                if (recipient == NULL) {
                    printf("Error: Recipient account not found.\n");
                    break;
                }
                //this is for data save that details you add
                printf("Enter transfer amount: ");
                scanf("%f", &amount);
                transfer(currentUser, recipient, amount);
                break;
            case 5:
                printTransactionHistory(currentUser);
                break;
            case 6:
                printf("Thank you for using the ATM. Goodbye!\n");
                saveAccounts();
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
    
    return 0;
}
