#include <iostream>
#include <fstream>
#include <cctype>
#include <ctime>
#include <vector>
#include <conio.h>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <string>

using namespace std;

class userLogin {
    string username;
    string password;

public:
    userLogin() : username(""), password("") {}

    void getCredentials() {
        cout << "\n\tUsername: ";
        cin >> username;
        cout << "\tPassword: ";
        cin.ignore();
        char x;
        while ((x = _getch()) != '\r') {
            if (x != '\b') {
                password.push_back(x);
                cout << '*';
            }
            else if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        }
    }

    bool validateCredentials() {
        return (username == "Administrator" || username.substr(0, 8) == "employee") && password == "Password@1234";
    }
};

class FinancialRecord {
public:
    string category; 
    double amount;
    string date;

    FinancialRecord(string t, double a) : category(t), amount(a) {
        time_t now;
        time(&now);
        char buffer[26];
        ctime_s(buffer, sizeof buffer, &now);
        date = buffer;
    }
};

class Consumer {
public:
    string name;
    string contactNo;
    vector<FinancialRecord> FinancialRecords;

    Consumer(string n, string c) : name(name), contactNo(contactNo) {}

    void  addTransaction(string category, double amount) {
        FinancialRecords.push_back(FinancialRecord(category, amount));
    }
};

class BankAccount : public Consumer
{
private:
    int accountNo;
    string category;
    double balance; 
    double overdraftLimit; 

public:
    BankAccount(int no, string n, string c, char t) : accountNo(no), Consumer(n, c), category(), balance(0), overdraftLimit(0) {}

    void setOverdraftLimit(double limit)
    {
        if (category == "Current")
            overdraftLimit = limit;
    }

    void createAccount()
    {
        cout << "\nEnter Type Of The Account (Savings or Current) : ";
        cin >> category;
        

        cout << "\nEnter The Initial amount: ";
        cin >> balance;

        if (category == "Current")
        {
            cout << "\nSpecify the overdraft threshold for the Current Account: ";
            cin >> overdraftLimit;
        }

        cout << "\n\n\n                 Account Created..";
    }


    int accountNumber() const
    {
        return accountNo;
    }

    string accountType() const
    {
        return category;
    }

    void displayAccountInformation() const
    {
        cout << "\n\n\nAccount No. : " << accountNo;
        cout << "\nAccount Holder Name : " << name;
        cout << "\nContact Number : " << contactNo;
        cout << "\nType of Account : " << category;
        cout << "\nBalance amount : " << balance;
    }

    void alterAccountDetails()
    {
        cout << "\n\n\t\t\tModify Account Holder Name : ";
        cin.ignore();
        getline(cin, name);
        cout << "\n\t\t\tModify Contact Number : ";
        getline(cin, contactNo);
        cout << "\n\t\t\tModify Type of Account : ";
        cin >> category;
        
        cout << "\n\t\t\tModify Balance amount : ";
        cin >> balance;

        if (category == "Current")
        {
            cout << "\n\t\t\tModify Overdraft Limit : ";
            cin >> overdraftLimit;
        }
    }

    double getOverdraftLimit() const
    {
        return overdraftLimit;
    }

    void deposit(double amount)
    {
        balance += amount;
        addTransaction("Deposit", amount);
    }

    void makeWithdrawal(double amount)
    {
        if (category == "Savings")
        {
            if (balance >= amount)
            {
                balance -= amount;
                addTransaction("", amount);
            }
            else
            {
                cout << "Transaction failed. Insufficient balance." << endl;
            }
        }
        else if (category == "Current")
        {
            if ((balance - amount) >= -overdraftLimit)
            {
                balance -= amount;
                addTransaction("", amount);
            }
            else
            {
                cout << "The transaction cannot be completed due to insufficient funds or surpassing the overdraft limit" << endl;
            }
        }
        else
        {
            cout << "Invalid account type" << endl;
        }
    }


    const vector<FinancialRecord>& accountTransactions() const
    {
        return FinancialRecords;
    }

    void report() const
    {
        cout << accountNo << setw(10) << " " << name << setw(15) << " " << contactNo << setw(10) << " " << category << setw(6) << balance << endl;
    }



    double accountBalance() const
    {
        return balance;
    }

   

};



class Employee
{
public:
    void addNewConsumer(vector<BankAccount>& accounts)
    {
        int accountNo = accounts.empty() ? 1 : accounts.back().accountNumber() + 1;

        string name, contact;
        cout << "\nEnter The Name Of Account Holder : ";
        cin.ignore();
        getline(cin, name);
        cout << "\nEnter The Contact Number : ";
        getline(cin, contact);

        char accountType;
        cout << "\nEnter Type Of The Account (Savings or Current) : ";
        cin >> accountType;
        accountType = toupper(accountType);

        BankAccount newAccount(accountNo, name, contact, accountType);
        accounts.push_back(newAccount);

        newAccount.createAccount();
        cout << "\nAccount Number: " << accountNo << "\n\n";

        ofstream Output;
        Output.open("file.dat", ios::binary | ios::app);
        Output.write(reinterpret_cast<char*>(&newAccount), sizeof(BankAccount));
        Output.close();
    }


    void deposit(vector<BankAccount>& accounts, int accountNo)
    {
        auto it = find_if(accounts.begin(), accounts.end(), [accountNo](const BankAccount& acc) {
            return acc.accountNumber() == accountNo;
            });

        if (it != accounts.end())
        {
            double depositAmount;
            cout << "\nEnter the amount to deposit: ";
            cin >> depositAmount;

            double previousBalance = it->accountBalance();
            it->deposit(depositAmount);

        
            cout << "\nAmount deposited successfully.";
            cout << "\nPrevious Balance: " << previousBalance;
            cout << "\nDeposited Amount: " << depositAmount;
            cout << "\nCurrent Balance: " << it->accountBalance();

            // Update account details in the file
            ofstream Output("file.dat", ios::binary | ios::trunc);
            for (const auto& acc : accounts)
            {
                Output.write(reinterpret_cast<const char*>(&acc), sizeof(BankAccount));
            }
            Output.close();
        }
        else
        {
            cout << "\nAccount not found.";
        }
    }

    void withdraw(vector<BankAccount>& accounts, int accountNo)
    {
        auto it = find_if(accounts.begin(), accounts.end(), [accountNo](const BankAccount& acc) {
            return acc.accountNumber() == accountNo;
            });

        if (it != accounts.end())
        {
            double withdrawAmount;
            cout << "\nEnter the amount to withdraw: ";
            cin >> withdrawAmount;

            double previousBalance = it->accountBalance();
            it->makeWithdrawal(withdrawAmount);

            // Display information after withdrawal
            cout << "\nAmount withdrawn successfully.";
            cout << "\nPrevious Balance: " << previousBalance;
            cout << "\nWithdrawn Amount: " << withdrawAmount;
            cout << "\nCurrent Balance: " << it->accountBalance();

            // Update account details in the file
            ofstream Output("file.dat", ios::binary | ios::trunc);
            for (const auto& acc : accounts)
            {
                Output.write(reinterpret_cast<const char*>(&acc), sizeof(BankAccount));
            }
            Output.close();
        }
        else
        {
            cout << "\nAccount not found.";
        }
    }

    void close(vector<BankAccount>& accounts, int accountNo)
    {
        auto it = find_if(accounts.begin(), accounts.end(), [accountNo](const BankAccount& acc) {
            return acc.accountNumber() == accountNo;
            });

        if (it != accounts.end())
        {
            accounts.erase(it);
            cout << "\nAccount closed successfully";
        }
        else
        {
            cout << "\nAccount not found.";
        }

        ofstream Output;
        Output.open("file.dat", ios::binary | ios::trunc);
        for (const auto& acc : accounts)
        {
            Output.write(reinterpret_cast<const char*>(&acc), sizeof(BankAccount));
        }
        Output.close();
    }

    void overdraftLimit(vector<BankAccount>& accounts, int accountNo, double limit)
    {
        auto it = find_if(accounts.begin(), accounts.end(), [accountNo](const BankAccount& acc) {
            return acc.accountNumber() == accountNo && acc.accountType() == "Current";
            });

        if (it != accounts.end())
        {
            it->setOverdraftLimit(limit);
            cout << "\nOverdraft limit set successfully.";
        }
        else
        {
            cout << "\nAccount not found or not a current account.";
        }

        ofstream Output;
        Output.open("file.dat", ios::binary | ios::trunc);
        for (const auto& acc : accounts)
        {
            Output.write(reinterpret_cast<const char*>(&acc), sizeof(BankAccount));
        }
        Output.close();
    }

    void viewTransactions(const vector<BankAccount>& accounts, int accountNo)
    {
        auto it = find_if(accounts.begin(), accounts.end(), [accountNo](const BankAccount& acc) {
            return acc.accountNumber() == accountNo;
            });

        if (it != accounts.end())
        {
            const auto& transactions = it->accountTransactions();
            cout << "\nTransactions for Account No. " << accountNo << ":\n";
            for (const auto& transaction : transactions)
            {
                cout << transaction.date << " - " << transaction.category << " - " << transaction.amount << "\n";
            }
        }
        else
        {
            cout << "\nAccount not found.";
        }
    }
};


class Administrator : public Employee
{
private:
    double annualSavingInterest;
    double overdraftFee;

public:
    Administrator() : annualSavingInterest(0.0), overdraftFee(0.0) {}

    void modifyDate(vector<BankAccount>& accounts)
    {
        for (auto& acc : accounts)
        {
            if (acc.accountType() == "Savings")
            {
                double interest = acc.accountBalance() * (annualSavingInterest / 100);
                acc.deposit(interest);
            }
            else if (acc.accountType() == "Current" && acc.accountBalance() < 0)
            {
                double charge = overdraftFee;
                acc.makeWithdrawal(charge);
            }
        }

        ofstream Output;
        Output.open("file.dat", ios::binary | ios::trunc);
        for (const auto& acc : accounts)
        {
            Output.write(reinterpret_cast<const char*>(&acc), sizeof(BankAccount));
        }
        Output.close();
    }

    void setAnnualSavingInterest(double interest)
    {
        annualSavingInterest = interest;
    }

    void setOverdraftPenalty(double charge)
    {
        overdraftFee = charge;
    }
};








int main()
{
    system("COLOR 0c");

    userLogin userLogin;
    bool loginSuccess = false;

    do
    {
        system("cls");
        userLogin.getCredentials();
        loginSuccess = userLogin.validateCredentials();

        if (!loginSuccess)
        {
            cout << "\nInvalid credentials. Please try again.\n";
            cin.ignore();
            cin.get();
        }

    } while (!loginSuccess);

    system("COLOR f0");
    char choice;
    vector<BankAccount> accounts;
    Administrator ad;

    do
    {
        system("cls");
        cout << "---------------------------------------------------------------------------";
        cout << "\n                  Dear , Welcome to Bank LMN           ";
        cout << "\n---------------------------------------------------------------------------";

        cout << "\n\n\t1. Open a New Account";
        cout << "\n\t2. Deposit Money";
        cout << "\n\t3. Withdraw Money";
        cout << "\n\t4. Check Balance";
        cout << "\n\t5. Close an Account";
        cout << "\n\t6. Modify Account Details";
        cout << "\n\t7. Administrator Menu";
        cout << "\n\t8. Exit";
        cout << "\n---------------------------------------------------------------------------";
        cout << "\n\n\n\t   Please Choose an Option(1-8) : ";
        cin >> choice;
        system("cls");

        // Check if accounts vector is empty before setting accountNum
        int accountNum = accounts.empty() ? -1 : accounts.back().accountNumber();

        if (choice == '1') {
            ad.addNewConsumer(accounts);
        }
        else if (choice == '2' || choice == '3' || choice == '4' || choice == '5' || choice == '6' ) {
            cout << "\n\n\tEnter The account No. : ";
            cin >> accountNum;
        }
        else if (choice == '2') {
            ad.deposit(accounts, accountNum);
        }
        else if (choice == '3') {
            ad.withdraw(accounts, accountNum);
        }
        else if (choice == '4') {
            ad.viewTransactions(accounts, accountNum);
        }
        else if (choice == '5') {
            ad.close(accounts, accountNum);
        }
        else if (choice == '6') {
            ad.overdraftLimit(accounts, accountNum, 500);
        }
        else if (choice == '7') {
            // Administrator menu
            char adChoice;

            cout << "---------------------------------------------------------------------------";
            cout << "\n                       Custom Administration Panel";
            cout << "\n---------------------------------------------------------------------------";
            cout << "\n\n\t1. Update Date";
            cout << "\n\t2. Set Annual Savings Interest Rate";
            cout << "\n\t3. Set Overdraft Penalty";
            cout << "\n\t4. Eamountit Administrator Menu";
            cout << "\n---------------------------------------------------------------------------";
            cout << "\n\n\t\t   Please Choose an Option : ";
            cin >> adChoice;

            if (adChoice == '1') {
                ad.modifyDate(accounts);
                cout << "\nDate increased successfully.";
            }
            else if (adChoice == '2') {
                double interest;
                cout << "\nEnter Annual Saving Interest Rate: ";
                cin >> interest;
                ad.setAnnualSavingInterest(interest);
                cout << "\nAnnual Saving Interest set successfully.";
            }
            else if (adChoice == '3') {
                double charge;
                cout << "\nEnter Overdraft Charge: ";
                cin >> charge;
                ad.setOverdraftPenalty(charge);
                cout << "\nOverdraft Charge set successfully.";
            }
            else if (adChoice == '4') {
            }
            else {
                cout << "\a";
            }
        }
        else if (choice == '8') {
            cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n                      We appreciate your choice in using the LMN Banking System";
        }
        else {
            cout << "\a";
        }

        cin.ignore();
        cin.get();
    } while (choice != '8');

return 0;
}

