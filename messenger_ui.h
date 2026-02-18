#ifndef MESSENGER_UI_H
#define MESSENGER_UI_H

#include "messenger_manager.h"   // adjust case if needed
#include "messenger_system.h"   // for Conversation & GroupChat::display()

#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>

class MessengerUI {
private:
    MessengerManager& manager;

    static void printSeparator(const std::string& title = "") {
        std::cout << "\n════════════════════════════════════════════════════════════\n";
        if (!title.empty()) {
            std::cout << std::setw(30 + title.length()/2) << std::right << title << "\n";
            std::cout << "════════════════════════════════════════════════════════════\n";
        }
    }

    void showMainMenu() const {
        printSeparator();
        if (manager.isUserLoggedIn()) {
            std::cout << "  Logged in as: " << manager.getCurrentUsername()
                      << " (" << manager.getCurrentUserId() << ")\n\n";
        }

        std::cout << "  1. Send private message\n";
        std::cout << "  2. List my conversations\n";
        std::cout << "  3. View conversation\n";
        std::cout << "  4. Create group\n";
        std::cout << "  5. Send message to group\n";
        std::cout << "  6. List my groups\n";
        std::cout << "  7. View group chat\n";
        std::cout << "  8. Like / Unlike message\n";
        std::cout << "  9. Show all users\n";
        std::cout << "  0. Logout\n";
        std::cout << "\n  → ";
    }

    std::string readLine(const std::string& prompt) {
        std::string input;
        std::cout << prompt;
        std::getline(std::cin, input);
        return input;
    }

    void handleSendMessage() {
        std::cout << "\nAvailable users:\n";
        manager.displayAllUsers();

        std::string receiver = readLine("\nReceiver User ID: ");
        if (receiver == manager.getCurrentUserId()) {
            std::cout << "Cannot send message to yourself.\n";
            return;
        }

        std::string content = readLine("Message: ");
        if (content.empty()) {
            std::cout << "Message cannot be empty.\n";
            return;
        }

        manager.sendMessage(receiver, content);
    }

    void handleViewMyConversations() const {
        auto convs = manager.getMyConversations();
        printSeparator("MY CONVERSATIONS");

        if (convs.empty()) {
            std::cout << "  No private conversations yet.\n";
            return;
        }

        std::cout << "  " << convs.size() << " conversation(s):\n\n";
        for (const auto& c : convs) {
            auto parts = c->getParticipantIds();
            std::string other = (parts[0] == manager.getCurrentUserId()) ? parts[1] : parts[0];
            std::cout << "  • " << manager.getUsername(other) << " (" << other << ")"
                      << "  —  " << c->getMessageCount() << " msg\n";
#include "messenger_manager.h"
#include <iostream>
#include <sstream>

using namespace std;

class MessengerUI {
private:
    MessengerManager& messenger;

    void printSeparator(const string& title = "") {
        cout << "\n============================================================" << endl;
        if (!title.empty()) {
            cout << title << endl;
            cout << "============================================================" << endl;
        }
    }

    void displayMainMenu() {
        //cout << "\n╔════════════════════════════════════════════════════════╗" << endl;
        if (messenger.isUserLoggedIn()) {
            cout << "║  Logged in as: " << messenger.getCurrentUsername() 
                 << " (" << messenger.getCurrentUserId() << ")";
            int padding = 54 - messenger.getCurrentUsername().length() - messenger.getCurrentUserId().length() - 19;
            cout << string(max(0, padding), ' ') << "║" << endl;
        }
        cout << "|--------------------------------------------------------|" << endl;
        cout << "|  1. Send Message                                       |" << endl;
        cout << "| 2. View My Conversations                               |" << endl;
        cout << "|  3. View Conversation                                  |" << endl;
        cout << "|  4. Create Group                                       |" << endl;
        cout << "|  5. Send Group Message                                 |" << endl;
        cout << "|  6. View My Groups                                     |" << endl;
        cout << "|  7. View Group Details                                 |" << endl;
        cout << "|  8. Like/Unlike Message                                |" << endl;
        cout << "|  9. View All Users                                     |" << endl;
        cout << "|  0. Logout                                             |" << endl;
        cout << "|--------------------------------------------------------|" << endl;
        cout << "Enter choice: ";
    }

    void handleSendMessage() {
        string receiverId, content;
        cout << "\nAvailable users:" << endl;
        auto allUsers = messenger.getAllUsers();
        for (const auto& user : allUsers) {
            if (user.first != messenger.getCurrentUserId()) {
                cout << "  - " << user.second << " (ID: " << user.first << ")" << endl;
            }
        }
        
        cout << "\nEnter receiver ID: ";
        getline(cin, receiverId);
        cout << "Enter message: ";
        getline(cin, content);
        
        messenger.sendMessage(receiverId, content);
    }

    void handleViewMyConversations() {
        auto convs = messenger.getMyConversations();
        
        printSeparator("MY CONVERSATIONS");
        if (convs.empty()) {
            cout << "No conversations yet." << endl;
        } else {
            cout << "You have " << convs.size() << " conversation(s):" << endl;
            for (const auto& conv : convs) {
                auto participants = conv->getParticipantIds();
                string otherUser = (participants[0] == messenger.getCurrentUserId()) 
                                 ? participants[1] : participants[0];
                cout << "  - With " << messenger.getUsername(otherUser) 
                     << " (ID: " << otherUser << ")"
                     << " - " << conv->getMessageCount() << " message(s)" << endl;
            }
        }
    }

    void handleViewConversation() {
        std::string other = readLine("Other user's ID: ");
        auto conv = manager.getConversation(manager.getCurrentUserId(), other);

        if (!conv) {
            std::cout << "No conversation with that user.\n";
            return;
        }

        printSeparator("CONVERSATION WITH " + manager.getUsername(other));
        conv->display();
    }

    void handleCreateGroup() {
        std::string name = readLine("Group name: ");
        if (name.empty()) {
            std::cout << "Name cannot be empty.\n";
            return;
        }

        std::string input = readLine("Other member IDs (comma separated): ");
        std::vector<std::string> members;

        if (!input.empty()) {
            std::stringstream ss(input);
            std::string token;
            while (std::getline(ss, token, ',')) {
                token.erase(0, token.find_first_not_of(" \t"));
                token.erase(token.find_last_not_of(" \t") + 1);
                if (!token.empty() && token != manager.getCurrentUserId()) {
                    members.push_back(token);
                }
            }
        }

        auto group = manager.createGroup(name, members);
        if (group) {
            std::cout << "  Group created → ID: " << group->getGroupId() << "\n";
        string otherUserId;
        cout << "Enter user ID to view conversation: ";
        getline(cin, otherUserId);
        
        auto conv = messenger.getConversation(messenger.getCurrentUserId(), otherUserId);
        if (!conv) {
            cout << "No conversation found with " << messenger.getUsername(otherUserId) << endl;
            return;
        }

        printSeparator("CONVERSATION WITH " + messenger.getUsername(otherUserId));
        
        auto messages = conv->getMessages();
        if (messages.empty()) {
            cout << "No messages yet." << endl;
            return;
        }

        for (const auto& msg : messages) {
            bool isMine = (msg->getSenderId() == messenger.getCurrentUserId());
            string sender = isMine ? "You" : messenger.getUsername(msg->getSenderId());
            
            cout << "\n[" << msg->getMessageId() << "]" << endl;
            cout << sender << ": " << msg->getContent();
            
            if (msg->getLikeCount() > 0) {
                cout << " [" << msg->getLikeCount() << " ❤️]";
            }
            cout << endl;
        }
    }

    void handleCreateGroup() {
        string groupName, participantsInput;
        cout << "Enter group name: ";
        getline(cin, groupName);
        cout << "Enter member IDs (comma-separated, optional): ";
        getline(cin, participantsInput);
        
        vector<string> participants;
        if (!participantsInput.empty()) {
            stringstream ss(participantsInput);
            string id;
            while (getline(ss, id, ',')) {
                id.erase(0, id.find_first_not_of(" \t"));
                id.erase(id.find_last_not_of(" \t") + 1);
                if (!id.empty()) participants.push_back(id);
            }
        }
        
        auto group = messenger.createGroup(groupName, participants);
        if (group) {
            cout << "Group ID: " << group->getGroupId() << endl;
        }
    }

    void handleSendGroupMessage() {
        auto groups = manager.getMyGroups();
        if (groups.empty()) {
            std::cout << "You are not in any groups.\n";
            return;
        }

        std::cout << "\nYour groups:\n";
        for (const auto& g : groups) {
            std::cout << "  • " << g->getGroupName() << " (" << g->getGroupId() << ")\n";
        }

        std::string gid = readLine("\nGroup ID: ");
        std::string msg  = readLine("Message: ");

        if (msg.empty()) {
            std::cout << "Message cannot be empty.\n";
            return;
        }

        manager.sendGroupMessage(gid, msg);
    }

    void handleViewMyGroups() const {
        auto groups = manager.getMyGroups();
        printSeparator("MY GROUPS");

        if (groups.empty()) {
            std::cout << "  You are not in any groups yet.\n";
            return;
        }

        std::cout << "  " << groups.size() << " group(s):\n\n";
        for (const auto& g : groups) {
            std::cout << "  • " << g->getGroupName() << " (" << g->getGroupId() << ")"
                      << "  —  " << g->getParticipantCount() << " members"
                      << "  —  " << g->getMessageCount() << " messages";
            if (g->getAdminId() == manager.getCurrentUserId()) {
                std::cout << "  [admin]";
            }
            std::cout << "\n";
        string groupId, content;
        
        auto myGroups = messenger.getMyGroups();
        if (myGroups.empty()) {
            cout << "You are not in any groups yet." << endl;
            return;
        }
        
        cout << "\nYour groups:" << endl;
        for (const auto& g : myGroups) {
            cout << "  - " << g->getGroupName() 
                 << " (ID: " << g->getGroupId() << ")" << endl;
        }
        
        cout << "\nEnter group ID: ";
        getline(cin, groupId);
        cout << "Enter message: ";
        getline(cin, content);
        
        messenger.sendGroupMessage(groupId, content);
    }

    void handleViewMyGroups() {
        auto myGroups = messenger.getMyGroups();
        
        printSeparator("MY GROUPS");
        if (myGroups.empty()) {
            cout << "You are not in any groups yet." << endl;
        } else {
            cout << "You are in " << myGroups.size() << " group(s):" << endl;
            for (const auto& g : myGroups) {
                cout << "  - " << g->getGroupName() 
                     << " (ID: " << g->getGroupId() << ")"
                     << " - " << g->getParticipantCount() << " member(s)"
                     << " - " << g->getMessageCount() << " message(s)";
                if (g->getAdminId() == messenger.getCurrentUserId()) {
                    cout << " [You are admin]";
                }
                cout << endl;
            }
        }
    }

    void handleViewGroupDetails() {
        std::string gid = readLine("Group ID: ");
        auto group = manager.getGroup(gid);

        if (!group) {
            std::cout << "Group not found.\n";
            return;
        }
        if (!group->isParticipant(manager.getCurrentUserId())) {
            std::cout << "You are not a member of this group.\n";
        string groupId;
        cout << "Enter group ID: ";
        getline(cin, groupId);
        
        auto group = messenger.getGroup(groupId);
        if (!group) {
            cout << "Group not found!" << endl;
            return;
        }

        if (!group->isParticipant(messenger.getCurrentUserId())) {
            cout << "You are not a member of this group!" << endl;
            return;
        }

        printSeparator("GROUP: " + group->getGroupName());
        group->display();
    }

    void handleLikeUnlike() {
        char type;
        std::cout << " (p)rivate conversation or (g)roup? ";
        std::cin >> type;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        bool isGroup = (type == 'g' || type == 'G');

        std::string chatId = readLine("Chat/Group ID: ");
        std::string msgId  = readLine("Message ID: ");

        char action;
        std::cout << " (l)ike or (u)nlike? ";
        std::cin >> action;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        bool ok = false;
        if (action == 'l' || action == 'L') {
            ok = manager.likeMessage(msgId, chatId, isGroup);
        } else if (action == 'u' || action == 'U') {
            ok = manager.unlikeMessage(msgId, chatId, isGroup);
        } else {
            std::cout << "Invalid action.\n";
            return;
        }

        if (!ok) {
            std::cout << "Operation failed (message not found / no permission / already done).\n";
        }
    }

public:
    explicit MessengerUI(MessengerManager& mgr) : manager(mgr) {}

    void run() {
        while (true) {
            printSeparator("MESSENGER");
            std::cout << "  1. Login\n";
            std::cout << "  2. Register\n";
            std::cout << "  0. Exit\n";
            std::cout << "  → ";

            int opt;
            std::cin >> opt;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (opt == 0) {
                std::cout << "Goodbye!\n";
                break;
            }

            if (opt == 1) {
                std::string uid = readLine("User ID: ");
                if (manager.login(uid)) {
                    runLoggedIn();
                }
            }
            else if (opt == 2) {
                std::string uid = readLine("Desired User ID: ");
                std::string name = readLine("Your name: ");
                std::string pass = readLine("Password: ");

                if (manager.registerUser(uid, name, pass)) {
                    std::cout << "\nAccount created. Now you can log in.\n";
                }
            }
            else {
                std::cout << "Invalid option.\n";
            }
        }
    }

private:
    void runLoggedIn() {
        int choice;
        do {
            showMainMenu();
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            switch (choice) {
                case 1: handleSendMessage();          break;
                case 2: handleViewMyConversations();  break;
                case 3: handleViewConversation();     break;
                case 4: handleCreateGroup();          break;
                case 5: handleSendGroupMessage();     break;
                case 6: handleViewMyGroups();         break;
                case 7: handleViewGroupDetails();     break;
                case 8: handleLikeUnlike();           break;
                case 9: manager.displayAllUsers();    break;
                case 0:
                    manager.logout();
                    std::cout << "Logged out.\n";
                    return;
                default:
                    std::cout << "Invalid choice.\n";
            }

            if (choice != 0) {
                std::cout << "\n[Press Enter to continue]";
                std::cin.get();
            }
        } while (choice != 0);
    }
};

#endif // MESSENGER_UI_H
        
        cout << "Group ID: " << group->getGroupId() << endl;
        cout << "Admin: " << messenger.getUsername(group->getAdminId());
        if (group->getAdminId() == messenger.getCurrentUserId()) {
            cout << " (You)";
        }
        cout << endl;
        
        cout << "\nMembers (" << group->getParticipantCount() << "):" << endl;
        for (const auto& pid : group->getParticipantIds()) {
            cout << "  - " << messenger.getUsername(pid);
            if (pid == messenger.getCurrentUserId()) {
                cout << " (You)";
            }
            if (pid == group->getAdminId()) {
                cout << " [Admin]";
            }
            cout << endl;
        }

        cout << "\nMessages:" << endl;
        auto messages = group->getMessages();
        if (messages.empty()) {
            cout << "No messages yet." << endl;
            return;
        }

        for (const auto& msg : messages) {
            bool isMine = (msg->getSenderId() == messenger.getCurrentUserId());
            string sender = isMine ? "You" : messenger.getUsername(msg->getSenderId());
            
            cout << "\n[" << msg->getMessageId() << "]" << endl;
            cout << sender << ": " << msg->getContent();
            
            if (msg->getLikeCount() > 0) {
                cout << " [" << msg->getLikeCount() << " ❤️]";
            }
            cout << endl;
        }
    }

    void handleLikeUnlike() {
        string messageId, chatId;
        char typeChoice, actionChoice;
        
        cout << "Is this a (c)onversation or (g)roup? ";
        cin >> typeChoice;
        cin.ignore();
        
        bool isGroup = (typeChoice == 'g' || typeChoice == 'G');
        
        cout << "Enter " << (isGroup ? "group" : "conversation") << " ID: ";
        getline(cin, chatId);
        cout << "Enter message ID: ";
        getline(cin, messageId);
        
        cout << "(L)ike or (U)nlike? ";
        cin >> actionChoice;
        cin.ignore();
        
        if (actionChoice == 'L' || actionChoice == 'l') {
            messenger.likeMessage(messageId, chatId, isGroup);
        } else {
            messenger.unlikeMessage(messageId, chatId, isGroup);
        }
    }

public:
    MessengerUI(MessengerManager& mgr) : messenger(mgr) {}

    void runChatInterface() {
        int choice;
        
        do {
            displayMainMenu();
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: handleSendMessage(); break;
                case 2: handleViewMyConversations(); break;
                case 3: handleViewConversation(); break;
                case 4: handleCreateGroup(); break;
                case 5: handleSendGroupMessage(); break;
                case 6: handleViewMyGroups(); break;
                case 7: handleViewGroupDetails(); break;
                case 8: handleLikeUnlike(); break;
                case 9: messenger.displayAllUsers(); break;
                case 0: 
                    messenger.logout();
                    cout << "Logged out successfully!" << endl;
                    return;
                default:
                    cout << "Invalid choice!" << endl;
            }
            
            if (choice != 0) {
                cout << "\nPress Enter to continue...";
                cin.get();
            }
            
        } while (choice != 0);
    }

    void displayWelcome() {
        printSeparator("MESSENGER SYSTEM");
        cout << "Welcome to the Messenger!" << endl;
        printSeparator();
    }

    void runLoginMenu() {
        while (true) {
            printSeparator("LOGIN");
            cout << "1. Login" << endl;
            cout << "2. Register New User" << endl;
            cout << "3. Exit" << endl;
            cout << "Choice: ";
            
            int choice;
            cin >> choice;
            cin.ignore();
            
            if (choice == 1) {
                string userId;
                cout << "\nEnter your User ID: ";
                getline(cin, userId);
                
                if (messenger.login(userId)) {
                    runChatInterface();
                }
            } 
            else if (choice == 2) {
                string userId, username;
                cout << "\nEnter desired User ID: ";
                getline(cin, userId);
                cout << "Enter your name: ";
                getline(cin, username);
                
                if (messenger.registerUser(userId, username)) {
                    cout << "Registration successful! You can now login." << endl;
                }
            }
            else if (choice == 3) {
                cout << "Goodbye!" << endl;
                break;
            }
            else {
                cout << "Invalid choice!" << endl;
            }
        }
    }
};

#endif // MESSENGER_UI_H
