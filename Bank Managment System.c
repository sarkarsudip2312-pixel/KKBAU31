#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 100
#define DAILY_LIMIT 50000
// ==============================
// ACCOUNT STRUCTURE
// ==============================
typedef struct
{
    int accNo;
    char name[50];
    int pin;
    float balance;
    char lastLogin[50];

    float todayWithdraw;
    char withdrawDate[20];

} Account;

Account acc[MAX];
int count = 0;

// ==============================
// LOAD DATA
// ==============================
void loadAccounts()
{
    FILE *fp = fopen("accounts.dat", "rb");

    if (fp != NULL)
    {
        count = fread(acc, sizeof(Account), MAX, fp);
        fclose(fp);
    }
}

// ==============================
// SAVE DATA
// ==============================
void saveAccounts()
{
    FILE *fp = fopen("accounts.dat", "wb");

    if (fp == NULL)
    {
        printf("File Error!\n");
        return;
    }

    fwrite(acc, sizeof(Account), count, fp);
    fclose(fp);
}

// ==============================
// LAST LOGIN UPDATE
// ==============================
void updateLastLogin(int index)
{
    time_t t;
    time(&t);

    strcpy(acc[index].lastLogin, ctime(&t));

    saveAccounts();
}

// ==============================
// TRANSACTION HISTORY
// ==============================
void addTransaction(int accNo, char action[])
{
    FILE *fp = fopen("history.txt", "a");

    if(fp == NULL)
        return;

    time_t t;
    time(&t);

    fprintf(fp,
            "Account:%d | %s | %s",
            accNo,
            action,
            ctime(&t));

    fclose(fp);
}
void printReceipt(int accNo, char type[], float amount, float balance)
{
    time_t t;
    time(&t);

    printf("\n=====================\n");
    printf("      RECEIPT\n");
    printf("=====================\n");
    printf("Account No : %d\n", accNo);
    printf("Transaction: %s\n", type);
    printf("Amount     : %.2f\n", amount);
    printf("Balance    : %.2f\n", balance);
    printf("Date & Time: %s", ctime(&t));
    printf("=====================\n");
}
// ==============================
// CREATE ACCOUNT
// ==============================
void createAccount()
{
    if (count >= MAX)
    {
        printf("Account Limit Reached!\n");
        return;
    }

    printf("\n===== CREATE ACCOUNT =====\n");

    acc[count].accNo = 1001 + count;

    printf("Enter Name: ");
    scanf(" %[^\n]", acc[count].name);

    // PIN validation
    do
    {
        printf("Set 4-Digit PIN: ");
        scanf("%d", &acc[count].pin);

        if (acc[count].pin < 1000 || acc[count].pin > 9999)
        {
            printf("Invalid PIN! Must be 4 digits.\n");
        }

    } while (acc[count].pin < 1000 || acc[count].pin > 9999);

    printf("Initial Deposit: ");
    scanf("%f", &acc[count].balance);

    strcpy(acc[count].lastLogin, "Never Logged In");
acc[count].todayWithdraw = 0;
strcpy(acc[count].withdrawDate, "");

    printf("\nAccount Created Successfully!\n");
    printf("Account Number: %d\n", acc[count].accNo);

    count++;
    saveAccounts();
}

// ==============================
// DEPOSIT
// ==============================
void deposit(int i)
{
    float amount;

    printf("Enter Deposit Amount: ");
    scanf("%f", &amount);

    if (amount <= 0)
    {
        printf("Invalid Amount!\n");
        return;
    }

    acc[i].balance += amount;
addTransaction(acc[i].accNo, "Deposit");

printf("Deposit Successful!\n");
printf("Current Balance: %.2f\n", acc[i].balance);

printReceipt(
    acc[i].accNo,
    "Deposit",
    amount,
    acc[i].balance
);

saveAccounts();
}

// ==============================
// WITHDRAW
// ==============================
void withdraw(int i)
{
    float amount;

    time_t t;
    time(&t);

    struct tm *tm_info = localtime(&t);

    char today[20];
    strftime(today, sizeof(today), "%Y-%m-%d", tm_info);

    // নতুন দিন হলে reset
    if(strcmp(acc[i].withdrawDate, today) != 0)
    {
        acc[i].todayWithdraw = 0;
        strcpy(acc[i].withdrawDate, today);
    }

    printf("Enter Withdraw Amount: ");
    scanf("%f", &amount);

    if(amount <= 0)
    {
        printf("Invalid Amount!\n");
        return;
    }

    if(amount > acc[i].balance)
    {
        printf("Insufficient Balance!\n");
        return;
    }

    if(acc[i].todayWithdraw + amount > DAILY_LIMIT)
    {
        printf("Daily Withdrawal Limit Exceeded!\n");
        printf("Today's Remaining Limit: %.2f\n",
               DAILY_LIMIT - acc[i].todayWithdraw);
        return;
    }

    acc[i].balance -= amount;
    acc[i].todayWithdraw += amount;

    addTransaction(acc[i].accNo, "Withdraw");

printf("Withdrawal Successful!\n");
printf("Current Balance: %.2f\n", acc[i].balance);

printReceipt(
    acc[i].accNo,
    "Withdraw",
    amount,
    acc[i].balance
);

saveAccounts();
}

// ==============================
// BALANCE CHECK
// ==============================
void checkBalance(int i)
{
    printf("\n===== ACCOUNT DETAILS =====\n");
    printf("Name       : %s\n", acc[i].name);
    printf("Account No : %d\n", acc[i].accNo);
    printf("Balance    : %.2f\n", acc[i].balance);
    printf("Last Login : %s", acc[i].lastLogin);
}

// ==============================
// CHANGE PIN
// ==============================
void changePIN(int i)
{
    int oldPin, newPin;

    printf("Enter Current PIN: ");
    scanf("%d", &oldPin);

    if (oldPin != acc[i].pin)
    {
        printf("Wrong PIN!\n");
        return;
    }

    do
    {
        printf("Enter New 4-Digit PIN: ");
        scanf("%d", &newPin);

        if (newPin < 1000 || newPin > 9999)
        {
            printf("PIN must be 4 digits!\n");
        }

    } while (newPin < 1000 || newPin > 9999);

    acc[i].pin = newPin;
addTransaction(acc[i].accNo, "PIN Changed");

printf("PIN Changed Successfully!\n");

printReceipt(
    acc[i].accNo,
    "PIN Changed",
    0,
    acc[i].balance
);

saveAccounts();
}
// History
void showHistory()
{
    FILE *fp = fopen("history.txt","r");

    if(fp == NULL)
    {
        printf("No History Found!\n");
        return;
    }

    char ch;

    while((ch=fgetc(fp)) != EOF)
    {
        putchar(ch);
    }

    fclose(fp);
}
// ==============================
// USER MENU
// ==============================
void userMenu(int i)
{
    int choice;

    while (1)
    {
printf("\n===== USER MENU =====\n");
printf("1. Deposit\n");
printf("2. Withdraw\n");
printf("3. Check Balance\n");
printf("4. Change PIN\n");
printf("5. Transaction History\n");
printf("6. Logout\n");

        printf("Enter Choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            deposit(i);
            break;

        case 2:
            withdraw(i);
            break;

        case 3:
            checkBalance(i);
            break;

        case 4:
            changePIN(i);
            break;

case 5:
    showHistory();
    break;

case 6:
    printf("Logged Out Successfully!\n");
    return;

        default:
            printf("Invalid Choice!\n");
        }
    }
}

// ==============================
// LOGIN (3 ATTEMPTS SYSTEM)
// ==============================
void login()
{
    int accNo, pin;
    int found = -1;

    printf("\n===== LOGIN =====\n");

    printf("Enter Account No: ");
    scanf("%d", &accNo);

    for (int i = 0; i < count; i++)
    {
        if (acc[i].accNo == accNo)
        {
            found = i;
            break;
        }
    }

    if (found == -1)
    {
        printf("Account Not Found!\n");
        return;
    }

    int attempts = 0;

    while (attempts < 3)
    {
        printf("Enter PIN: ");
        scanf("%d", &pin);

        if (pin == acc[found].pin)
        {
            printf("\nLogin Successful!\n");
            printf("Welcome %s\n", acc[found].name);

            updateLastLogin(found);

            userMenu(found);
            return;
        }

        attempts++;

        printf("Wrong PIN! Attempts left: %d\n", 3 - attempts);
    }

    printf("Account Locked (Too many attempts)\n");
}

// ==============================
// MAIN MENU
// ==============================
int main()
{
    loadAccounts();

    int choice;

    while (1)
    {
        printf("\n==============================\n");
        printf("   BANK MANAGEMENT SYSTEM\n");
        printf("==============================\n");

        printf("1. Create Account\n");
        printf("2. Login\n");
        printf("3. Exit\n");

        printf("Enter Choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            createAccount();
            break;

        case 2:
            login();
            break;

        case 3:
            saveAccounts();
            printf("Thank You!\n");
            exit(0);

        default:
            printf("Invalid Choice!\n");
        }
    }

    return 0;
}