#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <cstdlib>
#include <ctime>

using json = nlohmann::json;
using namespace std;

struct User {
    string username;
    string password;
    double balance;
    double creditDebt = 0.0;
};

class Bank {
private:
    unordered_map<string, User> users;
    const string dataFile = "data/bank_data.json";
    string language = "en";

    void loadData() {
        ifstream file(dataFile);
        if (file) {
            json j;
            file >> j;
            for (auto& [key, value] : j.items()) {
                users[key] = {key, value["password"], value["balance"], value.value("creditDebt", 0.0)};
            }
        }
    }

    void saveData() {
        json j;
        for (const auto& [username, user] : users) {
            j[username] = { {"password", user.password}, {"balance", user.balance}, {"creditDebt", user.creditDebt} };
        }
        ofstream file(dataFile);
        file << j.dump(4);
    }

public:
    Bank() { loadData(); }
    ~Bank() { saveData(); }

    void setLanguage(const string& lang) {
        if (lang == "en" || lang == "ru" || lang == "de" || lang == "fr") {
            language = lang;
        }
    }

    bool registerUser(const string& username, const string& password) {
        if (users.find(username) != users.end()) {
            cout << "User already exists!\n";
            return false;
        }
        users[username] = {username, password, 0.0, 0.0};
        saveData();
        cout << "User registered successfully!\n";
        return true;
    }

    bool loginUser(const string& username, const string& password) {
        if (users.find(username) != users.end() && users[username].password == password) {
            cout << "Login successful!\n";
            return true;
        }
        cout << "Invalid credentials!\n";
        return false;
    }

    void deposit(const string& username, double amount) {
        if (users.find(username) != users.end()) {
            users[username].balance += amount;
            saveData();
            cout << "Deposited " << amount << "\n";
        }
    }

    void transfer(const string& from, const string& to, double amount) {
        if (users.find(from) != users.end() && users.find(to) != users.end() && users[from].balance >= amount) {
            users[from].balance -= amount;
            users[to].balance += amount;
            saveData();
            cout << "Transferred " << amount << " to " << to << "\n";
        } else {
            cout << "Transfer failed!\n";
        }
    }

    void takeCredit(const string& username, double amount) {
        if (users.find(username) != users.end()) {
            users[username].balance += amount;
            users[username].creditDebt += amount;
            saveData();
            cout << "Credit approved: " << amount << "\n";
        }
    }

    void repayCredit(const string& username, double amount) {
        if (users.find(username) != users.end() && users[username].balance >= amount && users[username].creditDebt > 0) {
            users[username].balance -= amount;
            users[username].creditDebt -= amount;
            if (users[username].creditDebt < 0) users[username].creditDebt = 0;
            saveData();
            cout << "Credit repaid: " << amount << "\n";
        } else {
            cout << "Repayment failed!\n";
        }
    }

    void mineResources(const string& username) {
        if (users.find(username) == users.end()) return;
        srand(time(0));
        int coal = rand() % 10 + 1;
        int iron = rand() % 5;
        int diamonds = (rand() % 100 < 5) ? 1 : 0;
        double earnings = coal * 10 + iron * 25 + diamonds * 100;
        users[username].balance += earnings;
        saveData();
        cout << "You mined " << coal << " coal, " << iron << " iron, and " << diamonds << " diamonds!\n";
        cout << "Earned: " << earnings << "\n";
    }

    void checkBalance(const string& username) {
        if (users.find(username) != users.end()) {
            cout << "Your balance: " << users[username].balance << "\n";
        }
    }
};

int main() {
    Bank bank;
    string lang;
    cout << "Select language (en, ru, de, fr): ";
    cin >> lang;
    bank.setLanguage(lang);

    string username, password;
    int choice;
    bool loggedIn = false;

    while (true) {
        cout << "1. Register\n2. Login\n3. Exit\nChoice: ";
        cin >> choice;

        if (choice == 1) {
            cout << "Enter username: "; cin >> username;
            cout << "Enter password: "; cin >> password;
            bank.registerUser(username, password);
        } else if (choice == 2) {
            cout << "Enter username: "; cin >> username;
            cout << "Enter password: "; cin >> password;
            loggedIn = bank.loginUser(username, password);
            if (loggedIn) break;
        } else {
            return 0;
        }
    }

    while (loggedIn) {
        cout << "1. Deposit\n2. Transfer\n3. Take Credit\n4. Repay Credit\n5. Mine Resources\n6. Check Balance\n7. Logout\nChoice: ";
        cin >> choice;

        if (choice == 1) {
            double amount;
            cout << "Enter amount: "; cin >> amount;
            bank.deposit(username, amount);
        } else if (choice == 2) {
            string recipient;
            double amount;
            cout << "Enter recipient: "; cin >> recipient;
            cout << "Enter amount: "; cin >> amount;
            bank.transfer(username, recipient, amount);
        } else if (choice == 3) {
            double amount;
            cout << "Enter credit amount: "; cin >> amount;
            bank.takeCredit(username, amount);
        } else if (choice == 4) {
            double amount;
            cout << "Enter repayment amount: "; cin >> amount;
            bank.repayCredit(username, amount);
        } else if (choice == 5) {
            bank.mineResources(username);
        } else if (choice == 6) {
            bank.checkBalance(username);
        } else {
            loggedIn = false;
        }
    }
    return 0;
}
