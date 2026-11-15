#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
using namespace std;

class ServiceReceiver;
class ServiceProvider;

const int NUM_SERVICES = 9;
string serviceNames[NUM_SERVICES] = {"Electrician", "Plumber", "Carpenter", "Tutor", "Domestic Helper",
                                     "Beautician", "Hairstylist", "Makeup Stylist", "Saree Draper"};

const int NUM_AREAS = 6;
string areaNames[NUM_AREAS] = {"Sector 1", "Railway Road", "City Center", "University Area", "Main Market", "Old Town"};
int areaPosition[NUM_AREAS] = {0, 2, 5, 7, 11, 14};

string dayNames[7] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

const int PENDING = 0;
const int ACCEPTED = 1;
const int REJECTED = 2;
const int COMPLETED = 3;
const int CANCELLED = 4;

const int MAX_FOUND = 300;

int getNumber(string message, int low, int high) {
    int number;
    while (true) {
        cout << message;
        cin >> number;
        if (cin.fail()) {
            if (cin.eof()) exit(0);
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Please enter a number." << endl;
        } else if (number < low || number > high) {
            cin.ignore(1000, '\n');
            cout << "Please enter a number from " << low << " to " << high << "." << endl;
        } else {
            cin.ignore(1000, '\n');
            return number;
        }
    }
}

string getText(string message) {
    string text = "";
    while (text == "") {
        cout << message;
        if (!getline(cin, text)) exit(0);
    }
    return text;
}

bool askYesNo(string message) {
    string answer = getText(message);
    return (answer == "y" || answer == "Y");
}

string toLowerCase(string text) {
    for (int i = 0; i < (int) text.length(); i++) {
        if (text[i] >= 'A' && text[i] <= 'Z') text[i] = text[i] + 32;
    }
    return text;
}

bool containsText(string text, string key) {
    return toLowerCase(text).find(toLowerCase(key)) != string::npos;
}

string readTextLine(ifstream& file) {
    string line;
    getline(file, line);
    return line;
}

int readNumberLine(ifstream& file) {
    return atoi(readTextLine(file).c_str());
}

string chooseService() {
    cout << "\nServices:" << endl;
    for (int i = 0; i < NUM_SERVICES; i++) {
        cout << "  " << (i + 1) << ". " << serviceNames[i] << endl;
    }
    int number = getNumber("Choose a service number: ", 1, NUM_SERVICES);
    return serviceNames[number - 1];
}

int chooseArea() {
    cout << "\nAreas:" << endl;
    for (int i = 0; i < NUM_AREAS; i++) {
        cout << "  " << (i + 1) << ". " << areaNames[i] << endl;
    }
    int number = getNumber("Choose your area number: ", 1, NUM_AREAS);
    return number - 1;
}

int passed = 0;
int failed = 0;

void check(string testName, bool ok) {
    if (ok) {
        cout << "  [PASS] " << testName << endl;
        passed++;
    } else {
        cout << "  [FAIL] " << testName << endl;
        failed++;
    }
}

class User {
protected:
    int id;
    string name;
    string phone;
    string password;
    int areaNumber;
public:
    User(int id, string name, string phone, string password, int areaNumber) {
        this->id = id;
        this->name = name;
        this->phone = phone;
        this->password = password;
        this->areaNumber = areaNumber;
    }
    virtual ~User() {}
    int getId() { return id; }
    string getName() { return name; }
    string getPhone() { return phone; }
    string getPassword() { return password; }
    int getAreaNumber() { return areaNumber; }

    void setPassword(string newPassword) { password = newPassword; }
    void setAreaNumber(int newAreaNumber) { areaNumber = newAreaNumber; }

    bool checkLogin(string enteredPhone, string enteredPassword) {
        return (phone == enteredPhone && password == enteredPassword);
    }

    virtual string getRole() = 0;
    virtual void showProfile() = 0;
};

void showServices();
void searchProviders(ServiceReceiver* receiver);
void searchByName(ServiceReceiver* receiver);
void showNearestProviders(ServiceReceiver* receiver);
void showMyBookings(ServiceReceiver* receiver);
void cancelBooking(ServiceReceiver* receiver);
void giveReview(ServiceReceiver* receiver);
void showReceiverSummary(ServiceReceiver* receiver);
int countReviewsWaiting(int receiverId);
bool providerExists(int providerId);
void showProviderLine(int providerId);
void showRequests(ServiceProvider* provider);
void markCompleted(ServiceProvider* provider);
void showProviderDashboard(ServiceProvider* provider);
int countPendingRequests(int providerId);
void showProviderReviews(int providerId);
void sendRequest(ServiceReceiver* receiver, ServiceProvider* provider);
void rankProviders(ServiceProvider* found[], int count, ServiceReceiver* receiver);
bool verifyProvider(string idProof, string name);
bool isValidPhone(string phone);
bool isValidPassword(string password);
void addReview(int bookingId, int providerId, string receiverName, int rating, string comment);
void saveReviews();

class ServiceReceiver : public User {
public:
    ServiceReceiver(int id, string name, string phone, string password, int areaNumber)
        : User(id, name, phone, password, areaNumber) {}

    string getRole() { return "Service Receiver"; }

    void showProfile() {
        cout << "\n----- Receiver Profile -----" << endl;
        cout << "ID    : " << id << endl;
        cout << "Name  : " << name << endl;
        cout << "Phone : " << phone << endl;
        cout << "Area  : " << areaNames[areaNumber] << endl;
    }
};

class ReceiverNode {
public:
    ServiceReceiver* receiver;
    ReceiverNode* next;
};

class ReceiverList {
private:
    ReceiverNode* head;
    int count;
public:
    ReceiverList() {
        head = NULL;
        count = 0;
    }
    int getCount() { return count; }
    int nextId() { return count + 1; }
    ReceiverNode* getHead() { return head; }

    void add(ServiceReceiver* receiver) {
        ReceiverNode* node = new ReceiverNode;
        node->receiver = receiver;
        node->next = NULL;

        if (head == NULL) {
            head = node;
        } else {
            ReceiverNode* current = head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = node;
        }
        count++;
    }

    ServiceReceiver* findByPhone(string phone) {
        ReceiverNode* current = head;
        while (current != NULL) {
            if (current->receiver->getPhone() == phone) return current->receiver;
            current = current->next;
        }
        return NULL;
    }

    ServiceReceiver* findById(int id) {
        ReceiverNode* current = head;
        while (current != NULL) {
            if (current->receiver->getId() == id) return current->receiver;
            current = current->next;
        }
        return NULL;
    }
};

ReceiverList receivers;

class FavouriteNode {
public:
    int receiverId;
    int providerId;
    FavouriteNode* next;
};

class FavouriteList {
private:
    FavouriteNode* head;
    int count;
public:
    FavouriteList() {
        head = NULL;
        count = 0;
    }
    int getCount() { return count; }
    FavouriteNode* getHead() { return head; }

    bool isFavourite(int receiverId, int providerId) {
        FavouriteNode* current = head;
        while (current != NULL) {
            if (current->receiverId == receiverId && current->providerId == providerId) return true;
            current = current->next;
        }
        return false;
    }

    void add(int receiverId, int providerId) {
        if (isFavourite(receiverId, providerId)) return;

        FavouriteNode* node = new FavouriteNode;
        node->receiverId = receiverId;
        node->providerId = providerId;
        node->next = NULL;

        if (head == NULL) {
            head = node;
        } else {
            FavouriteNode* current = head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = node;
        }
        count++;
    }

    void remove(int receiverId, int providerId) {
        FavouriteNode* current = head;
        FavouriteNode* previous = NULL;

        while (current != NULL) {
            if (current->receiverId == receiverId && current->providerId == providerId) {
                if (previous == NULL) head = current->next;
                else previous->next = current->next;
                delete current;
                count--;
                return;
            }
            previous = current;
            current = current->next;
        }
    }
};

FavouriteList favourites;

void saveFavourites() {
    ofstream favouriteFile("favourites.txt");
    favouriteFile << favourites.getCount() << endl;
    FavouriteNode* current = favourites.getHead();
    while (current != NULL) {
        favouriteFile << current->receiverId << endl;
        favouriteFile << current->providerId << endl;
        current = current->next;
    }
    favouriteFile.close();
}

void registerReceiver() {
    cout << "\n===== Receiver Registration =====" << endl;
    string name = getText("Enter your name: ");

    string phone = getText("Enter your 10 digit phone number: ");
    while (!isValidPhone(phone)) {
        cout << "A phone number must have exactly 10 digits." << endl;
        phone = getText("Enter your 10 digit phone number: ");
    }
    if (receivers.findByPhone(phone) != NULL) {
        cout << "This phone number is already registered. Please login." << endl;
        return;
    }

    string password = getText("Create a password (at least 4 characters): ");
    while (!isValidPassword(password)) {
        cout << "The password must have at least 4 characters." << endl;
        password = getText("Create a password (at least 4 characters): ");
    }
    int areaNumber = chooseArea();

    ServiceReceiver* receiver = new ServiceReceiver(receivers.nextId(), name, phone, password, areaNumber);
    receivers.add(receiver);
    cout << "\nRegistration successful. You can login now." << endl;
}

ServiceReceiver* loginReceiver() {
    cout << "\n===== Receiver Login =====" << endl;
    for (int attempt = 1; attempt <= 3; attempt++) {
        string phone = getText("Phone number: ");
        string password = getText("Password: ");
        ServiceReceiver* receiver = receivers.findByPhone(phone);
        if (receiver != NULL && receiver->checkLogin(phone, password)) {
            cout << "\nWelcome, " << receiver->getName() << "!" << endl;
            return receiver;
        }
        cout << "Wrong phone number or password. Attempts left: " << (3 - attempt) << endl;
    }
    return NULL;
}

void changeReceiverPassword(ServiceReceiver* receiver) {
    string oldPassword = getText("Enter your old password: ");
    if (!receiver->checkLogin(receiver->getPhone(), oldPassword)) {
        cout << "Old password is wrong." << endl;
        return;
    }
    string newPassword = getText("Enter your new password (at least 4 characters): ");
    if (!isValidPassword(newPassword)) {
        cout << "The password must have at least 4 characters. Password not changed." << endl;
        return;
    }
    receiver->setPassword(newPassword);
    cout << "Password changed." << endl;
}

void manageFavourites(ServiceReceiver* receiver) {
    int choice;
    do {
        cout << "\n----- My Favourite Providers -----" << endl;
        cout << "1. View my favourites" << endl;
        cout << "2. Add a provider to my favourites" << endl;
        cout << "3. Remove a provider from my favourites" << endl;
        cout << "0. Back" << endl;
        choice = getNumber("Enter your choice: ", 0, 3);

        if (choice == 1) {
            int found = 0;
            FavouriteNode* current = favourites.getHead();
            while (current != NULL) {
                if (current->receiverId == receiver->getId()) {
                    showProviderLine(current->providerId);
                    found++;
                }
                current = current->next;
            }
            if (found == 0) cout << "You have no favourite providers yet." << endl;
        } else if (choice == 2) {
            int providerId = getNumber("Enter the provider ID (see the list of providers): ", 1, 100000);
            if (!providerExists(providerId)) cout << "There is no provider with this ID." << endl;
            else if (favourites.isFavourite(receiver->getId(), providerId)) cout << "This provider is already in your favourites." << endl;
            else {
                favourites.add(receiver->getId(), providerId);
                cout << "Added to your favourites." << endl;
            }
        } else if (choice == 3) {
            int providerId = getNumber("Enter the provider ID to remove: ", 1, 100000);
            if (!favourites.isFavourite(receiver->getId(), providerId)) cout << "This provider is not in your favourites." << endl;
            else {
                favourites.remove(receiver->getId(), providerId);
                cout << "Removed from your favourites." << endl;
            }
        }
    } while (choice != 0);
}

void receiverHome(ServiceReceiver* receiver) {
    int choice;
    do {
        cout << "\n========== RECEIVER MENU (" << receiver->getName() << ") ==========" << endl;
        int waiting = countReviewsWaiting(receiver->getId());
        if (waiting > 0) cout << "Notification: " << waiting << " completed booking(s) waiting for your review." << endl;

        cout << "1. See available services" << endl;
        cout << "2. Find a provider and book" << endl;
        cout << "3. Search a provider by name" << endl;
        cout << "4. Nearest providers who are taking customers" << endl;
        cout << "5. My bookings" << endl;
        cout << "6. Cancel a pending booking" << endl;
        cout << "7. Give a review" << endl;
        cout << "8. My favourite providers" << endl;
        cout << "9. My booking summary" << endl;
        cout << "10. My profile" << endl;
        cout << "11. Change my password" << endl;
        cout << "0. Logout" << endl;
        choice = getNumber("Enter your choice: ", 0, 11);

        if (choice == 1) showServices();
        else if (choice == 2) searchProviders(receiver);
        else if (choice == 3) searchByName(receiver);
        else if (choice == 4) showNearestProviders(receiver);
        else if (choice == 5) showMyBookings(receiver);
        else if (choice == 6) cancelBooking(receiver);
        else if (choice == 7) giveReview(receiver);
        else if (choice == 8) manageFavourites(receiver);
        else if (choice == 9) showReceiverSummary(receiver);
        else if (choice == 10) receiver->showProfile();
        else if (choice == 11) changeReceiverPassword(receiver);
    } while (choice != 0);
}

void receiverStart() {
    int choice;
    do {
        cout << "\n----- SERVICE RECEIVER -----" << endl;
        cout << "1. Login" << endl;
        cout << "2. Register as a new receiver" << endl;
        cout << "0. Back" << endl;
        choice = getNumber("Enter your choice: ", 0, 2);

        if (choice == 1) {
            ServiceReceiver* receiver = loginReceiver();
            if (receiver != NULL) receiverHome(receiver);
        } else if (choice == 2) {
            registerReceiver();
        }
    } while (choice != 0);
}

void testReceiverModule() {
    cout << "\n--- Receiver module tests ---" << endl;

    ReceiverList testList;
    check("A new receiver list is empty", testList.getCount() == 0);

    ServiceReceiver* anita = new ServiceReceiver(1, "Anita", "9000011111", "pass1234", 0);
    testList.add(anita);
    check("After adding one receiver the count is 1", testList.getCount() == 1);
    check("Receiver is found by phone number", testList.findByPhone("9000011111") == anita);
    check("Unknown phone number is not found", testList.findByPhone("9999999000") == NULL);
    check("Receiver is found by ID", testList.findById(1) == anita);
    check("Correct phone and password login is accepted", anita->checkLogin("9000011111", "pass1234"));
    check("Wrong password login is rejected", !anita->checkLogin("9000011111", "wrong"));

    FavouriteList testFavourites;
    testFavourites.add(1, 7);
    testFavourites.add(1, 7);
    check("A favourite provider is saved only once", testFavourites.getCount() == 1);
    check("Added provider is a favourite", testFavourites.isFavourite(1, 7));
    testFavourites.remove(1, 7);
    check("Removed provider is not a favourite any more", !testFavourites.isFavourite(1, 7));
    check("Removing from the list reduces the count to 0", testFavourites.getCount() == 0);

    delete anita;
}

class ServiceProvider : public User {
private:
    string service;
    int experience;
    int charge;
    bool available;
    bool acceptingCustomers;
    int ratingTotal;
    int ratingCount;
    string idProof;
    string about;
    string workingDays;
public:
    ServiceProvider(int id, string name, string phone, string password, int areaNumber,
                    string service, int experience, int charge, string idProof)
        : User(id, name, phone, password, areaNumber) {
        this->service = service;
        this->experience = experience;
        this->charge = charge;
        this->idProof = idProof;
        available = true;
        acceptingCustomers = true;
        ratingTotal = 0;
        ratingCount = 0;
        about = "No introduction written yet.";
        workingDays = "1111100";
    }

    string getService() { return service; }
    int getExperience() { return experience; }
    int getCharge() { return charge; }
    bool isAvailable() { return available; }
    bool isAcceptingCustomers() { return acceptingCustomers; }
    string getIdProof() { return idProof; }
    int getRatingTotal() { return ratingTotal; }
    int getRatingCount() { return ratingCount; }
    string getAbout() { return about; }
    string getWorkingDays() { return workingDays; }

    void setService(string newService) { service = newService; }
    void setExperience(int newExperience) { experience = newExperience; }
    void setCharge(int newCharge) { charge = newCharge; }
    void setAvailable(bool value) { available = value; }
    void setAcceptingCustomers(bool value) { acceptingCustomers = value; }
    void setAbout(string newAbout) { about = newAbout; }
    void setWorkingDays(string newDays) { workingDays = newDays; }
    void setRating(int total, int count) {
        ratingTotal = total;
        ratingCount = count;
    }

    void addRating(int rating) {
        ratingTotal = ratingTotal + rating;
        ratingCount++;
    }

    void toggleDay(int dayNumber) {
        if (workingDays[dayNumber] == '1') workingDays[dayNumber] = '0';
        else workingDays[dayNumber] = '1';
    }

    string getWorkingDaysText() {
        string text = "";
        for (int i = 0; i < 7; i++) {
            if (workingDays[i] == '1') text = text + dayNames[i] + " ";
        }
        if (text == "") text = "No working days";
        return text;
    }

    bool canTakeBooking() {
        return available && acceptingCustomers;
    }

    double getRating() {
        if (ratingCount == 0) return 0;
        return (double) ratingTotal / ratingCount;
    }

    string getRole() { return "Service Provider"; }

    void showProfile() {
        cout << "\n----- Provider Profile -----" << endl;
        cout << "ID                  : " << id << endl;
        cout << "Name                : " << name << endl;
        cout << "Phone               : " << phone << endl;
        cout << "Service provided    : " << service << endl;
        cout << "About               : " << about << endl;
        cout << "Experience          : " << experience << " years" << endl;
        cout << "Charge              : Rs " << charge << endl;
        cout << "Area                : " << areaNames[areaNumber] << endl;
        cout << "Working days        : " << getWorkingDaysText() << endl;
        cout << "Available now       : " << (available ? "Yes" : "No") << endl;
        cout << "Accepting customers : " << (acceptingCustomers ? "Yes" : "No") << endl;
        if (ratingCount == 0) cout << "Rating              : No ratings yet" << endl;
        else cout << "Rating              : " << fixed << setprecision(1) << getRating()
                  << " out of 5 (" << ratingCount << " ratings)" << endl;
    }
};

class ProviderNode {
public:
    ServiceProvider* provider;
    ProviderNode* next;
};

class ProviderList {
private:
    ProviderNode* head;
    int count;
public:
    ProviderList() {
        head = NULL;
        count = 0;
    }
    int getCount() { return count; }
    int nextId() { return count + 1; }
    ProviderNode* getHead() { return head; }

    void add(ServiceProvider* provider) {
        ProviderNode* node = new ProviderNode;
        node->provider = provider;
        node->next = NULL;

        if (head == NULL) {
            head = node;
        } else {
            ProviderNode* current = head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = node;
        }
        count++;
    }

    ServiceProvider* findByPhone(string phone) {
        ProviderNode* current = head;
        while (current != NULL) {
            if (current->provider->getPhone() == phone) return current->provider;
            current = current->next;
        }
        return NULL;
    }

    ServiceProvider* findById(int id) {
        ProviderNode* current = head;
        while (current != NULL) {
            if (current->provider->getId() == id) return current->provider;
            current = current->next;
        }
        return NULL;
    }
};

ProviderList providers;

bool providerExists(int providerId) {
    return providers.findById(providerId) != NULL;
}

void showProviderLine(int providerId) {
    ServiceProvider* provider = providers.findById(providerId);
    if (provider == NULL) return;
    cout << "  ID " << provider->getId() << " | " << provider->getName() << " | " << provider->getService()
         << " | " << areaNames[provider->getAreaNumber()] << " | Rs " << provider->getCharge()
         << " | rating " << fixed << setprecision(1) << provider->getRating() << endl;
}

void registerProvider() {
    cout << "\n===== Provider Registration =====" << endl;
    string name = getText("Enter your full name: ");

    string phone = getText("Enter your 10 digit phone number: ");
    while (!isValidPhone(phone)) {
        cout << "A phone number must have exactly 10 digits." << endl;
        phone = getText("Enter your 10 digit phone number: ");
    }
    if (providers.findByPhone(phone) != NULL) {
        cout << "This phone number is already registered. Please login." << endl;
        return;
    }

    string password = getText("Create a password (at least 4 characters): ");
    while (!isValidPassword(password)) {
        cout << "The password must have at least 4 characters." << endl;
        password = getText("Create a password (at least 4 characters): ");
    }
    int areaNumber = chooseArea();
    string service = chooseService();
    int experience = getNumber("Years of experience: ", 0, 50);
    int charge = getNumber("Your charge in Rs per visit: ", 1, 50000);
    string about = getText("Write a short introduction about yourself: ");
    string idProof = getText("Enter your ID proof number: ");

    if (!verifyProvider(idProof, name)) {
        cout << "Registration cancelled." << endl;
        return;
    }

    ServiceProvider* provider = new ServiceProvider(providers.nextId(), name, phone, password,
                                                    areaNumber, service, experience, charge, idProof);
    provider->setAbout(about);
    providers.add(provider);
    cout << "\nRegistration successful. Customers can now find you." << endl;
}

ServiceProvider* loginProvider() {
    cout << "\n===== Provider Login =====" << endl;
    for (int attempt = 1; attempt <= 3; attempt++) {
        string phone = getText("Phone number: ");
        string password = getText("Password: ");
        ServiceProvider* provider = providers.findByPhone(phone);
        if (provider != NULL && provider->checkLogin(phone, password)) {
            cout << "\nWelcome, " << provider->getName() << "!" << endl;
            return provider;
        }
        cout << "Wrong phone number or password. Attempts left: " << (3 - attempt) << endl;
    }
    return NULL;
}

void changeProviderPassword(ServiceProvider* provider) {
    string oldPassword = getText("Enter your old password: ");
    if (!provider->checkLogin(provider->getPhone(), oldPassword)) {
        cout << "Old password is wrong." << endl;
        return;
    }
    string newPassword = getText("Enter your new password (at least 4 characters): ");
    if (!isValidPassword(newPassword)) {
        cout << "The password must have at least 4 characters. Password not changed." << endl;
        return;
    }
    provider->setPassword(newPassword);
    cout << "Password changed." << endl;
}

void changeWorkingDays(ServiceProvider* provider) {
    int day;
    do {
        cout << "\nYour working days: " << provider->getWorkingDaysText() << endl;
        cout << "1 Mon   2 Tue   3 Wed   4 Thu   5 Fri   6 Sat   7 Sun" << endl;
        day = getNumber("Enter a day number to switch it on or off (0 = done): ", 0, 7);
        if (day != 0) provider->toggleDay(day - 1);
    } while (day != 0);
    cout << "Working days saved: " << provider->getWorkingDaysText() << endl;
}

void providerHome(ServiceProvider* provider) {
    int choice;
    do {
        cout << "\n========== PROVIDER MENU (" << provider->getName() << ") ==========" << endl;
        int pending = countPendingRequests(provider->getId());
        if (pending > 0) cout << "Notification: you have " << pending << " pending booking request(s)." << endl;

        cout << "1. View my profile" << endl;
        cout << "2. Change the service I provide" << endl;
        cout << "3. Change my charge" << endl;
        cout << "4. Update my experience" << endl;
        cout << "5. Change my area" << endl;
        cout << "6. Write about myself" << endl;
        cout << "7. Set my working days" << endl;
        cout << "8. Set availability (available / not available)" << endl;
        cout << "9. Accept new customers (yes / no)" << endl;
        cout << "10. View booking requests" << endl;
        cout << "11. Mark an accepted booking as completed" << endl;
        cout << "12. View my reviews" << endl;
        cout << "13. My dashboard (bookings and rating)" << endl;
        cout << "14. Change my password" << endl;
        cout << "0. Logout" << endl;
        choice = getNumber("Enter your choice: ", 0, 14);

        if (choice == 1) {
            provider->showProfile();
        } else if (choice == 2) {
            provider->setService(chooseService());
            cout << "Service changed to " << provider->getService() << "." << endl;
        } else if (choice == 3) {
            provider->setCharge(getNumber("Enter your new charge in Rs: ", 1, 50000));
            cout << "Charge changed." << endl;
        } else if (choice == 4) {
            provider->setExperience(getNumber("Years of experience: ", 0, 50));
            cout << "Experience updated." << endl;
        } else if (choice == 5) {
            provider->setAreaNumber(chooseArea());
            cout << "Area changed to " << areaNames[provider->getAreaNumber()] << "." << endl;
        } else if (choice == 6) {
            provider->setAbout(getText("Write a short introduction about yourself: "));
            cout << "Introduction saved." << endl;
        } else if (choice == 7) {
            changeWorkingDays(provider);
        } else if (choice == 8) {
            provider->setAvailable(!provider->isAvailable());
            if (provider->isAvailable()) cout << "You are now AVAILABLE." << endl;
            else cout << "You are now NOT AVAILABLE." << endl;
        } else if (choice == 9) {
            provider->setAcceptingCustomers(!provider->isAcceptingCustomers());
            if (provider->isAcceptingCustomers()) cout << "You are now ACCEPTING new customers." << endl;
            else cout << "You are NOT accepting new customers now." << endl;
        } else if (choice == 10) {
            showRequests(provider);
        } else if (choice == 11) {
            markCompleted(provider);
        } else if (choice == 12) {
            showProviderReviews(provider->getId());
        } else if (choice == 13) {
            showProviderDashboard(provider);
        } else if (choice == 14) {
            changeProviderPassword(provider);
        }
    } while (choice != 0);
}

void providerStart() {
    int choice;
    do {
        cout << "\n----- SERVICE PROVIDER -----" << endl;
        cout << "1. Login" << endl;
        cout << "2. Register as a new provider" << endl;
        cout << "0. Back" << endl;
        choice = getNumber("Enter your choice: ", 0, 2);

        if (choice == 1) {
            ServiceProvider* provider = loginProvider();
            if (provider != NULL) providerHome(provider);
        } else if (choice == 2) {
            registerProvider();
        }
    } while (choice != 0);
}

void testProviderModule() {
    cout << "\n--- Provider module tests ---" << endl;

    ServiceProvider* ravi = new ServiceProvider(1, "Ravi", "9000022222", "pass1234", 0, "Plumber", 5, 300, "SB0000");
    check("A new provider is available and accepting customers", ravi->canTakeBooking());

    ravi->setAvailable(false);
    check("A provider who is not available cannot take a booking", !ravi->canTakeBooking());
    ravi->setAvailable(true);
    ravi->setAcceptingCustomers(false);
    check("A provider who is not accepting customers cannot take a booking", !ravi->canTakeBooking());

    check("Rating is 0 when there are no ratings", ravi->getRating() == 0);
    ravi->addRating(5);
    ravi->addRating(3);
    check("Average of ratings 5 and 3 is 4", ravi->getRating() == 4.0);
    check("Rating count is 2", ravi->getRatingCount() == 2);

    ravi->setCharge(450);
    check("Charge can be changed", ravi->getCharge() == 450);
    ravi->setService("Electrician");
    check("Service can be changed", ravi->getService() == "Electrician");

    check("Default working days are Monday to Friday", ravi->getWorkingDays() == "1111100");
    ravi->toggleDay(6);
    check("Sunday can be switched on", ravi->getWorkingDays() == "1111101");
    ravi->toggleDay(0);
    check("Monday can be switched off", ravi->getWorkingDays() == "0111101");

    ProviderList testList;
    testList.add(ravi);
    check("Provider is found by ID", testList.findById(1) == ravi);
    check("Provider is found by phone number", testList.findByPhone("9000022222") == ravi);
    check("Unknown ID is not found", testList.findById(99) == NULL);

    delete ravi;
}

int getDistance(int areaA, int areaB) {
    int difference = areaPosition[areaA] - areaPosition[areaB];
    if (difference < 0) difference = -difference;
    return difference;
}

void showServices() {
    cout << "\n----- Services on SkillBridge -----" << endl;
    cout << left << setw(5) << "No" << setw(18) << "Service" << setw(11) << "Providers"
         << setw(18) << "Taking customers" << "Charge (Rs)" << endl;
    cout << "-------------------------------------------------------------" << endl;

    for (int i = 0; i < NUM_SERVICES; i++) {
        int total = 0;
        int takingCustomers = 0;
        int lowest = 0;
        int highest = 0;

        ProviderNode* current = providers.getHead();
        while (current != NULL) {
            ServiceProvider* provider = current->provider;
            if (provider->getService() == serviceNames[i]) {
                total++;
                if (provider->canTakeBooking()) takingCustomers++;
                if (total == 1 || provider->getCharge() < lowest) lowest = provider->getCharge();
                if (total == 1 || provider->getCharge() > highest) highest = provider->getCharge();
            }
            current = current->next;
        }

        cout << left << setw(5) << (i + 1) << setw(18) << serviceNames[i] << setw(11) << total
             << setw(18) << takingCustomers;
        if (total == 0) cout << "-" << endl;
        else cout << lowest << " to " << highest << endl;
    }
}

void showTable(ServiceProvider* found[], int count, ServiceReceiver* receiver) {
    cout << "\n" << left << setw(4) << "No" << setw(20) << "Name" << setw(17) << "Area"
         << setw(10) << "Distance" << setw(9) << "Charge" << setw(8) << "Rating"
         << setw(6) << "Exp" << "Taking customers" << endl;
    cout << "---------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < count; i++) {
        ServiceProvider* provider = found[i];
        int distance = getDistance(receiver->getAreaNumber(), provider->getAreaNumber());

        cout << left << setw(4) << (i + 1) << setw(20) << provider->getName()
             << setw(17) << areaNames[provider->getAreaNumber()]
             << setw(10) << distance << setw(9) << provider->getCharge();
        if (provider->getRatingCount() == 0) cout << setw(8) << "New";
        else cout << setw(8) << fixed << setprecision(1) << provider->getRating();
        cout << setw(6) << provider->getExperience();
        if (provider->canTakeBooking()) cout << "Yes" << endl;
        else cout << "No" << endl;
    }
}

void sortProviders(ServiceProvider* list[], int count, int receiverArea, int sortBy) {
    for (int round = 1; round < count; round++) {
        for (int i = 0; i < count - round; i++) {
            bool swapNeeded = false;

            if (sortBy == 1) {
                int firstDistance = getDistance(receiverArea, list[i]->getAreaNumber());
                int secondDistance = getDistance(receiverArea, list[i + 1]->getAreaNumber());
                if (firstDistance > secondDistance) swapNeeded = true;
            } else if (sortBy == 2) {
                if (list[i]->getCharge() > list[i + 1]->getCharge()) swapNeeded = true;
            } else if (sortBy == 3) {
                if (list[i]->getRating() < list[i + 1]->getRating()) swapNeeded = true;
            } else {
                if (list[i]->getExperience() < list[i + 1]->getExperience()) swapNeeded = true;
            }

            if (swapNeeded) {
                ServiceProvider* temp = list[i];
                list[i] = list[i + 1];
                list[i + 1] = temp;
            }
        }
    }
}

void compareProviders(ServiceProvider* found[], int count, ServiceReceiver* receiver) {
    if (count < 2) {
        cout << "At least two providers are needed to compare." << endl;
        return;
    }
    int firstNumber = getNumber("Enter the first provider number: ", 1, count);
    int secondNumber = getNumber("Enter the second provider number: ", 1, count);
    if (firstNumber == secondNumber) {
        cout << "Please choose two different providers." << endl;
        return;
    }

    ServiceProvider* first = found[firstNumber - 1];
    ServiceProvider* second = found[secondNumber - 1];
    int firstDistance = getDistance(receiver->getAreaNumber(), first->getAreaNumber());
    int secondDistance = getDistance(receiver->getAreaNumber(), second->getAreaNumber());

    cout << "\n" << left << setw(20) << "" << setw(22) << first->getName() << second->getName() << endl;
    cout << "---------------------------------------------------------------" << endl;
    cout << left << setw(20) << "Service" << setw(22) << first->getService() << second->getService() << endl;
    cout << left << setw(20) << "Area" << setw(22) << areaNames[first->getAreaNumber()]
         << areaNames[second->getAreaNumber()] << endl;
    cout << left << setw(20) << "Distance (km)" << setw(22) << firstDistance << secondDistance << endl;
    cout << left << setw(20) << "Charge (Rs)" << setw(22) << first->getCharge() << second->getCharge() << endl;
    cout << left << setw(20) << "Rating" << setw(22) << fixed << setprecision(1) << first->getRating()
         << second->getRating() << endl;
    cout << left << setw(20) << "Experience (years)" << setw(22) << first->getExperience() << second->getExperience() << endl;
    cout << left << setw(20) << "Taking customers" << setw(22) << (first->canTakeBooking() ? "Yes" : "No")
         << (second->canTakeBooking() ? "Yes" : "No") << endl;

    cout << "\nNearer provider : " << (firstDistance <= secondDistance ? first->getName() : second->getName()) << endl;
    cout << "Cheaper provider: " << (first->getCharge() <= second->getCharge() ? first->getName() : second->getName()) << endl;
    cout << "Better rated    : " << (first->getRating() >= second->getRating() ? first->getName() : second->getName()) << endl;
}

void chooseProvider(ServiceProvider* found[], int count, ServiceReceiver* receiver) {
    int number = getNumber("Enter provider number (0 = cancel): ", 0, count);
    if (number == 0) return;

    ServiceProvider* provider = found[number - 1];
    provider->showProfile();
    cout << "Distance            : "
         << getDistance(receiver->getAreaNumber(), provider->getAreaNumber()) << " km from you" << endl;
    showProviderReviews(provider->getId());

    if (favourites.isFavourite(receiver->getId(), provider->getId())) {
        cout << "\nThis provider is in your favourites." << endl;
    } else if (askYesNo("\nAdd this provider to your favourites? (y/n): ")) {
        favourites.add(receiver->getId(), provider->getId());
        cout << "Added to your favourites." << endl;
    }

    if (!provider->canTakeBooking()) {
        cout << "\nThis provider is not taking customers right now." << endl;
        return;
    }
    if (askYesNo("\nSend a booking request to this provider? (y/n): ")) {
        sendRequest(receiver, provider);
    }
}

void searchProviders(ServiceReceiver* receiver) {
    cout << "\n--- Find a Provider ---" << endl;
    string service = chooseService();
    int radius = getNumber("Search radius in km (1 to 30): ", 1, 30);
    int maxCharge = getNumber("Maximum charge in Rs (0 = no limit): ", 0, 50000);
    if (maxCharge == 0) maxCharge = 50000;
    bool onlyTaking = askYesNo("Show only providers who are taking customers? (y/n): ");

    ServiceProvider* found[MAX_FOUND];
    int count = 0;

    ProviderNode* current = providers.getHead();
    while (current != NULL) {
        ServiceProvider* provider = current->provider;
        int distance = getDistance(receiver->getAreaNumber(), provider->getAreaNumber());

        bool matches = true;
        if (provider->getService() != service) matches = false;
        if (distance > radius) matches = false;
        if (provider->getCharge() > maxCharge) matches = false;
        if (onlyTaking && !provider->canTakeBooking()) matches = false;

        if (matches && count < MAX_FOUND) {
            found[count] = provider;
            count++;
        }
        current = current->next;
    }

    if (count == 0) {
        cout << "\nNo provider found. Try a bigger radius or a higher charge." << endl;
        return;
    }

    int choice;
    do {
        showTable(found, count, receiver);
        cout << "\n1. Sort by distance (nearest first)" << endl;
        cout << "2. Sort by charge (cheapest first)" << endl;
        cout << "3. Sort by rating (best first)" << endl;
        cout << "4. Sort by experience (most experienced first)" << endl;
        cout << "5. Smart ranking (rating + distance + charge)" << endl;
        cout << "6. Compare two providers" << endl;
        cout << "7. Select a provider" << endl;
        cout << "0. Back" << endl;
        choice = getNumber("Enter your choice: ", 0, 7);

        if (choice >= 1 && choice <= 4) sortProviders(found, count, receiver->getAreaNumber(), choice);
        else if (choice == 5) rankProviders(found, count, receiver);
        else if (choice == 6) compareProviders(found, count, receiver);
        else if (choice == 7) chooseProvider(found, count, receiver);
    } while (choice != 0);
}

void searchByName(ServiceReceiver* receiver) {
    string key = getText("Enter the provider's name (or a part of it): ");

    ServiceProvider* found[MAX_FOUND];
    int count = 0;

    ProviderNode* current = providers.getHead();
    while (current != NULL && count < MAX_FOUND) {
        if (containsText(current->provider->getName(), key)) {
            found[count] = current->provider;
            count++;
        }
        current = current->next;
    }

    if (count == 0) {
        cout << "No provider found with this name." << endl;
        return;
    }
    showTable(found, count, receiver);
    chooseProvider(found, count, receiver);
}

void showNearestProviders(ServiceReceiver* receiver) {
    ServiceProvider* found[MAX_FOUND];
    int count = 0;

    ProviderNode* current = providers.getHead();
    while (current != NULL && count < MAX_FOUND) {
        if (current->provider->canTakeBooking()) {
            found[count] = current->provider;
            count++;
        }
        current = current->next;
    }
    if (count == 0) {
        cout << "No provider is taking customers right now." << endl;
        return;
    }

    sortProviders(found, count, receiver->getAreaNumber(), 1);
    int shown = 3;
    if (count < 3) shown = count;

    cout << "\n--- The " << shown << " nearest providers who are taking customers ---" << endl;
    showTable(found, shown, receiver);
    chooseProvider(found, shown, receiver);
}

void testSearchModule() {
    cout << "\n--- Search module tests ---" << endl;

    check("Distance from Sector 1 to City Center is 5 km", getDistance(0, 2) == 5);
    check("Distance is the same in both directions", getDistance(2, 0) == getDistance(0, 2));
    check("Distance from an area to itself is 0", getDistance(3, 3) == 0);
    check("Name search ignores capital letters", containsText("Ramesh Kumar", "MESH"));
    check("Name search says no when the text is not there", !containsText("Ramesh Kumar", "xyz"));

    ServiceProvider* a = new ServiceProvider(1, "A", "9000000001", "x", 5, "Plumber", 3, 300, "x");
    ServiceProvider* b = new ServiceProvider(2, "B", "9000000002", "x", 1, "Plumber", 9, 100, "x");
    ServiceProvider* c = new ServiceProvider(3, "C", "9000000003", "x", 3, "Plumber", 6, 200, "x");
    a->setRating(8, 2);
    b->setRating(10, 2);
    c->setRating(6, 2);

    ServiceProvider* list[3];

    list[0] = a; list[1] = b; list[2] = c;
    sortProviders(list, 3, 0, 1);
    check("Sort by distance: nearest provider is first", list[0] == b);
    check("Sort by distance: farthest provider is last", list[2] == a);

    list[0] = a; list[1] = b; list[2] = c;
    sortProviders(list, 3, 0, 2);
    check("Sort by charge: cheapest provider is first", list[0] == b);
    check("Sort by charge: costliest provider is last", list[2] == a);

    list[0] = a; list[1] = b; list[2] = c;
    sortProviders(list, 3, 0, 3);
    check("Sort by rating: best rated provider is first", list[0] == b);
    check("Sort by rating: lowest rated provider is last", list[2] == c);

    list[0] = a; list[1] = b; list[2] = c;
    sortProviders(list, 3, 0, 4);
    check("Sort by experience: most experienced provider is first", list[0] == b);
    check("Sort by experience: least experienced provider is last", list[2] == a);

    delete a;
    delete b;
    delete c;
}

class Booking {
private:
    int id;
    int receiverId;
    int providerId;
    string receiverName;
    string providerName;
    string service;
    string day;
    string message;
    string reply;
    int status;
    bool reviewed;
    int fee;
public:
    Booking(int id, int receiverId, int providerId, string receiverName,
            string providerName, string service, string day, string message, int fee) {
        this->id = id;
        this->receiverId = receiverId;
        this->providerId = providerId;
        this->receiverName = receiverName;
        this->providerName = providerName;
        this->service = service;
        this->day = day;
        this->message = message;
        this->fee = fee;
        reply = "";
        status = PENDING;
        reviewed = false;
    }
    int getId() { return id; }
    int getReceiverId() { return receiverId; }
    int getProviderId() { return providerId; }
    string getReceiverName() { return receiverName; }
    string getProviderName() { return providerName; }
    string getService() { return service; }
    string getDay() { return day; }
    string getMessage() { return message; }
    string getReply() { return reply; }
    int getStatus() { return status; }
    bool isReviewed() { return reviewed; }
    int getFee() { return fee; }
    void setStatus(int newStatus) { status = newStatus; }
    void setReply(string newReply) { reply = newReply; }
    void setReviewed(bool value) { reviewed = value; }

    string getStatusText() {
        if (status == PENDING) return "Pending";
        if (status == ACCEPTED) return "Accepted";
        if (status == REJECTED) return "Rejected";
        if (status == COMPLETED) return "Completed";
        return "Cancelled";
    }

    void show() {
        cout << "  Booking " << id << " | " << service << " | Receiver: " << receiverName
             << " | Provider: " << providerName << " | When: " << day
             << " | Fee: Rs " << fee << " | Status: " << getStatusText() << endl;
        cout << "      Receiver's message: " << message << endl;
        if (reply != "") cout << "      Provider's reply  : " << reply << endl;
    }
};

class BookingNode {
public:
    Booking* booking;
    BookingNode* next;
};

class BookingList {
private:
    BookingNode* head;
    int count;
public:
    BookingList() {
        head = NULL;
        count = 0;
    }
    int getCount() { return count; }
    int nextId() { return count + 1; }
    BookingNode* getHead() { return head; }

    void add(Booking* booking) {
        BookingNode* node = new BookingNode;
        node->booking = booking;
        node->next = NULL;

        if (head == NULL) {
            head = node;
        } else {
            BookingNode* current = head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = node;
        }
        count++;
    }

    Booking* findById(int id) {
        BookingNode* current = head;
        while (current != NULL) {
            if (current->booking->getId() == id) return current->booking;
            current = current->next;
        }
        return NULL;
    }
};

BookingList bookings;

class QueueNode {
public:
    Booking* booking;
    QueueNode* next;
};

class BookingQueue {
private:
    QueueNode* front;
    QueueNode* rear;
public:
    BookingQueue() {
        front = NULL;
        rear = NULL;
    }
    QueueNode* getFront() { return front; }
    bool isEmpty() { return front == NULL; }

    void enqueue(Booking* booking) {
        QueueNode* node = new QueueNode;
        node->booking = booking;
        node->next = NULL;

        if (rear == NULL) {
            front = node;
            rear = node;
        } else {
            rear->next = node;
            rear = node;
        }
    }

    Booking* dequeue() {
        if (front == NULL) return NULL;

        QueueNode* node = front;
        Booking* booking = node->booking;
        front = front->next;
        if (front == NULL) rear = NULL;
        delete node;
        return booking;
    }

    void removeById(int id) {
        QueueNode* current = front;
        QueueNode* previous = NULL;

        while (current != NULL) {
            if (current->booking->getId() == id) {
                if (previous == NULL) front = current->next;
                else previous->next = current->next;
                if (current == rear) rear = previous;
                delete current;
                return;
            }
            previous = current;
            current = current->next;
        }
    }
};

BookingQueue pendingRequests;

class PriorityNode {
public:
    ServiceProvider* provider;
    double score;
    PriorityNode* next;
};

class PriorityQueue {
private:
    PriorityNode* head;
public:
    PriorityQueue() { head = NULL; }
    bool isEmpty() { return head == NULL; }
    double getFrontScore() { return head->score; }

    void insert(ServiceProvider* provider, double score) {
        PriorityNode* node = new PriorityNode;
        node->provider = provider;
        node->score = score;
        node->next = NULL;

        if (head == NULL || score > head->score) {
            node->next = head;
            head = node;
            return;
        }
        PriorityNode* current = head;
        while (current->next != NULL && current->next->score >= score) {
            current = current->next;
        }
        node->next = current->next;
        current->next = node;
    }

    ServiceProvider* extractBest() {
        if (head == NULL) return NULL;

        PriorityNode* node = head;
        ServiceProvider* provider = node->provider;
        head = head->next;
        delete node;
        return provider;
    }
};

double calculateScore(ServiceProvider* provider, int distance) {
    double ratingPoints = provider->getRating() * 20;
    double nearnessPoints = (20 - distance) * 2;
    double cheapness = 1000 - provider->getCharge();
    if (cheapness < 0) cheapness = 0;
    double cheapnessPoints = cheapness / 20;
    return ratingPoints + nearnessPoints + cheapnessPoints;
}

void rankProviders(ServiceProvider* found[], int count, ServiceReceiver* receiver) {
    PriorityQueue queue;
    for (int i = 0; i < count; i++) {
        int distance = getDistance(receiver->getAreaNumber(), found[i]->getAreaNumber());
        queue.insert(found[i], calculateScore(found[i], distance));
    }

    cout << "\nProviders ranked by score:" << endl;
    for (int i = 0; i < count; i++) {
        double score = queue.getFrontScore();
        found[i] = queue.extractBest();
        cout << "  Rank " << (i + 1) << " : " << found[i]->getName()
             << "  (score " << fixed << setprecision(1) << score << ")" << endl;
    }
}

void sendRequest(ServiceReceiver* receiver, ServiceProvider* provider) {

    BookingNode* current = bookings.getHead();
    while (current != NULL) {
        Booking* old = current->booking;
        if (old->getReceiverId() == receiver->getId() && old->getProviderId() == provider->getId()
            && old->getStatus() == PENDING) {
            cout << "\nYou already have a pending request with this provider." << endl;
            return;
        }
        current = current->next;
    }

    cout << "This provider works on: " << provider->getWorkingDaysText() << endl;
    string day = getText("Preferred day and time (for example Monday 5 PM): ");
    string message = getText("Write a short message for the provider: ");

    Booking* booking = new Booking(bookings.nextId(), receiver->getId(), provider->getId(),
                                   receiver->getName(), provider->getName(),
                                   provider->getService(), day, message, provider->getCharge());
    bookings.add(booking);
    pendingRequests.enqueue(booking);

    cout << "\nBooking " << booking->getId() << " sent to " << provider->getName()
         << ". Fee: Rs " << booking->getFee() << ". Status: Pending." << endl;
}

int countPendingRequests(int providerId) {
    int total = 0;
    QueueNode* current = pendingRequests.getFront();
    while (current != NULL) {
        if (current->booking->getProviderId() == providerId) total++;
        current = current->next;
    }
    return total;
}

void showRequests(ServiceProvider* provider) {
    cout << "\n--- Pending Requests (oldest first) ---" << endl;
    int found = 0;

    QueueNode* current = pendingRequests.getFront();
    while (current != NULL) {
        if (current->booking->getProviderId() == provider->getId()) {
            current->booking->show();
            found++;
        }
        current = current->next;
    }
    if (found == 0) {
        cout << "You have no pending requests." << endl;
        return;
    }

    int bookingId = getNumber("\nEnter the booking number to answer (0 = back): ", 0, 100000);
    if (bookingId == 0) return;

    Booking* booking = bookings.findById(bookingId);
    if (booking == NULL || booking->getProviderId() != provider->getId() || booking->getStatus() != PENDING) {
        cout << "Wrong booking number." << endl;
        return;
    }

    cout << "1. Accept" << endl;
    cout << "2. Reject" << endl;
    int choice = getNumber("Enter your choice (0 = back): ", 0, 2);
    if (choice == 0) return;

    booking->setReply(getText("Write a short reply for the receiver: "));
    if (choice == 1) {
        booking->setStatus(ACCEPTED);
        cout << "Booking " << bookingId << " accepted." << endl;
    } else {
        booking->setStatus(REJECTED);
        cout << "Booking " << bookingId << " rejected." << endl;
    }
    pendingRequests.removeById(bookingId);
}

void markCompleted(ServiceProvider* provider) {
    cout << "\n--- Accepted Bookings ---" << endl;
    int found = 0;

    BookingNode* current = bookings.getHead();
    while (current != NULL) {
        Booking* booking = current->booking;
        if (booking->getProviderId() == provider->getId() && booking->getStatus() == ACCEPTED) {
            booking->show();
            found++;
        }
        current = current->next;
    }
    if (found == 0) {
        cout << "You have no accepted bookings." << endl;
        return;
    }

    int bookingId = getNumber("\nEnter the booking number that is completed (0 = back): ", 0, 100000);
    if (bookingId == 0) return;

    Booking* booking = bookings.findById(bookingId);
    if (booking == NULL || booking->getProviderId() != provider->getId() || booking->getStatus() != ACCEPTED) {
        cout << "Wrong booking number." << endl;
        return;
    }
    booking->setStatus(COMPLETED);
    cout << "Booking " << bookingId << " is completed. The receiver can now review you." << endl;
}

void showMyBookings(ServiceReceiver* receiver) {
    cout << "\nShow which bookings?" << endl;
    cout << "0. All   1. Pending   2. Accepted   3. Rejected   4. Completed   5. Cancelled" << endl;
    int filter = getNumber("Enter your choice: ", 0, 5);

    cout << "\n--- My Bookings ---" << endl;
    int found = 0;

    BookingNode* current = bookings.getHead();
    while (current != NULL) {
        Booking* booking = current->booking;
        bool mine = (booking->getReceiverId() == receiver->getId());
        bool statusMatches = (filter == 0 || booking->getStatus() == filter - 1);
        if (mine && statusMatches) {
            booking->show();
            found++;
        }
        current = current->next;
    }
    if (found == 0) cout << "No bookings found." << endl;
}

void cancelBooking(ServiceReceiver* receiver) {
    cout << "\n--- Your Pending Bookings ---" << endl;
    int found = 0;

    BookingNode* current = bookings.getHead();
    while (current != NULL) {
        Booking* booking = current->booking;
        if (booking->getReceiverId() == receiver->getId() && booking->getStatus() == PENDING) {
            booking->show();
            found++;
        }
        current = current->next;
    }
    if (found == 0) {
        cout << "You have no pending bookings." << endl;
        return;
    }

    int bookingId = getNumber("\nEnter the booking number to cancel (0 = back): ", 0, 100000);
    if (bookingId == 0) return;

    Booking* booking = bookings.findById(bookingId);
    if (booking == NULL || booking->getReceiverId() != receiver->getId() || booking->getStatus() != PENDING) {
        cout << "Wrong booking number." << endl;
        return;
    }
    booking->setStatus(CANCELLED);
    pendingRequests.removeById(bookingId);
    cout << "Booking " << bookingId << " cancelled." << endl;
}

void showProviderDashboard(ServiceProvider* provider) {
    int countByStatus[5] = {0, 0, 0, 0, 0};

    BookingNode* current = bookings.getHead();
    while (current != NULL) {
        Booking* booking = current->booking;
        if (booking->getProviderId() == provider->getId()) {
            countByStatus[booking->getStatus()]++;
        }
        current = current->next;
    }

    cout << "\n========== MY DASHBOARD ==========" << endl;
    cout << "Pending requests : " << countByStatus[PENDING] << endl;
    cout << "Accepted bookings: " << countByStatus[ACCEPTED] << endl;
    cout << "Rejected bookings: " << countByStatus[REJECTED] << endl;
    cout << "Completed jobs   : " << countByStatus[COMPLETED] << endl;
    cout << "Cancelled        : " << countByStatus[CANCELLED] << endl;
    if (provider->getRatingCount() == 0) cout << "Average rating   : No ratings yet" << endl;
    else cout << "Average rating   : " << fixed << setprecision(1) << provider->getRating()
              << " out of 5 (" << provider->getRatingCount() << " ratings)" << endl;
}

void showReceiverSummary(ServiceReceiver* receiver) {
    int countByStatus[5] = {0, 0, 0, 0, 0};

    BookingNode* current = bookings.getHead();
    while (current != NULL) {
        Booking* booking = current->booking;
        if (booking->getReceiverId() == receiver->getId()) {
            countByStatus[booking->getStatus()]++;
        }
        current = current->next;
    }

    cout << "\n========== MY BOOKING SUMMARY ==========" << endl;
    cout << "Pending   : " << countByStatus[PENDING] << endl;
    cout << "Accepted  : " << countByStatus[ACCEPTED] << endl;
    cout << "Rejected  : " << countByStatus[REJECTED] << endl;
    cout << "Completed : " << countByStatus[COMPLETED] << endl;
    cout << "Cancelled : " << countByStatus[CANCELLED] << endl;
}

void testBookingModule() {
    cout << "\n--- Booking module tests ---" << endl;

    Booking* booking1 = new Booking(1, 1, 1, "R", "P", "Plumber", "Monday", "hi", 300);
    Booking* booking2 = new Booking(2, 1, 1, "R", "P", "Plumber", "Tuesday", "hi", 300);
    Booking* booking3 = new Booking(3, 1, 1, "R", "P", "Plumber", "Friday", "hi", 300);

    check("A new booking is Pending", booking1->getStatusText() == "Pending");
    check("The fee of a booking is stored", booking1->getFee() == 300);
    booking1->setStatus(ACCEPTED);
    check("Status text changes to Accepted", booking1->getStatusText() == "Accepted");
    booking1->setStatus(CANCELLED);
    check("Status text changes to Cancelled", booking1->getStatusText() == "Cancelled");

    BookingQueue testQueue;
    check("A new queue is empty", testQueue.isEmpty());
    testQueue.enqueue(booking1);
    testQueue.enqueue(booking2);
    testQueue.enqueue(booking3);
    check("Queue is not empty after enqueue", !testQueue.isEmpty());
    check("Queue gives the first request first (FIFO)", testQueue.dequeue()->getId() == 1);
    testQueue.removeById(3);
    check("A request in the middle or end can be removed", testQueue.getFront()->booking->getId() == 2);
    check("The remaining request is booking 2", testQueue.dequeue()->getId() == 2);
    check("Queue is empty after all requests leave", testQueue.isEmpty());

    ServiceProvider* good = new ServiceProvider(1, "Good", "9000000001", "x", 0, "Plumber", 5, 100, "x");
    ServiceProvider* middle = new ServiceProvider(2, "Middle", "9000000002", "x", 0, "Plumber", 5, 200, "x");
    ServiceProvider* weak = new ServiceProvider(3, "Weak", "9000000003", "x", 0, "Plumber", 5, 900, "x");
    good->setRating(10, 2);
    middle->setRating(8, 2);
    weak->setRating(4, 2);

    check("A better provider gets a higher score", calculateScore(good, 2) > calculateScore(weak, 7));

    PriorityQueue testPriority;
    testPriority.insert(weak, 50);
    testPriority.insert(good, 90);
    testPriority.insert(middle, 70);
    check("Priority queue: the highest score (90) is first", testPriority.getFrontScore() == 90);
    check("Priority queue gives the best provider first", testPriority.extractBest() == good);
    check("Priority queue: the next highest score is 70", testPriority.getFrontScore() == 70);
    check("Priority queue gives the second best provider next", testPriority.extractBest() == middle);

    delete booking1;
    delete booking2;
    delete booking3;
    delete good;
    delete middle;
    delete weak;
}

void createProviders() {
    string firstNames[10] = {"Ramesh", "Sunita", "Anil", "Pooja", "Imran", "Meena", "Vikas", "Neha", "Rakesh", "Farah"};
    string lastNames[10] = {"Kumar", "Sharma", "Singh", "Verma", "Gupta", "Yadav", "Rawat", "Joshi", "Mishra", "Chauhan"};

    for (int i = 0; i < 100; i++) {
        string name = firstNames[i % 10] + " " + lastNames[i / 10];
        string phone = to_string(9000000001L + i);
        int areaNumber = (i + i / 9) % NUM_AREAS;
        string service = serviceNames[i % NUM_SERVICES];
        int experience = 2 + i % 12;
        int charge = 250 + (i % 8) * 75;
        string idProof = "SB" + to_string(1001 + i);

        ServiceProvider* provider = new ServiceProvider(providers.nextId(), name, phone, "1234",
                                                        areaNumber, service, experience, charge, idProof);

        provider->setAvailable(i % 7 != 0);
        provider->setAcceptingCustomers(i % 11 != 0);
        provider->setAbout(service + " with " + to_string(experience) + " years of experience in " + areaNames[areaNumber] + ".");

        if (i % 3 == 0) provider->setWorkingDays("1111111");
        else if (i % 3 == 1) provider->setWorkingDays("1111110");
        else provider->setWorkingDays("1111100");

        providers.add(provider);
    }
}

void createReceivers() {
    string firstNames[10] = {"Aarav", "Priya", "Rohan", "Ananya", "Karan", "Isha", "Aditya", "Kavya", "Manish", "Riya"};
    string lastNames[10] = {"Mehta", "Das", "Nair", "Patel", "Reddy", "Iyer", "Khan", "Shah", "Bose", "Kapoor"};

    for (int i = 0; i < 100; i++) {
        string name = firstNames[i % 10] + " " + lastNames[i / 10];
        string phone = to_string(9100000001L + i);
        int areaNumber = i % NUM_AREAS;

        ServiceReceiver* receiver = new ServiceReceiver(receivers.nextId(), name, phone, "1234", areaNumber);
        receivers.add(receiver);
    }
}

string bookingDays[7] = {"Monday 10 AM", "Tuesday 11 AM", "Wednesday 3 PM", "Thursday 5 PM",
                         "Friday 4 PM", "Saturday 11 AM", "Sunday 9 AM"};
string requestMessages[5] = {"Please come on time.", "I need this work done urgently.",
                             "Please bring all the tools.", "Call me before you come.",
                             "The work is at my home."};
string goodComments[5] = {"Quick and neat work.", "Very polite and on time.", "Good service, will book again.",
                          "Reasonable charges and good work.", "Did the job perfectly."};
string averageComments[3] = {"Work was fine.", "Okay service, a little late.", "Average work."};

int pickReceiver(int providerId, int number) {
    return (providerId * 7 + number * 13) % receivers.getCount() + 1;
}

Booking* addSampleBooking(int receiverId, int providerId, int status, int number) {
    ServiceReceiver* receiver = receivers.findById(receiverId);
    ServiceProvider* provider = providers.findById(providerId);

    Booking* booking = new Booking(bookings.nextId(), receiverId, providerId, receiver->getName(),
                                   provider->getName(), provider->getService(), bookingDays[number % 7],
                                   requestMessages[number % 5], provider->getCharge());
    booking->setStatus(status);
    if (status == ACCEPTED || status == COMPLETED) booking->setReply("Confirmed. I will be there on time.");
    if (status == REJECTED) booking->setReply("Sorry, I am not free on that day.");
    bookings.add(booking);
    if (status == PENDING) pendingRequests.enqueue(booking);
    return booking;
}

void createBookings() {
    for (int providerId = 1; providerId <= providers.getCount(); providerId++) {
        ServiceProvider* provider = providers.findById(providerId);
        int number = 0;

        int pendingCount = 6 + providerId % 2;
        for (int i = 0; i < pendingCount; i++) {
            addSampleBooking(pickReceiver(providerId, number), providerId, PENDING, number);
            number++;
        }

        int acceptedCount = 3 + providerId % 2;
        for (int i = 0; i < acceptedCount; i++) {
            addSampleBooking(pickReceiver(providerId, number), providerId, ACCEPTED, number);
            number++;
        }

        int rejectedCount = 1 + providerId % 2;
        for (int i = 0; i < rejectedCount; i++) {
            addSampleBooking(pickReceiver(providerId, number), providerId, REJECTED, number);
            number++;
        }

        int cancelledCount = 1 + (providerId / 2) % 2;
        for (int i = 0; i < cancelledCount; i++) {
            addSampleBooking(pickReceiver(providerId, number), providerId, CANCELLED, number);
            number++;
        }

        int reviewedCount = 3 + (providerId / 4) % 3;
        int quality = 3 + (providerId * 3 / 4) % 3;
        for (int i = 0; i < 6; i++) {
            Booking* done = addSampleBooking(pickReceiver(providerId, number), providerId, COMPLETED, number);
            number++;

            if (i < reviewedCount) {
                int rating = quality;
                if ((i + providerId) % 4 == 0) rating = rating - 1;

                string comment;
                if (rating >= 4) comment = goodComments[(i + providerId) % 5];
                else comment = averageComments[(i + providerId) % 3];

                addReview(done->getId(), providerId, done->getReceiverName(), rating, comment);
                provider->addRating(rating);
                done->setReviewed(true);
            }
        }
    }

    for (int receiverId = 1; receiverId <= receivers.getCount(); receiverId++) {
        favourites.add(receiverId, (receiverId * 3) % providers.getCount() + 1);
        favourites.add(receiverId, (receiverId * 3 + 37) % providers.getCount() + 1);
    }
}

void showAllProviders() {
    cout << "\n===== ALL SERVICE PROVIDERS (" << providers.getCount() << ") =====" << endl;
    cout << left << setw(4) << "ID" << setw(18) << "Name" << setw(12) << "Phone" << setw(17) << "Service"
         << setw(16) << "Area" << setw(8) << "Charge" << setw(8) << "Rating" << setw(11) << "Available"
         << "Accepting" << endl;
    cout << "-------------------------------------------------------------------------------------------------" << endl;

    ProviderNode* current = providers.getHead();
    while (current != NULL) {
        ServiceProvider* provider = current->provider;
        cout << left << setw(4) << provider->getId() << setw(18) << provider->getName()
             << setw(12) << provider->getPhone() << setw(17) << provider->getService()
             << setw(16) << areaNames[provider->getAreaNumber()] << setw(8) << provider->getCharge();
        cout << setw(8) << fixed << setprecision(1) << provider->getRating();
        cout << setw(11) << (provider->isAvailable() ? "Yes" : "No")
             << (provider->isAcceptingCustomers() ? "Yes" : "No") << endl;
        current = current->next;
    }

    int id = getNumber("\nEnter a provider ID to see the full profile (0 = back): ", 0, providers.getCount());
    if (id != 0) {
        User* user = providers.findById(id);
        user->showProfile();
    }
}

void showAllReceivers() {
    cout << "\n===== ALL SERVICE RECEIVERS (" << receivers.getCount() << ") =====" << endl;
    cout << left << setw(4) << "ID" << setw(18) << "Name" << setw(12) << "Phone" << "Area" << endl;
    cout << "-------------------------------------------------------" << endl;

    ReceiverNode* current = receivers.getHead();
    while (current != NULL) {
        ServiceReceiver* receiver = current->receiver;
        cout << left << setw(4) << receiver->getId() << setw(18) << receiver->getName()
             << setw(12) << receiver->getPhone() << areaNames[receiver->getAreaNumber()] << endl;
        current = current->next;
    }

    int id = getNumber("\nEnter a receiver ID to see the full profile (0 = back): ", 0, receivers.getCount());
    if (id != 0) {
        User* user = receivers.findById(id);
        user->showProfile();
    }
}

void showBookingsOfProvider() {
    string key = getText("Enter the provider's name (or a part of it): ");

    ServiceProvider* matches[MAX_FOUND];
    int count = 0;
    ProviderNode* providerNode = providers.getHead();
    while (providerNode != NULL && count < MAX_FOUND) {
        if (containsText(providerNode->provider->getName(), key)) {
            matches[count] = providerNode->provider;
            count++;
        }
        providerNode = providerNode->next;
    }
    if (count == 0) {
        cout << "No provider found with this name." << endl;
        return;
    }

    cout << "\nProviders found:" << endl;
    for (int i = 0; i < count; i++) {
        cout << "  " << (i + 1) << ". " << matches[i]->getName() << " | " << matches[i]->getService()
             << " | " << areaNames[matches[i]->getAreaNumber()] << endl;
    }
    int number = getNumber("Choose a provider number (0 = back): ", 0, count);
    if (number == 0) return;
    ServiceProvider* provider = matches[number - 1];

    int countByStatus[5] = {0, 0, 0, 0, 0};
    BookingNode* bookingNode = bookings.getHead();
    while (bookingNode != NULL) {
        if (bookingNode->booking->getProviderId() == provider->getId()) {
            countByStatus[bookingNode->booking->getStatus()]++;
        }
        bookingNode = bookingNode->next;
    }

    cout << "\n===== BOOKINGS OF " << provider->getName() << " (" << provider->getService() << ") =====" << endl;
    cout << "Pending " << countByStatus[PENDING] << " | Accepted " << countByStatus[ACCEPTED]
         << " | Rejected " << countByStatus[REJECTED] << " | Completed " << countByStatus[COMPLETED]
         << " | Cancelled " << countByStatus[CANCELLED] << endl;

    cout << "\nShow which bookings?" << endl;
    cout << "0. Back   1. Pending   2. Accepted   3. Rejected   4. Completed   5. Cancelled   6. All" << endl;
    int filter = getNumber("Enter your choice: ", 0, 6);
    if (filter == 0) return;

    cout << endl;
    bookingNode = bookings.getHead();
    while (bookingNode != NULL) {
        Booking* booking = bookingNode->booking;
        bool statusMatches = (filter == 6 || booking->getStatus() == filter - 1);
        if (booking->getProviderId() == provider->getId() && statusMatches) {
            booking->show();
        }
        bookingNode = bookingNode->next;
    }
}

void saveToFiles() {
    ofstream providerFile("providers.txt");
    providerFile << providers.getCount() << endl;
    ProviderNode* providerNode = providers.getHead();
    while (providerNode != NULL) {
        ServiceProvider* provider = providerNode->provider;
        providerFile << provider->getId() << endl;
        providerFile << provider->getName() << endl;
        providerFile << provider->getPhone() << endl;
        providerFile << provider->getPassword() << endl;
        providerFile << provider->getAreaNumber() << endl;
        providerFile << provider->getService() << endl;
        providerFile << provider->getExperience() << endl;
        providerFile << provider->getCharge() << endl;
        providerFile << provider->isAvailable() << endl;
        providerFile << provider->isAcceptingCustomers() << endl;
        providerFile << provider->getRatingTotal() << endl;
        providerFile << provider->getRatingCount() << endl;
        providerFile << provider->getIdProof() << endl;
        providerFile << provider->getAbout() << endl;
        providerFile << provider->getWorkingDays() << endl;
        providerNode = providerNode->next;
    }
    providerFile.close();

    ofstream receiverFile("receivers.txt");
    receiverFile << receivers.getCount() << endl;
    ReceiverNode* receiverNode = receivers.getHead();
    while (receiverNode != NULL) {
        ServiceReceiver* receiver = receiverNode->receiver;
        receiverFile << receiver->getId() << endl;
        receiverFile << receiver->getName() << endl;
        receiverFile << receiver->getPhone() << endl;
        receiverFile << receiver->getPassword() << endl;
        receiverFile << receiver->getAreaNumber() << endl;
        receiverNode = receiverNode->next;
    }
    receiverFile.close();

    ofstream bookingFile("bookings.txt");
    bookingFile << bookings.getCount() << endl;
    BookingNode* bookingNode = bookings.getHead();
    while (bookingNode != NULL) {
        Booking* booking = bookingNode->booking;
        bookingFile << booking->getId() << endl;
        bookingFile << booking->getReceiverId() << endl;
        bookingFile << booking->getProviderId() << endl;
        bookingFile << booking->getReceiverName() << endl;
        bookingFile << booking->getProviderName() << endl;
        bookingFile << booking->getService() << endl;
        bookingFile << booking->getDay() << endl;
        bookingFile << booking->getMessage() << endl;
        bookingFile << booking->getReply() << endl;
        bookingFile << booking->getStatus() << endl;
        bookingFile << booking->isReviewed() << endl;
        bookingFile << booking->getFee() << endl;
        bookingNode = bookingNode->next;
    }
    bookingFile.close();

    saveReviews();
    saveFavourites();
}

void testFileModule() {
    cout << "\n--- File module tests ---" << endl;

    ofstream tempFile("test_temp.txt");
    tempFile << 42 << endl;
    tempFile << "Hello SkillBridge" << endl;
    tempFile << 7 << endl;
    tempFile.close();

    ifstream readFile("test_temp.txt");
    check("A written file can be opened for reading", (bool) readFile);
    check("A number is read back correctly", readNumberLine(readFile) == 42);
    check("A text line is read back correctly", readTextLine(readFile) == "Hello SkillBridge");
    check("The last number is read back correctly", readNumberLine(readFile) == 7);
    readFile.close();
    remove("test_temp.txt");

    ifstream missing("no_such_file.txt");
    check("A missing file cannot be opened", !missing);

    saveToFiles();
    ifstream providerFile("providers.txt");
    check("providers.txt is created by saveToFiles()", (bool) providerFile);
    check("providers.txt starts with the number of providers", readNumberLine(providerFile) == providers.getCount());
    providerFile.close();

    ifstream bookingFile("bookings.txt");
    check("bookings.txt starts with the number of bookings", readNumberLine(bookingFile) == bookings.getCount());
    bookingFile.close();
}

const int NUM_RECORDS = 5;
string recordIds[NUM_RECORDS] = {"SB2001", "SB2002", "SB2003", "SB2004", "SB2005"};
string recordNames[NUM_RECORDS] = {"Kavita Joshi", "Arjun Rao", "Divya Nair", "Sanjay Mishra", "Mohan Lal"};
bool backgroundClear[NUM_RECORDS] = {true, true, true, true, false};

int checkRecord(string idProof, string name) {
    for (int i = 0; i < NUM_RECORDS; i++) {
        if (recordIds[i] == idProof) {
            if (recordNames[i] != name) return 2;
            if (!backgroundClear[i]) return 3;
            return 0;
        }
    }
    return 1;
}

bool verifyProvider(string idProof, string name) {
    cout << "\nVerifying..." << endl;
    int result = checkRecord(idProof, name);

    if (result == 0) {
        cout << "Verification successful." << endl;
        return true;
    }
    if (result == 1) cout << "Verification failed: ID proof not found." << endl;
    else if (result == 2) cout << "Verification failed: name does not match the ID." << endl;
    else cout << "Verification failed: background check not cleared." << endl;
    return false;
}

bool isValidPhone(string phone) {
    if (phone.length() != 10) return false;
    for (int i = 0; i < 10; i++) {
        if (phone[i] < '0' || phone[i] > '9') return false;
    }
    return true;
}

bool isValidPassword(string password) {
    return password.length() >= 4;
}

class Review {
private:
    int id;
    int bookingId;
    int providerId;
    string receiverName;
    int rating;
    string comment;
public:
    Review(int id, int bookingId, int providerId, string receiverName, int rating, string comment) {
        this->id = id;
        this->bookingId = bookingId;
        this->providerId = providerId;
        this->receiverName = receiverName;
        this->rating = rating;
        this->comment = comment;
    }
    int getId() { return id; }
    int getBookingId() { return bookingId; }
    int getProviderId() { return providerId; }
    string getReceiverName() { return receiverName; }
    int getRating() { return rating; }
    string getComment() { return comment; }
};

class ReviewNode {
public:
    Review* review;
    ReviewNode* next;
};

class ReviewList {
private:
    ReviewNode* head;
    int count;
public:
    ReviewList() {
        head = NULL;
        count = 0;
    }
    int getCount() { return count; }
    int nextId() { return count + 1; }
    ReviewNode* getHead() { return head; }

    void add(Review* review) {
        ReviewNode* node = new ReviewNode;
        node->review = review;
        node->next = NULL;

        if (head == NULL) {
            head = node;
        } else {
            ReviewNode* current = head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = node;
        }
        count++;
    }
};

ReviewList reviews;

void addReview(int bookingId, int providerId, string receiverName, int rating, string comment) {
    reviews.add(new Review(reviews.nextId(), bookingId, providerId, receiverName, rating, comment));
}

int countReviewsWaiting(int receiverId) {
    int total = 0;
    BookingNode* current = bookings.getHead();
    while (current != NULL) {
        Booking* booking = current->booking;
        if (booking->getReceiverId() == receiverId && booking->getStatus() == COMPLETED && !booking->isReviewed()) {
            total++;
        }
        current = current->next;
    }
    return total;
}

void giveReview(ServiceReceiver* receiver) {
    cout << "\n--- Completed Bookings Waiting for Your Review ---" << endl;
    int found = 0;

    BookingNode* current = bookings.getHead();
    while (current != NULL) {
        Booking* booking = current->booking;
        if (booking->getReceiverId() == receiver->getId() && booking->getStatus() == COMPLETED
            && !booking->isReviewed()) {
            booking->show();
            found++;
        }
        current = current->next;
    }
    if (found == 0) {
        cout << "You have no completed bookings waiting for a review." << endl;
        return;
    }

    int bookingId = getNumber("\nEnter the booking number to review (0 = back): ", 0, 100000);
    if (bookingId == 0) return;

    Booking* booking = bookings.findById(bookingId);
    if (booking == NULL || booking->getReceiverId() != receiver->getId()
        || booking->getStatus() != COMPLETED || booking->isReviewed()) {
        cout << "Wrong booking number." << endl;
        return;
    }

    int rating = getNumber("Give a rating from 1 to 5: ", 1, 5);
    string comment = getText("Write a short comment: ");

    addReview(bookingId, booking->getProviderId(), receiver->getName(), rating, comment);
    providers.findById(booking->getProviderId())->addRating(rating);
    booking->setReviewed(true);
    cout << "Thank you! Your review is saved." << endl;
}

void showProviderReviews(int providerId) {
    cout << "\n--- Reviews ---" << endl;
    int found = 0;

    ReviewNode* current = reviews.getHead();
    while (current != NULL) {
        Review* review = current->review;
        if (review->getProviderId() == providerId) {
            cout << "  " << review->getRating() << "/5 by " << review->getReceiverName()
                 << ": " << review->getComment() << endl;
            found++;
        }
        current = current->next;
    }
    if (found == 0) cout << "  No reviews yet." << endl;
}

void showTopRated() {
    ServiceProvider* list[MAX_FOUND];
    int count = 0;

    ProviderNode* current = providers.getHead();
    while (current != NULL && count < MAX_FOUND) {
        if (current->provider->getRatingCount() >= 3) {
            list[count] = current->provider;
            count++;
        }
        current = current->next;
    }
    if (count == 0) {
        cout << "No provider has enough ratings yet." << endl;
        return;
    }

    sortProviders(list, count, 0, 3);
    int shown = 5;
    if (count < 5) shown = count;

    cout << "\n========== TOP RATED PROVIDERS ==========" << endl;
    cout << left << setw(6) << "Rank" << setw(20) << "Name" << setw(17) << "Service" << setw(8) << "Rating" << "Ratings" << endl;
    cout << "---------------------------------------------------------" << endl;
    for (int i = 0; i < shown; i++) {
        cout << left << setw(6) << (i + 1) << setw(20) << list[i]->getName() << setw(17) << list[i]->getService()
             << setw(8) << fixed << setprecision(1) << list[i]->getRating() << list[i]->getRatingCount() << endl;
    }
}

void saveReviews() {
    ofstream reviewFile("reviews.txt");
    reviewFile << reviews.getCount() << endl;
    ReviewNode* current = reviews.getHead();
    while (current != NULL) {
        Review* review = current->review;
        reviewFile << review->getId() << endl;
        reviewFile << review->getBookingId() << endl;
        reviewFile << review->getProviderId() << endl;
        reviewFile << review->getReceiverName() << endl;
        reviewFile << review->getRating() << endl;
        reviewFile << review->getComment() << endl;
        current = current->next;
    }
    reviewFile.close();
}

void testVerificationModule() {
    cout << "\n--- Verification, validation and review tests ---" << endl;

    check("Correct ID and name is verified", checkRecord("SB2001", "Kavita Joshi") == 0);
    check("Unknown ID is rejected", checkRecord("XX9999", "Kavita Joshi") == 1);
    check("Wrong name for the ID is rejected", checkRecord("SB2001", "Someone Else") == 2);
    check("Failed background check is rejected", checkRecord("SB2005", "Mohan Lal") == 3);

    check("A phone number with 10 digits is valid", isValidPhone("9812345678"));
    check("A phone number with 5 digits is not valid", !isValidPhone("12345"));
    check("A phone number with letters is not valid", !isValidPhone("98123abc78"));
    check("A password with 4 characters is valid", isValidPassword("abcd"));
    check("A password with 3 characters is not valid", !isValidPassword("abc"));

    ReviewList testReviews;
    check("A new review list is empty", testReviews.getCount() == 0);
    check("The first review gets ID 1", testReviews.nextId() == 1);
    Review* review = new Review(1, 5, 2, "Test User", 4, "Good work");
    testReviews.add(review);
    check("After adding a review the count is 1", testReviews.getCount() == 1);
    check("The review keeps its rating", testReviews.getHead()->review->getRating() == 4);
    check("The next review gets ID 2", testReviews.nextId() == 2);

    delete review;
}

void showTestResult() {
    cout << "\nResult: " << passed << " passed, " << failed << " failed." << endl;
}

void runTests() {
    int choice;
    do {
        cout << "\n=============== TEST CASES ===============" << endl;
        cout << "1. Receiver module" << endl;
        cout << "2. Provider module" << endl;
        cout << "3. Search module" << endl;
        cout << "4. Booking module" << endl;
        cout << "5. File module" << endl;
        cout << "6. Verification, validation and review module" << endl;
        cout << "7. Run all tests" << endl;
        cout << "0. Back" << endl;
        choice = getNumber("Enter your choice: ", 0, 7);

        passed = 0;
        failed = 0;
        if (choice == 1) testReceiverModule();
        else if (choice == 2) testProviderModule();
        else if (choice == 3) testSearchModule();
        else if (choice == 4) testBookingModule();
        else if (choice == 5) testFileModule();
        else if (choice == 6) testVerificationModule();
        else if (choice == 7) {
            testReceiverModule();
            testProviderModule();
            testSearchModule();
            testBookingModule();
            testFileModule();
            testVerificationModule();
        }
        if (choice != 0) showTestResult();
    } while (choice != 0);
}

int main() {

    createProviders();
    createReceivers();
    createBookings();

    cout << "\n********************************************" << endl;
    cout << "*   SKILLBRIDGE - Local Service Platform   *" << endl;
    cout << "********************************************" << endl;

    int choice;
    do {
        cout << "\n=============== MAIN MENU ===============" << endl;
        cout << "Who are you?" << endl;
        cout << "1. Service Provider" << endl;
        cout << "2. Service Receiver" << endl;
        cout << "3. View all service providers" << endl;
        cout << "4. View all service receivers" << endl;
        cout << "5. Top rated providers" << endl;
        cout << "6. View the bookings of a provider" << endl;
        cout << "7. Run test cases" << endl;
        cout << "0. Exit" << endl;
        choice = getNumber("Enter your choice: ", 0, 7);

        if (choice == 1) providerStart();
        else if (choice == 2) receiverStart();
        else if (choice == 3) showAllProviders();
        else if (choice == 4) showAllReceivers();
        else if (choice == 5) showTopRated();
        else if (choice == 6) showBookingsOfProvider();
        else if (choice == 7) runTests();
    } while (choice != 0);

    saveToFiles();
    cout << "\nData saved in the text files. Thank you for using SkillBridge!" << endl;
    return 0;
}

