#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

const string DATA_FILE = "contacts.txt";

// Clears bad input from the buffer so the program doesn't get stuck
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Lowercases a string so search can be case-insensitive
string toLower(const string& s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// ---------------------------------------------------------
// Contact struct: holds one person's details
// ---------------------------------------------------------
struct Contact {
    int id;
    string name;
    string phone;
    string email;
    string address;

    // Converts a contact to one pipe-delimited line for file storage
    string serialize() const {
        ostringstream oss;
        oss << id << "|" << name << "|" << phone << "|" << email << "|" << address;
        return oss.str();
    }

    // Rebuilds a contact from a stored line
    static Contact deserialize(const string& line) {
        stringstream ss(line);
        string token;
        Contact c;

        getline(ss, token, '|'); c.id = stoi(token);
        getline(ss, c.name, '|');
        getline(ss, c.phone, '|');
        getline(ss, c.email, '|');
        getline(ss, c.address, '|');

        return c;
    }

    void display() const {
        cout << "ID: " << id << "\n"
             << "  Name:    " << name << "\n"
             << "  Phone:   " << phone << "\n"
             << "  Email:   " << email << "\n"
             << "  Address: " << address << "\n";
    }
};

// ---------------------------------------------------------
// ContactBook class: owns the contact list and file I/O
// ---------------------------------------------------------
class ContactBook {
private:
    vector<Contact> contacts;
    int nextId;

public:
    ContactBook() : nextId(1) {
        loadFromFile();
        for (const auto& c : contacts) {
            if (c.id >= nextId) nextId = c.id + 1;
        }
    }

    void saveToFile() const {
        ofstream file(DATA_FILE);
        for (const auto& c : contacts) {
            file << c.serialize() << "\n";
        }
    }

    void loadFromFile() {
        ifstream file(DATA_FILE);
        string line;
        contacts.clear();
        while (getline(file, line)) {
            if (!line.empty()) contacts.push_back(Contact::deserialize(line));
        }
    }

    void addContact() {
        Contact c;
        c.id = nextId++;

        cin.ignore();
        cout << "Enter name: ";
        getline(cin, c.name);
        cout << "Enter phone: ";
        getline(cin, c.phone);
        cout << "Enter email: ";
        getline(cin, c.email);
        cout << "Enter address: ";
        getline(cin, c.address);

        contacts.push_back(c);
        saveToFile();
        cout << "Contact added with ID " << c.id << ".\n";
    }

    void viewAllContacts() const {
        if (contacts.empty()) {
            cout << "\nNo contacts saved yet.\n";
            return;
        }
        cout << "\n--- All Contacts (" << contacts.size() << ") ---\n";
        for (const auto& c : contacts) {
            c.display();
            cout << "-----------------------\n";
        }
    }

    // Returns a pointer to the contact matching an ID, or nullptr
    Contact* findById(int id) {
        for (auto& c : contacts) {
            if (c.id == id) return &c;
        }
        return nullptr;
    }

    void searchContact() const {
        cin.ignore();
        string query;
        cout << "Enter name or phone to search: ";
        getline(cin, query);
        string queryLower = toLower(query);

        bool found = false;
        for (const auto& c : contacts) {
            if (toLower(c.name).find(queryLower) != string::npos ||
                c.phone.find(query) != string::npos) {
                c.display();
                cout << "-----------------------\n";
                found = true;
            }
        }
        if (!found) cout << "No matching contact found.\n";
    }

    void editContact() {
        int id;
        cout << "Enter the ID of the contact to edit: ";
        cin >> id;
        if (cin.fail()) { cout << "Invalid input!\n"; clearInputBuffer(); return; }

        Contact* c = findById(id);
        if (!c) {
            cout << "No contact found with ID " << id << ".\n";
            return;
        }

        cin.ignore();
        string input;

        cout << "Editing " << c->name << ". Leave blank to keep current value.\n";

        cout << "New name [" << c->name << "]: ";
        getline(cin, input);
        if (!input.empty()) c->name = input;

        cout << "New phone [" << c->phone << "]: ";
        getline(cin, input);
        if (!input.empty()) c->phone = input;

        cout << "New email [" << c->email << "]: ";
        getline(cin, input);
        if (!input.empty()) c->email = input;

        cout << "New address [" << c->address << "]: ";
        getline(cin, input);
        if (!input.empty()) c->address = input;

        saveToFile();
        cout << "Contact updated.\n";
    }

    void deleteContact() {
        int id;
        cout << "Enter the ID of the contact to delete: ";
        cin >> id;
        if (cin.fail()) { cout << "Invalid input!\n"; clearInputBuffer(); return; }

        for (size_t i = 0; i < contacts.size(); i++) {
            if (contacts[i].id == id) {
                cout << "Deleting contact: " << contacts[i].name << "\n";
                contacts.erase(contacts.begin() + i);
                saveToFile();
                cout << "Contact deleted.\n";
                return;
            }
        }
        cout << "No contact found with ID " << id << ".\n";
    }
};

// ---------------------------------------------------------
// Menu-driven interface
// ---------------------------------------------------------
void showMenu() {
    cout << "\n===== Contact Management System =====\n";
    cout << "1. Add Contact\n";
    cout << "2. View All Contacts\n";
    cout << "3. Search Contact (by name/phone)\n";
    cout << "4. Edit Contact\n";
    cout << "5. Delete Contact\n";
    cout << "6. Exit\n";
    cout << "======================================\n";
    cout << "Enter your choice (1-6): ";
}

int main() {
    ContactBook book;
    int choice;
    bool running = true;

    cout << "Welcome to the Contact Management System.\n";
    cout << "(Contacts are loaded from and saved to " << DATA_FILE << ")\n";

    while (running) {
        showMenu();
        cin >> choice;

        if (cin.fail()) {
            cout << "Invalid choice! Please enter a number.\n";
            clearInputBuffer();
            continue;
        }

        switch (choice) {
            case 1: book.addContact(); break;
            case 2: book.viewAllContacts(); break;
            case 3: book.searchContact(); break;
            case 4: book.editContact(); break;
            case 5: book.deleteContact(); break;
            case 6:
                cout << "Saving contacts and exiting. Goodbye!\n";
                running = false;
                break;
            default:
                cout << "Invalid choice! Please select an option between 1 and 6.\n";
        }
    }

    return 0;
}
