#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <openssl/evp.h>
#include <vector>

using namespace std;

const string PATH = "C:/Users/konyu/users.txt";
const char SEPARATOR = ',';
const string SALT = "someRandomSaltValue";


struct User {
    string login;
    string password;
    string role;
};

class MenuItem {
    string name;
    float price;
public:
    MenuItem(string n, float p) : name(n), price(p) {}
    string getName() { return name; }
    float getPrice() { return price; }
};

class Menu {
    vector<MenuItem> items;
public:
    // Возвращает количество элементов в меню
    int size() { return items.size(); }
    // Добавляет элемент в конец меню
    void addItem(MenuItem item) { items.push_back(item); }
    // Возвращает элемент по его индексу
    MenuItem getItem(int index) { return items[index]; }
    // Выводит на экран все элементы меню
    void print() {
        for (int i = 0; i < items.size(); i++) {
            cout << i + 1 << ". " << items[i].getName() << " - " << items[i].getPrice() << endl;
        }
    }
};

class Order {
    Menu menu;
    vector<MenuItem> items;
public:
    // Добавляет элемент меню в заказ
    void addItem(MenuItem item) { items.push_back(item); }
    // Возвращает общую стоимость заказа
    float getTotal() {
        float total = 0;
        for (int i = 0; i < items.size(); i++) {
            total += items[i].getPrice();
        }
        return total;
    }
    // Записывает чек заказа в файл
    void writeToFile(string filename) {
        ofstream outFile;
        outFile.open(filename);
        outFile << "Заказ: " << endl;
        for (int i = 0; i < items.size(); i++) {
            outFile << items[i].getName() << " - " << items[i].getPrice() << endl;
        }
        outFile << "Общая стоимость: " << getTotal() << endl;
        outFile.close();
    }
};


size_t getSeparatorPosition(const string& line) {
    return line.find(SEPARATOR);
}

void removePrefix(string& line) {
    line.erase(0, line.find(SEPARATOR) + 1);
}

string getPrefix(const string& line) {
    return line.substr(0, getSeparatorPosition(line));
}

string encodeAsMD5(const string& input) {
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_md5();
    unsigned char digest[16];
    unsigned int digestLen;

    EVP_MD_CTX_init(mdctx);
    EVP_DigestInit_ex(mdctx, md, nullptr);
    EVP_DigestUpdate(mdctx, input.c_str(), input.length());
    EVP_DigestFinal_ex(mdctx, digest, &digestLen);
    EVP_MD_CTX_free(mdctx);

    stringstream stream;
    for (const auto i : digest) {
        stream << hex << setw(2) << setfill('0') << static_cast<int>(i);
    }

    return stream.str();
}


void raiseOnInvalidCredentials(User& user) {
    cout << "Введите логин: ";
    cin >> user.login;
    string unencryptedPass;
    cout << "Введите пароль: ";
    cin >> unencryptedPass;
    user.password = encodeAsMD5(unencryptedPass + SALT);

    ifstream file(PATH);
    if (!file) {
        throw runtime_error("Невозможно открыть файл");
    }
    string line;
    bool isUserFound = false;
    while (getline(file, line)) {
        string login = getPrefix(line);
        removePrefix(line);
        string password = getPrefix(line);
        removePrefix(line);
        if (login == user.login && password == user.password) {
            isUserFound = true;
            user.role = getPrefix(line);
            break;
        }
    }

    file.close();

    if (!isUserFound) {
        throw runtime_error("Неверный логин или пароль");
    }
}

void registerUser(User& user) {
    cout << "Введите логин: ";
    cin >> user.login;

    cout << "Введите пароль: ";
    cin >> user.password;

    cout << "Введите роль [админ | гость | пользователь]: ";
    cin >> user.role;

    ifstream file(PATH);
    if (!file) {
        throw runtime_error("Невозможно открыть файл");
    }

    string line;
    while (getline(file, line)) {
        size_t pos = line.find(',');
        string login = line.substr(0, pos);
        if (login == user.login) {
            throw runtime_error("Пользователь уже зарегистрирован");
        }
    }

    file.close();
}

void saveUser(const User& user) {
    ofstream file;
    file.open(PATH, ios_base::app);

    if (!file) {
        throw runtime_error("Невозможно открыть файл");
    }

    file << user.login << "," << encodeAsMD5(user.password + SALT) << "," << user.role << endl;

    file.close();
}

void displayUserData() {
    ifstream file(PATH);

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

    Menu menu;
    Order order;

    menu.addItem(MenuItem("Роллы", 150));
    menu.addItem(MenuItem("Суп", 100));
    menu.addItem(MenuItem("Салат", 80));


    cout << "Выберите действие:\n"
        "1. Аутентификация\n"
        "2. Регистрация\n";

    int choice;
    cin >> choice;
    try {
        User user;
        switch (choice) {
        case 1:
            raiseOnInvalidCredentials(user);
            if (user.role == "админ") {
                cout << "Выберите действие:\n"
                    "1. Показать информацию о пользователях\n"
                    "2. Добавить новый пункт в меню\n";

                int choice_second;

                cin >> choice_second;
                if (choice_second == 1) {
                    displayUserData();
                }
                if (choice_second == 2) {
                    // Добавление нового элемента в меню
                    string name;
                    float price;
                    cout << "Введите название нового пункта: ";
                    cin >> name;
                    cout << "Введите стоимость нового пункта: ";
                    cin >> price;
                    MenuItem item(name, price);
                    menu.addItem(item);
                }
            }
            if (user.role == "гость") {
                cout << "Добро пожаловать, Гость!" << endl;
                cout << "Выберите действие:\n"
                    "1. Вывести меню\n"
                    "2. Сделать заказ\n";

                int choice_second;
                cin >> choice_second;

                if (choice_second == 1) {
                    menu.print();
                }
                if (choice_second == 2) {
                    // Составление заказа
                    int itemIndex;
                    do {
                        cout << "Введите номер пункта меню для добавления в заказ (0 - закончить заказ): ";
                        cin >> itemIndex;
                        if (itemIndex > 0 && itemIndex <= menu.size()) {
                            MenuItem item = menu.getItem(itemIndex - 1);
                            order.addItem(item);
                            cout << "Добавлено в заказ: " << item.getName() << endl;
                        }
                        else if (itemIndex < 0 || itemIndex > menu.size()) {
                            cout << "Неверный номер пункта меню. Пожалуйста, попробуйте еще раз." << endl;
                        }
                    } while (itemIndex != 0);

                    // Запись чека заказа в файл
                    string filename;
                    cout << "Введите имя файла для сохранения чека: ";
                    cin >> filename;
                    order.writeToFile(filename);
                    cout << "Чек заказа сохранен в файл " << filename << "." << endl;
                }
            }
            else {
                cout << "Добро пожаловать, " << user.login << "!" << endl;
            }
            break;
        case 2:
            registerUser(user);
            saveUser(user);
            cout << "Пользователь зарегистрирован" << endl;
            break;
        default:
            throw runtime_error("invalid option: '" + to_string(choice) + "' ;must be 1 or 2");
        }
    }
    catch (exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return -1;
    }
    return 0;
}