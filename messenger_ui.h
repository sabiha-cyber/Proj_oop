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