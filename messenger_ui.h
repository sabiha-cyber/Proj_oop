#ifndef MESSENGER_UI_H
#define MESSENGER_UI_H

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
