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
