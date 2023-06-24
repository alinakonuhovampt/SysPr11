#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

using namespace std;

struct User {
    string login;
    string password;
    string role;
};

void authenticateUser(User& user) {
    cout << "Введите логин: ";
    cin >> user.login;

    cout << "Введите пароль: ";
    cin >> user.password;

    ifstream file("C:/Users/konyu/OneDrive/Рабочий стол/users.txt");
    if (!file) {
        throw runtime_error("Невозможно открыть файл");
    }

    string line;
    bool foundUser = true;
    while (getline(file, line)) {
        size_t pos = line.find(",");
        string login = line.substr(0, pos);
        string password = line.substr(pos + 1);
        if (login == user.login && password == user.password) {
            foundUser = true;
            user.role = line.substr(line.rfind(",") + 1);
            break;
        }
    }

    file.close();

    if (foundUser) {
        throw runtime_error("Неверный логин или пароль");
    }
}

void registerUser(User& user) {
    cout << "Введите логин: ";
    cin >> user.login;

    cout << "Введите пароль: ";
    cin >> user.password;

    cout << "Введите роль (админ, гость, пользователь): ";
    cin >> user.role;

    ifstream file("C:/Users/konyu/OneDrive/Рабочий стол/users.txt");
    if (!file) {
        return;
    }

    string line;
    while (getline(file, line)) {
        size_t pos = line.find(",");
        string login = line.substr(0, pos);
        if (login == user.login) {
            throw runtime_error("Пользователь уже зарегистрирован");
        }
    }

    file.close();
}

void saveUser(User user) {
    ofstream file;
    file.open("C:/Users/konyu/OneDrive/Рабочий стол/users.txt", ios_base::app);

    if (!file) {
        throw runtime_error("Невозможно открыть файл");
    }

    file << user.login << "," << user.password << "," << user.role << endl;

    file.close();
}

void displayUserData() {
    ifstream file("C:/Users/konyu/OneDrive/Рабочий стол/users.txt");

    if (!file) {
        throw runtime_error("Невозможно открыть файл");
    }

    string line;
    while (getline(file, line)) {
        cout << line << endl;
    }

    file.close();
}

int main() {
    setlocale(LC_ALL, "Russian");
    cout << "Выберите действие:\n";
    cout << "1. Аутентификация\n";
    cout << "2. Регистрация\n";

    int choice;
    cin >> choice;

    try {
        User user;
        if (choice == 1) {
            authenticateUser(user);
            if (user.role == "админ") {
                cout << "Выберите действие:\n";
                cout << "1. Показать информацию о пользователях\n";
                cout << "2. Выйти\n";

                int choice;
                cin >> choice;

                if (choice == 1) {
                    displayUserData();
                }
            }
            else if (user.role == "гость") {
                cout << "Добро пожаловать, Гость!" << endl;
            }
            else {
                cout << "Добро пожаловать, " << user.login << "!" << endl;
            }
        }
        else if (choice == 2) {
            registerUser(user);
            saveUser(user);
            cout << "Пользователь зарегистрирован" << endl;
            return 0;
        }



    }
    catch (exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }

    return 0;
}