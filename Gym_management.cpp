#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <map>
#include <queue>
#include <vector>
#include <list>
#include <unordered_map>
#include <ctime>
#include <sstream>
using namespace std;
time_t convertToTime(const string &dateStr)
{
    struct tm tm = {0};
    int year, month, day;

    stringstream ss(dateStr);
    char dash;
    ss >> year >> dash >> month >> dash >> day;

    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;

    return mktime(&tm);
}

struct Member
{
    int id;
    string name;
    int age;
    string status;
    int height;
    time_t expirationDate;
    Member *left, *right;
};

struct Trainer
{
    int id;
    string name;
    string specialization;
    bool isAvailable;
    vector<int> members;
};

struct Payment
{
    int paymentID;
    int memberID;
    double amount;
    time_t paymentDate;
    Payment *next;
};

class TrainerManagement
{
public:
    unordered_map<int, Trainer> trainers;

    void addTrainer(int id, string name, string specialization, bool isAvailable)
    {
        if (trainers.find(id) != trainers.end())
        {
            cout << "Error: Trainer ID " << id << " already exists. Please use a unique ID." << endl;
            return;
        }
        trainers[id] = {id, name, specialization, isAvailable};
    }

    void deleteTrainer(int id)
    {
        if (trainers.find(id) == trainers.end())
        {
            cout << "trainer not found" << endl;
            return;
        }

        trainers.erase(id);
    }

    void unassignMemberFromAllTrainers(int memberID)
    {
        for (auto &trainer : trainers)
        {
            auto &members = trainer.second.members;
            members.erase(remove(members.begin(), members.end(), memberID), members.end());
        }
    }

    Trainer *searchTrainer(int id)
    {
        if (trainers.find(id) != trainers.end())
            return &trainers[id];
        else
            return nullptr;
    }

    void displayTrainerDetails(Trainer *trainer)
    {
        if (trainer)
        {
            cout << left << setw(10) << "ID" << setw(20) << "Name"
                 << setw(25) << "Specialization" << setw(15) << "Available" << endl;
            cout << left << setw(10) << trainer->id << setw(20) << trainer->name
                 << setw(25) << trainer->specialization
                 << setw(15) << (trainer->isAvailable ? "Yes" : "No") << endl;
        }
        else
        {
            cout << "Trainer not found.\n";
        }
    }

    void assignMemberToTrainer(int trainerID, int memberID)
    {

        if (trainers.find(trainerID) != trainers.end())
        {
            if (!trainers[trainerID].isAvailable)
            {
                cout << "Trainer not available " << endl;
                return;
            }

            vector<int> v = trainers[trainerID].members;
            auto it = std::find(v.begin(), v.end(), memberID);
            if (it != v.end())
            {
                cout << "member already assigned to the trainer " << endl;
                return;
            }

            trainers[trainerID].members.push_back(memberID);
            cout << "Member " << memberID << " assigned to Trainer " << trainerID << endl;
        }
        else
        {
            cout << "Trainer not found." << endl;
        }
    }

    void unassignMemberFromTrainer(int trainerID, int memberID)
    {
        if (trainers.find(trainerID) != trainers.end())
        {
            auto &members = trainers[trainerID].members;

            auto it = std::find(members.begin(), members.end(), memberID);

            if (it != members.end())
            {
                members.erase(it);
                cout << "Member " << memberID << " unassigned from Trainer " << trainerID << endl;
            }
            else
            {
                cout << "Member " << memberID << " not found under Trainer " << trainerID << endl;
            }
        }
        else
        {
            cout << "Trainer " << trainerID << " not found." << endl;
        }
    }

    void displayTrainerMembers()
    {
        cout << left << setw(15) << "Trainer ID" << setw(25) << "Members" << endl;
        cout << string(40, '-') << endl;

        for (const auto &trainer : trainers)
        {
            cout << left << setw(15) << trainer.second.id;

            for (const auto &memberID : trainer.second.members)
            {
                cout << memberID << " ";
            }
            cout << endl;
        }
    }

    void displayTrainers()
    {
        cout << left << setw(10) << "ID"
             << setw(20) << "Name"
             << setw(25) << "Specialization"
             << setw(15) << "Available" << endl;
        cout << string(70, '-') << endl;

        for (const auto &t : trainers)
        {
            cout << left << setw(10) << t.second.id
                 << setw(20) << t.second.name
                 << setw(25) << t.second.specialization
                 << setw(15) << (t.second.isAvailable ? "Yes" : "No") << endl;
        }
    }

    void saveToFile()
    {
        ofstream outFile("trainers.txt");
        for (auto &t : trainers)
        {
            outFile << t.second.id << " " << t.second.name << " " << t.second.specialization << " " << t.second.isAvailable << " ";
            for (const auto &memberID : t.second.members)
            {
                outFile << memberID << " ";
            }
            outFile << endl;
        }
        outFile.close();
    }

    void loadFromFile()
    {
        ifstream inFile("trainers.txt");
        if (!inFile.is_open())
        {
            ofstream outFile("trainers.txt");
            outFile.close();
            cout << "trainers.txt file not found. A new file has been created." << endl;
            return;
        }

        int id;
        string name, specialization;
        bool isAvailable;

        while (inFile >> id >> name >> specialization >> isAvailable)
        {
            Trainer trainer{id, name, specialization, isAvailable, {}};

            string line;
            getline(inFile, line);

            istringstream ss(line);
            int memberID;
            while (ss >> memberID)
            {
                trainer.members.push_back(memberID);
            }

            trainers[id] = trainer;
        }
        inFile.close();
    }
};

class MemberManagement
{
public:
    Member *root;

    MemberManagement() : root(nullptr) {}

    Member *insert(Member *node, int id, string name, int age, string status, time_t expirationDate)
    {
        if (!node)
        {
            Member *newNode = new Member{id, name, age, status, 1, expirationDate, nullptr, nullptr};

            return newNode;
        }

        if (id < node->id)
            node->left = insert(node->left, id, name, age, status, expirationDate);
        else if (id > node->id)
            node->right = insert(node->right, id, name, age, status, expirationDate);
        else
        {
            cout << "member ID already exists " << endl;
            return node;
        }

        node->height = 1 + max(height(node->left), height(node->right));
        int balance = getBalance(node);

        if (balance > 1 && id < node->left->id)
            return rotateRight(node);
        if (balance < -1 && id > node->right->id)
            return rotateLeft(node);
        if (balance > 1 && id > node->left->id)
        {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (balance < -1 && id < node->right->id)
        {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    Member *searchMember(Member *node, int id)
    {
        if (!node || node->id == id)
            return node;
        if (id < node->id)
            return searchMember(node->left, id);
        else
            return searchMember(node->right, id);
    }

    void displayMemberDetails(Member *member)
    {
        if (member)
        {
            cout << left << setw(10) << "ID" << setw(20) << "Name" << setw(10) << "Age"
                 << setw(15) << "Status" << setw(20) << "Expiration Date" << endl;
            cout << left << setw(10) << member->id << setw(20) << member->name
                 << setw(10) << member->age << setw(15) << member->status
                 << setw(20) << ctime(&member->expirationDate);
        }
        else
        {
            cout << "Member not found.\n";
        }
    }

    Member *deleteMember(Member *node, int id, TrainerManagement &trainerMgmt)
    {
        Member *member = searchMember(root, id);
        if (!member)
        {
            cout << "Member not found " << endl;
            return node;
        }

        if (!node)
            return node;

        if (id < node->id)
            node->left = deleteMember(node->left, id, trainerMgmt);
        else if (id > node->id)
            node->right = deleteMember(node->right, id, trainerMgmt);
        else
        {
            trainerMgmt.unassignMemberFromAllTrainers(id);

            if (!node->left || !node->right)
            {
                Member *temp = node->left ? node->left : node->right;
                if (!temp)
                {
                    temp = node;
                    node = nullptr;
                }
                else
                    *node = *temp;
                delete temp;
            }
            else
            {
                Member *temp = minValueNode(node->right);
                node->id = temp->id;
                node->name = temp->name;
                node->age = temp->age;
                node->status = temp->status;
                node->right = deleteMember(node->right, temp->id, trainerMgmt);
            }
        }

        if (!node)
            return node;

        node->height = 1 + max(height(node->left), height(node->right));
        int balance = getBalance(node);

        if (balance > 1 && getBalance(node->left) >= 0)
            return rotateRight(node);
        if (balance > 1 && getBalance(node->left) < 0)
        {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (balance < -1 && getBalance(node->right) <= 0)
            return rotateLeft(node);
        if (balance < -1 && getBalance(node->right) > 0)
        {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    void updateStatusBasedOnExpiration(Member *node)
    {
        if (!node)
            return;

        time_t currentTime = time(0);
        if (difftime(node->expirationDate, currentTime) <= 0 && node->status == "Active")
        {
            node->status = "Inactive";
        }

        updateStatusBasedOnExpiration(node->left);
        updateStatusBasedOnExpiration(node->right);
    }

    void displayMembers(Member *node, bool headerPrinted)
    {

        if (!node)
            return;

        if (!headerPrinted)
        {
            cout << left << setw(10) << "ID" << setw(20) << "Name" << setw(10) << "Age"
                 << setw(15) << "Status" << setw(20) << "Expiration Date" << endl;
            cout << string(75, '-') << endl;
            headerPrinted = true;
        }

        displayMembers(node->left, headerPrinted);
        cout << left << setw(10) << node->id << setw(20) << node->name << setw(10) << node->age
             << setw(15) << node->status << setw(20) << ctime(&node->expirationDate);
        displayMembers(node->right, headerPrinted);
    }

    int height(Member *node) { return node ? node->height : 0; }
    int getBalance(Member *node) { return node ? height(node->left) - height(node->right) : 0; }

    Member *rotateRight(Member *y)
    {
        Member *x = y->left;
        Member *T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;
        return x;
    }

    Member *rotateLeft(Member *x)
    {
        Member *y = x->right;
        Member *T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;

        return y;
    }

    Member *minValueNode(Member *node)
    {
        Member *current = node;

        while (current && current->left != nullptr)
            current = current->left;

        return current;
    }

    void saveToFile()
    {
        ofstream outFile("members.txt");
        saveToFileHelper(root, outFile);
        outFile.close();
    }

    void saveToFileHelper(Member *node, ofstream &outFile)
    {
        if (!node)
            return;
        outFile << node->id << " " << node->name << " " << node->age << " "
                << node->status << " " << node->expirationDate << endl;
        saveToFileHelper(node->left, outFile);
        saveToFileHelper(node->right, outFile);
    }

    void loadFromFile()
    {
        ifstream inFile("members.txt");
        int id, age;
        string name, status;
        time_t expirationDate;
        if (inFile.is_open())
        {
            while (inFile >> id >> name >> age >> status >> expirationDate)
            {
                root = insert(root, id, name, age, status, expirationDate);
            }
            inFile.close();
        }
        else
        {
            ofstream outFile("members.txt");
            outFile.close();
            cout << "members.txt file not found. A new file has been created." << endl;
        }
    }
};

class PaymentManagement
{
public:
    Payment *head;
    int paymentIDCounter = 1;

    PaymentManagement() : head(nullptr) {}

    void addPayment(int paymentID, int memberID, double amount)
    {
        time_t currentTime = time(0);
        Payment *newPayment = new Payment{paymentID, memberID, amount, currentTime, NULL};

        if (head == NULL)
        {
            head = newPayment;
        }
        else
        {
            Payment *current = head;
            while (current->next != NULL)
            {
                current = current->next;
            }
            current->next = newPayment;
        }

        cout << "Payment added for Member " << memberID << " with amount $" << amount << endl;
    }

    void processPayment(int memberID, double amount, MemberManagement &memberMgmt)
    {

        Member *member = memberMgmt.searchMember(memberMgmt.root, memberID);
        if (member)
        {
            time_t currentTime = time(0);
            int renewalPeriod = 0;

            if (amount == 1500)
            {
                renewalPeriod = 30 * 24 * 60 * 60;
                addPayment(paymentIDCounter++, memberID, amount);
            }
            else if (amount == 8000)
            {
                addPayment(paymentIDCounter++, memberID, amount);
                renewalPeriod = 6 * 30 * 24 * 60 * 60;
            }
            else if (amount == 14000)
            {
                addPayment(paymentIDCounter++, memberID, amount);
                renewalPeriod = 365 * 24 * 60 * 60;
            }
            else
            {
                cout << "Invalid payment amount. Please pay according to the available plans.\n";
                return;
            }

            if (member->status == "Inactive" && difftime(member->expirationDate, currentTime) <= 0)
            {
                member->status = "Active";
                member->expirationDate = currentTime + renewalPeriod;
                cout << "Membership for Member " << memberID << " renewed and status set to Active.\n";
            }
            else
            {
                member->expirationDate += renewalPeriod;
                cout << "Membership for Member " << memberID << " extended.\n";
            }
        }
        else
        {
            cout << "Member not found.\n";
        }
    }

    void saveToFile()
    {
        ofstream outFile("payments.txt");
        Payment *current = head;
        outFile << paymentIDCounter << endl;
        while (current)
        {
            outFile << current->paymentID << " " << current->memberID << " "
                    << current->amount << " " << current->paymentDate << endl;
            current = current->next;
        }
        outFile.close();
    }

    void loadFromFile()
    {
        ifstream inFile("payments.txt");
        if (!inFile.is_open())
        {
            ofstream outFile("payments.txt");
            outFile << 1 << endl;
            outFile.close();
            cout << "payments.txt file not found. A new file has been created." << endl;
            return;
        }

        int paymentID, memberID;
        double amount;
        time_t paymentDate;
        inFile >> paymentIDCounter;
        while (inFile >> paymentID >> memberID >> amount >> paymentDate)
        {
            time_t currentTime = time(0);
            Payment *newPayment = new Payment{paymentID, memberID, amount, currentTime, NULL};

            if (head == NULL)
            {
                head = newPayment;
            }
            else
            {
                Payment *current = head;
                while (current->next != NULL)
                {
                    current = current->next;
                }
                current->next = newPayment;
            }
        }
        inFile.close();
    }

    void displayPayments()
    {
        Payment *current = head;
        cout << left << setw(10) << "Payment ID " << setw(10) << "Member ID" << setw(15) << "Amount" << setw(20) << "Payment Date" << endl;
        cout << string(55, '-') << endl;
        while (current)
        {
            cout << left << setw(10) << current->paymentID << setw(10) << current->memberID
                 << setw(15) << current->amount << setw(20) << ctime(&(current->paymentDate));
            current = current->next;
        }
    }
};

int main()
{
    MemberManagement memberMgmt;
    memberMgmt.loadFromFile();

    TrainerManagement trainerMgmt;
    trainerMgmt.loadFromFile();

    PaymentManagement paymentMgmt;
    paymentMgmt.loadFromFile();

    int choice, trainerID, memberID, id, age;
    string name, status, specialization;
    string time;
    time_t expirationDate;
    bool isAvailable;

    while (true)
    {

        cout << "\nGym Management System Menu:\n";
        cout << "1. Add Member\n";
        cout << "2. Delete Member\n";
        cout << "3. Add Trainer\n";
        cout << "4. Delete Trainer\n";
        cout << "5. Display Members\n";
        cout << "6. Display Trainers\n";
        cout << "7. update membership status\n";
        cout << "8. Assign Member to Trainer\n";
        cout << "9. Unassign Member from Trainer\n";
        cout << "10. Display Members by Trainer\n";
        cout << "11. Search Member\n";
        cout << "12. Search Trainer\n";
        cout << "13. Make Payment and Renew Membership\n";
        cout << "14. Display Payments\n";
        cout << "15. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            cout << "Enter Member ID, Name, Age, Status , Expiration_Date : ";
            cin >> id >> name >> age >> status >> time;
            expirationDate = convertToTime(time);
            memberMgmt.root = memberMgmt.insert(memberMgmt.root, id, name, age, status, expirationDate);
            memberMgmt.saveToFile();
            break;
        case 2:
            cout << "Enter Member ID to delete: ";
            cin >> id;
            memberMgmt.root = memberMgmt.deleteMember(memberMgmt.root, id, trainerMgmt);
            memberMgmt.saveToFile();
            break;
        case 3:
            cout << "Enter Trainer ID, Name, Specialization, Availability (1 for available, 0 for not): ";
            cin >> id >> name >> specialization >> isAvailable;
            trainerMgmt.addTrainer(id, name, specialization, isAvailable);
            trainerMgmt.saveToFile();
            break;
        case 4:
            cout << "Enter Trainer ID to delete: ";
            cin >> id;
            trainerMgmt.deleteTrainer(id);
            trainerMgmt.saveToFile();
            break;
        case 5:
            cout << "Displaying all members:\n";
            memberMgmt.displayMembers(memberMgmt.root, false);
            break;
        case 6:
            cout << "Displaying all trainers:\n";
            trainerMgmt.displayTrainers();
            break;
        case 7:
            cout << "Updating memberships with expired status...\n";
            memberMgmt.updateStatusBasedOnExpiration(memberMgmt.root);
            memberMgmt.saveToFile();
            break;
        case 8:
            cout << "Enter Trainer ID and Member ID to assign: ";
            cin >> trainerID >> memberID;
            if (!(memberMgmt.searchMember(memberMgmt.root, memberID)))
            {
                cout << "member with member ID : " << memberID << " not found to assign" << endl;
            }
            else
            {
                trainerMgmt.assignMemberToTrainer(trainerID, memberID);
            }
            break;
        case 9:
            cout << "Enter Trainer ID and Member ID to unassign: ";
            cin >> trainerID >> memberID;
            trainerMgmt.unassignMemberFromTrainer(trainerID, memberID);
            trainerMgmt.saveToFile();
            break;
        case 10:
            cout << "Displaying members assigned to each trainer:\n";
            trainerMgmt.displayTrainerMembers();
            break;
        case 11:
            cout << "Enter Member ID to search: ";
            cin >> id;
            memberMgmt.displayMemberDetails(memberMgmt.searchMember(memberMgmt.root, id));
            break;
        case 12:
            cout << "Enter Trainer ID to search: ";
            cin >> id;
            trainerMgmt.displayTrainerDetails(trainerMgmt.searchTrainer(id));
            break;
        case 13:
            double amount;
            cout << "Our Gym offers the following membership plans:\n";
            cout << "---------------------------------------------\n";
            cout << "1. 1-Month Membership : $1500\n";
            cout << "2. 6-Month Membership : $8000\n";
            cout << "3. 1-Year Membership : $14000\n";

            cout << "Enter Member ID and Payment Amount: ";
            cin >> memberID >> amount;
            paymentMgmt.processPayment(memberID, amount, memberMgmt);
            memberMgmt.saveToFile();
            break;

        case 14:
            paymentMgmt.displayPayments();
            break;

        case 15:
            memberMgmt.saveToFile();
            trainerMgmt.saveToFile();
            paymentMgmt.saveToFile();
            cout << "Exiting and saving data to files.\n";
            return 0;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    }
}