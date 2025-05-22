#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <list>
#include <fstream>
#include <stdexcept>
#include <iomanip>

using namespace std;

// Base class for Inventory Item
class InventoryItem {
protected:
    string name;
    int quantity;

public:
    InventoryItem(string n, int q) : name(n), quantity(q) {}
    virtual void display() const {
        cout << left << setw(15) << name << setw(10) << quantity;
    }
    virtual string serialize() const = 0;
    virtual string getType() const = 0;
    virtual string getExtraInfo() const = 0;
    virtual ~InventoryItem() {}
    string getName() const { return name; }
};

// Derived class for Electronic Items
class ElectronicItem : public InventoryItem {
    int warranty;

public:
    ElectronicItem(string n, int q, int w) : InventoryItem(n, q), warranty(w) {}
    void display() const override {
        InventoryItem::display();
        cout << left << setw(15) << "Electronic" << warranty << " years" << endl;
    }
    string serialize() const override {
        return name + "," + to_string(quantity) + ",Electronic," + to_string(warranty) + " years\n";
    }
    string getType() const override { return "Electronic"; }
    string getExtraInfo() const override { return to_string(warranty) + " years"; }
};

// Derived class for Furniture Items
class FurnitureItem : public InventoryItem {
    string material;

public:
    FurnitureItem(string n, int q, string m) : InventoryItem(n, q), material(m) {}
    void display() const override {
        InventoryItem::display();
        cout << left << setw(15) << "Furniture" << material << endl;
    }
    string serialize() const override {
        return name + "," + to_string(quantity) + ",Furniture," + material + "\n";
    }
    string getType() const override { return "Furniture"; }
    string getExtraInfo() const override { return material; }
};

// Inventory Management System
class InventoryManager {
private:
    unordered_map<string, shared_ptr<InventoryItem>> inventory;
    list<string> transactionLog;
    const string filename = "inventory_data.txt";

public:
    InventoryManager() {
        loadInventory();
    }

    void addItem(const string &name, shared_ptr<InventoryItem> item) {
        inventory[name] = item;
        transactionLog.push_back("Added " + name);
        saveInventory();
    }

    void displayInventory() const {
        if (inventory.empty()) {
            cout << "Inventory is empty." << endl;
            return;
        }
        cout << "\nComplete Inventory List:\n";
        cout << left << setw(15) << "Name" << setw(10) << "Quantity" << setw(15) << "Type" << "Extra Info" << endl;
        cout << string(50, '-') << endl;
        for (const auto &pair : inventory) {
            pair.second->display();
        }
    }

    void displayTransactions() const {
        cout << "\nTransaction Log:\n";
        if (transactionLog.empty()) {
            cout << "No transactions recorded." << endl;
            return;
        }
        for (const auto &log : transactionLog) {
            cout << log << endl;
        }
    }

    void saveInventory() {
        ofstream file(filename, ios::app);
        if (!file) {
            cerr << "Error opening file for writing!" << endl;
            return;
        }
        for (const auto &pair : inventory) {
            file << pair.second->serialize();
        }
        file.close();
    }

    void loadInventory() {
        ifstream file(filename);
        if (!file) {
            return;
        }
        string type, name, extraInfo;
        int quantity;
        while (getline(file, name, ',') && file >> quantity && getline(file, type, ',')) {
            getline(file, extraInfo);
            if (type == "Electronic") {
                inventory[name] = make_shared<ElectronicItem>(name, quantity, stoi(extraInfo));
            } else if (type == "Furniture") {
                inventory[name] = make_shared<FurnitureItem>(name, quantity, extraInfo);
            }
        }
        file.close();
    }
};

int main() {
    try {
        InventoryManager manager;
        int n;
        cout << "Enter number of items: ";
        cin >> n;

        for (int i = 0; i < n; ++i) {
            string name, type;
            int quantity, warranty;
            string material;

            cout << "Enter item name: ";
            cin >> name;
            cout << "Enter quantity: ";
            cin >> quantity;
            cout << "Enter item type (electronic/furniture): ";
            cin >> type;

            if (type == "electronic") {
                cout << "Enter warranty (years): ";
                cin >> warranty;
                manager.addItem(name, make_shared<ElectronicItem>(name, quantity, warranty));
            } else if (type == "furniture") {
                cout << "Enter material type: ";
                cin >> material;
                manager.addItem(name, make_shared<FurnitureItem>(name, quantity, material));
            } else {
                cout << "Invalid item type. Skipping...\n";
            }
        }

        manager.displayInventory();
        manager.displayTransactions();
    } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}
