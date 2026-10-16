#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdlib>
using namespace std;
class ServiceReceiver;
class ServiceProvider;
const int NUM_SERVICES = 9;
string serviceNames[NUM_SERVICES] = {"Electrician", "Plumber", "Carpenter", "Tutor", "Domestic Helper",
                                     "Beautician", "Hairstylist", "Makeup Stylist", "Saree Draper"};
const int NUM_AREAS = 6;
string areaNames[NUM_AREAS] = {"Sector 1", "Railway Road", "City Center", "University Area", "Main Market", "Old Town"};
int areaPosition[NUM_AREAS] = {0, 2, 5, 7, 11, 14};

// Booking status
const int PENDING = 0;
const int ACCEPTED = 1;
const int REJECTED = 2;
const int MAX_FOUND = 100;      
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

    bool checkLogin(string enteredPhone, string enteredPassword) {
        return (phone == enteredPhone && password == enteredPassword);
    }

    virtual string getRole() = 0;
    virtual void showProfile() = 0;
};
void showServices();
void searchProviders(ServiceReceiver* receiver);
void showMyBookings(ServiceReceiver* receiver);
void showRequests(ServiceProvider* provider);
void sendRequest(ServiceReceiver* receiver, ServiceProvider* provider);
bool verifyProvider(string idProof, string name);
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
void registerReceiver() {
    cout << "\n===== Receiver Registration =====" << endl;
    string name = getText("Enter your name: ");
    string phone = getText("Enter your phone number: ");
    if (receivers.findByPhone(phone) != NULL) {
        cout << "This phone number is already registered. Please login." << endl;
        return;
    }
    string password = getText("Create a password: ");
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
void receiverHome(ServiceReceiver* receiver) {
    int choice;
    do {
        cout << "\n========== RECEIVER MENU (" << receiver->getName() << ") ==========" << endl;
        cout << "1. See available services" << endl;
        cout << "2. Find a provider and book" << endl;
        cout << "3. My bookings" << endl;
        cout << "4. My profile" << endl;
        cout << "0. Logout" << endl;
        choice = getNumber("Enter your choice: ", 0, 4);

        if (choice == 1) showServices();
        else if (choice == 2) searchProviders(receiver);
        else if (choice == 3) showMyBookings(receiver);
        else if (choice == 4) receiver->showProfile();
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
    }
    string getService() { return service; }
    int getExperience() { return experience; }
    int getCharge() { return charge; }
    bool isAvailable() { return available; }
    bool isAcceptingCustomers() { return acceptingCustomers; }
    string getIdProof() { return idProof; }
    int getRatingCount() { return ratingCount; }

    void setService(string newService) { service = newService; }
    void setCharge(int newCharge) { charge = newCharge; }
    void setAvailable(bool value) { available = value; }
    void setAcceptingCustomers(bool value) { acceptingCustomers = value; }
    void setRating(int total, int count) {
        ratingTotal = total;
        ratingCount = count;
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
        cout << "Experience          : " << experience << " years" << endl;
        cout << "Charge              : Rs " << charge << endl;
        cout << "Area                : " << areaNames[areaNumber] << endl;
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
void registerProvider() {
    cout << "\n===== Provider Registration =====" << endl;
    string name = getText("Enter your full name: ");
    string phone = getText("Enter your phone number: ");
    if (providers.findByPhone(phone) != NULL) {
        cout << "This phone number is already registered. Please login." << endl;
        return;
    }
    string password = getText("Create a password: ");
    int areaNumber = chooseArea();
    string service = chooseService();
    int experience = getNumber("Years of experience: ", 0, 50);
    int charge = getNumber("Your charge in Rs per visit: ", 1, 50000);
    string idProof = getText("Enter your ID proof number: ");
    if (!verifyProvider(idProof, name)) {
        cout << "Registration cancelled." << endl;
        return;
    }
    ServiceProvider* provider = new ServiceProvider(providers.nextId(), name, phone, password,
                                                    areaNumber, service, experience, charge, idProof);
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
void providerHome(ServiceProvider* provider) {
    int choice;
    do {
        cout << "\n========== PROVIDER MENU (" << provider->getName() << ") ==========" << endl;
        cout << "1. View my profile" << endl;
        cout << "2. Change the service I provide" << endl;
        cout << "3. Change my charge" << endl;
        cout << "4. Set availability (available / not available)" << endl;
        cout << "5. Accept new customers (yes / no)" << endl;
        cout << "6. View booking requests" << endl;
        cout << "0. Logout" << endl;
        choice = getNumber("Enter your choice: ", 0, 6);
        if (choice == 1) {
            provider->showProfile();
        } else if (choice == 2) {
            provider->setService(chooseService());
            cout << "Service changed to " << provider->getService() << "." << endl;
        } else if (choice == 3) {
            provider->setCharge(getNumber("Enter your new charge in Rs: ", 1, 50000));
            cout << "Charge changed." << endl;
        } else if (choice == 4) {
            provider->setAvailable(!provider->isAvailable());       // yes becomes no, no becomes yes
            if (provider->isAvailable()) cout << "You are now AVAILABLE." << endl;
            else cout << "You are now NOT AVAILABLE." << endl;
        } else if (choice == 5) {
            provider->setAcceptingCustomers(!provider->isAcceptingCustomers());
            if (provider->isAcceptingCustomers()) cout << "You are now ACCEPTING new customers." << endl;
            else cout << "You are NOT accepting new customers now." << endl;
        } else if (choice == 6) {
            showRequests(provider);
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
             << setw(18) << takingCustomers << lowest << " to " << highest << endl;
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
            } else {
                if (list[i]->getRating() < list[i + 1]->getRating()) swapNeeded = true;
            }

            if (swapNeeded) {
                ServiceProvider* temp = list[i];
                list[i] = list[i + 1];
                list[i + 1] = temp;
            }
        }
    }
}
void chooseProvider(ServiceProvider* found[], int count, ServiceReceiver* receiver) {
    int number = getNumber("Enter provider number (0 = cancel): ", 0, count);
    if (number == 0) return;
    ServiceProvider* provider = found[number - 1];
    provider->showProfile();
    cout << "Distance            : "
         << getDistance(receiver->getAreaNumber(), provider->getAreaNumber()) << " km from you" << endl;

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

    ServiceProvider* found[MAX_FOUND];
    int count = 0;
    ProviderNode* current = providers.getHead();
    while (current != NULL) {
        ServiceProvider* provider = current->provider;
        int distance = getDistance(receiver->getAreaNumber(), provider->getAreaNumber());

        if (provider->getService() == service && distance <= radius && count < MAX_FOUND) {
            found[count] = provider;
            count++;
        }
        current = current->next;
    }
    if (count == 0) {
        cout << "\nNo provider found. Try a bigger radius." << endl;
        return;
    }
    int choice;
    do {
        showTable(found, count, receiver);
        cout << "\n1. Sort by distance (nearest first)" << endl;
        cout << "2. Sort by charge (cheapest first)" << endl;
        cout << "3. Sort by rating (best first)" << endl;
        cout << "4. Select a provider" << endl;
        cout << "0. Back" << endl;
        choice = getNumber("Enter your choice: ", 0, 4);

        if (choice == 1) sortProviders(found, count, receiver->getAreaNumber(), 1);
        else if (choice == 2) sortProviders(found, count, receiver->getAreaNumber(), 2);
        else if (choice == 3) sortProviders(found, count, receiver->getAreaNumber(), 3);
        else if (choice == 4) chooseProvider(found, count, receiver);
    } while (choice != 0);
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
    int status;
public:
    Booking(int id, int receiverId, int providerId, string receiverName,
            string providerName, string service, string day) {
        this->id = id;
        this->receiverId = receiverId;
        this->providerId = providerId;
        this->receiverName = receiverName;
        this->providerName = providerName;
        this->service = service;
        this->day = day;
        status = PENDING;
    }
    int getId() { return id; }
    int getReceiverId() { return receiverId; }
    int getProviderId() { return providerId; }
    string getReceiverName() { return receiverName; }
    string getProviderName() { return providerName; }
    string getService() { return service; }
    string getDay() { return day; }
    int getStatus() { return status; }
    void setStatus(int newStatus) { status = newStatus; }

    string getStatusText() {
        if (status == PENDING) return "Pending";
        if (status == ACCEPTED) return "Accepted";
        return "Rejected";
    }

    void show() {
        cout << "  Booking " << id << " | " << service << " | Receiver: " << receiverName
             << " | Provider: " << providerName << " | When: " << day
             << " | Status: " << getStatusText() << endl;
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
void sendRequest(ServiceReceiver* receiver, ServiceProvider* provider) {
    string day = getText("Preferred day and time (for example Monday 5 PM): ");
    Booking* booking = new Booking(bookings.nextId(), receiver->getId(), provider->getId(),
                                   receiver->getName(), provider->getName(),
                                   provider->getService(), day);
    bookings.add(booking);
    pendingRequests.enqueue(booking);

    cout << "\nBooking " << booking->getId() << " sent to " << provider->getName()
         << ". Status: Pending." << endl;
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
    if (choice == 1) {
        booking->setStatus(ACCEPTED);
        pendingRequests.removeById(bookingId);        // it is not pending any more
        cout << "Booking " << bookingId << " accepted." << endl;
    } else if (choice == 2) {
        booking->setStatus(REJECTED);
        pendingRequests.removeById(bookingId);
        cout << "Booking " << bookingId << " rejected." << endl;
    }
}
void showMyBookings(ServiceReceiver* receiver) {
    cout << "\n--- My Bookings ---" << endl;
    int found = 0;

    BookingNode* current = bookings.getHead();
    while (current != NULL) {
        if (current->booking->getReceiverId() == receiver->getId()) {
            current->booking->show();
            found++;
        }
        current = current->next;
    }
    if (found == 0) cout << "You have no bookings yet." << endl;
}
void createProviders() {
    string firstNames[10] = {"Ramesh", "Sunita", "Anil", "Pooja", "Imran", "Meena", "Vikas", "Neha", "Rakesh", "Farah"};
    string lastNames[5] = {"Kumar", "Sharma", "Singh", "Verma", "Gupta"};

    for (int i = 0; i < 50; i++) {
        string name = firstNames[i % 10] + " " + lastNames[i / 10];
        string phone = to_string(9000000001L + i);
        int areaNumber = i % NUM_AREAS;
        string service = serviceNames[i % NUM_SERVICES];
        int experience = 2 + i % 12;
        int charge = 250 + (i % 8) * 75;
        string idProof = "SB" + to_string(1001 + i);

        ServiceProvider* provider = new ServiceProvider(providers.nextId(), name, phone, "1234",
                                                        areaNumber, service, experience, charge, idProof);

        int ratingCount = 3 + i % 6;                      
        int ratingTotal = ratingCount * (3 + (i / 4) % 3) - i % 2;
        provider->setRating(ratingTotal, ratingCount);
        provider->setAvailable(i % 7 != 0);               
        provider->setAcceptingCustomers(i % 11 != 0);       
        providers.add(provider);
    }
}
void createReceivers() {
    string firstNames[10] = {"Aarav", "Priya", "Rohan", "Ananya", "Karan", "Isha", "Aditya", "Kavya", "Manish", "Riya"};
    string lastNames[5] = {"Mehta", "Das", "Joshi", "Nair", "Patel"};
    for (int i = 0; i < 50; i++) {
        string name = firstNames[i % 10] + " " + lastNames[i / 10];
        string phone = to_string(9100000001L + i);
        int areaNumber = i % NUM_AREAS;

        ServiceReceiver* receiver = new ServiceReceiver(receivers.nextId(), name, phone, "1234", areaNumber);
        receivers.add(receiver);
    }
}
void addSampleBooking(int receiverId, int providerId, string day, int status) {
    ServiceReceiver* receiver = receivers.findById(receiverId);
    ServiceProvider* provider = providers.findById(providerId);
    Booking* booking = new Booking(bookings.nextId(), receiverId, providerId, receiver->getName(),
                                   provider->getName(), provider->getService(), day);
    booking->setStatus(status);
    bookings.add(booking);
    if (status == PENDING) pendingRequests.enqueue(booking);
}
void createBookings() {
    addSampleBooking(1, 2, "Monday 5 PM", PENDING);
    addSampleBooking(2, 3, "Tuesday 10 AM", PENDING);
    addSampleBooking(3, 4, "Friday 11 AM", ACCEPTED);
    addSampleBooking(4, 5, "Saturday 4 PM", REJECTED);
    addSampleBooking(1, 8, "Sunday 9 AM", PENDING);
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
void saveToFiles() {
    ofstream providerFile("providers.txt");
    ProviderNode* providerNode = providers.getHead();
    while (providerNode != NULL) {
        ServiceProvider* provider = providerNode->provider;
        providerFile << provider->getId() << " | " << provider->getName() << " | " << provider->getPhone()
                     << " | " << provider->getService() << " | " << areaNames[provider->getAreaNumber()]
                     << " | Rs " << provider->getCharge() << " | rating " << provider->getRating() << endl;
        providerNode = providerNode->next;
    }
    providerFile.close();
    ofstream receiverFile("receivers.txt");
    ReceiverNode* receiverNode = receivers.getHead();
    while (receiverNode != NULL) {
        ServiceReceiver* receiver = receiverNode->receiver;
        receiverFile << receiver->getId() << " | " << receiver->getName() << " | " << receiver->getPhone()
                     << " | " << areaNames[receiver->getAreaNumber()] << endl;
        receiverNode = receiverNode->next;
    }
    receiverFile.close();
    ofstream bookingFile("bookings.txt");
    BookingNode* bookingNode = bookings.getHead();
    while (bookingNode != NULL) {
        Booking* booking = bookingNode->booking;
        bookingFile << booking->getId() << " | " << booking->getReceiverName() << " | "
                    << booking->getProviderName() << " | " << booking->getService() << " | "
                    << booking->getDay() << " | " << booking->getStatusText() << endl;
        bookingNode = bookingNode->next;
    }
    bookingFile.close();

    cout << "\nData saved in providers.txt, receivers.txt and bookings.txt" << endl;
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
void runTests() {
    passed = 0;
    failed = 0;
    cout << "\n===== TEST CASES =====" << endl;
    check("Distance from Sector 1 to City Center is 5 km", getDistance(0, 2) == 5);
    check("Distance is the same in both directions", getDistance(2, 0) == getDistance(0, 2));
    check("Correct ID and name is verified", checkRecord("SB2001", "Kavita Joshi") == 0);
    check("Unknown ID is rejected", checkRecord("XX9999", "Kavita Joshi") == 1);
    check("Wrong name for the ID is rejected", checkRecord("SB2001", "Someone Else") == 2);
    check("Failed background check is rejected", checkRecord("SB2005", "Mohan Lal") == 3);
    ServiceProvider* first = new ServiceProvider(1, "A", "1", "x", 0, "Plumber", 1, 300, "x");
    ServiceProvider* second = new ServiceProvider(2, "B", "2", "x", 0, "Plumber", 1, 100, "x");
    ServiceProvider* third = new ServiceProvider(3, "C", "3", "x", 0, "Plumber", 1, 200, "x");
    ServiceProvider* list[3] = {first, second, third};
    sortProviders(list, 3, 0, 2);
    check("After sorting by charge the cheapest (100) is first", list[0]->getCharge() == 100);
    check("After sorting by charge the costliest (300) is last", list[2]->getCharge() == 300);
    Booking* booking1 = new Booking(1, 1, 1, "R", "P", "Plumber", "Monday");
    Booking* booking2 = new Booking(2, 1, 1, "R", "P", "Plumber", "Tuesday");
    BookingQueue testQueue;
    testQueue.enqueue(booking1);
    testQueue.enqueue(booking2);
    check("Queue gives the first request first", testQueue.dequeue()->getId() == 1);
    testQueue.removeById(2);
    check("Queue is empty after removing the last request", testQueue.isEmpty());
    delete first;
    delete second;
    delete third;
    delete booking1;
    delete booking2;
    cout << "\nResult: " << passed << " passed, " << failed << " failed." << endl;
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
        cout << "5. Run test cases" << endl;
        cout << "0. Exit" << endl;
        choice = getNumber("Enter your choice: ", 0, 5);
        if (choice == 1) providerStart();
        else if (choice == 2) receiverStart();
        else if (choice == 3) showAllProviders();
        else if (choice == 4) showAllReceivers();
        else if (choice == 5) runTests();
    } while (choice != 0);
    saveToFiles();
    cout << "\nThank you for using SkillBridge!" << endl;
    return 0;
}