#include <iostream>
#include <string>
#include <vector>

#include "AuthenticationService.h"
#include "FriendService.h"

using namespace std;

int main() {
    AuthenticationService auth;          // loads users from file
    FriendService friendService(auth);   // loads friend requests & friends

    int currentUserId = -1;
    string choice;


    while (true) {
        if (currentUserId == -1) {
            // -------- NOT LOGGED IN --------
            cout << "1. Register\n";
            cout << "2. Login\n";
            cout << "0. Exit\n";
            cout << "Choice: ";
            getline(cin, choice);

            if (choice == "1") {
                string username, password;
                cout << "Username: ";
                getline(cin, username);
                cout << "Password: ";
                getline(cin, password);

                auth.registerUser(username, password);
            }
            else if (choice == "2") {
                string username, password;
                cout << "Username: ";
                getline(cin, username);
                cout << "Password: ";
                getline(cin, password);

                currentUserId = auth.login(username, password);
            }
            else if (choice == "0") {
                break;
            }
            else {
                cout << "Invalid option.\n";
            }
        }
        else {
            // -------- LOGGED IN --------
            User* me = auth.findUserById(currentUserId);
            if (!me) {
                cout << "Session error. Logging out.\n";
                currentUserId = -1;
                continue;
            }

            cout << "\nLogged in as: " << me->getUsername()
                 << " (ID: " << currentUserId << ")\n";
            cout << "------------------------------------\n";
            cout << "1. Send friend request\n";
            cout << "2. Show pending requests\n";
            cout << "3. Accept friend request\n";
            cout << "4. Reject friend request\n";
            cout << "5. Show my friends\n";
            cout << "6. List all users\n";
            cout << "0. Logout\n";
            cout << "Choice: ";

            getline(cin, choice);

            if (choice == "1") {
                string target;
                cout << "Enter username: ";
                getline(cin, target);
                friendService.sendFriendRequest(currentUserId, target);
            }
            else if (choice == "2") {
                friendService.showPendingRequestsForUser(currentUserId);
            }
            else if (choice == "3") {
                string fromUser;
                cout << "Enter sender username to accept: ";
                getline(cin, fromUser);

                User* sender = auth.findUserByUsername(fromUser);
                if (sender) {
                    friendService.acceptFriendRequest(currentUserId, sender->getUserId());
                } else {
                    cout << "User not found.\n";
                }
            }
            else if (choice == "4") {
                string fromUser;
                cout << "Enter sender username to reject: ";
                getline(cin, fromUser);

                User* sender = auth.findUserByUsername(fromUser);
                if (sender) {
                    friendService.rejectFriendRequest(currentUserId, sender->getUserId());
                } else {
                    cout << "User not found.\n";
                }
            }
            else if (choice == "5") {
                vector<int> friends = friendService.getFriendIdsOf(currentUserId);
                if (friends.empty()) {
                    cout << "You have no friends yet.\n";
                } else {
                    cout << "\nYour friends:\n";
                    for (int fid : friends) {
                        User* f = auth.findUserById(fid);
                        if (f) f->printBasicInfo();
                    }
                }
            }
            else if (choice == "6") {
                auth.listAllUsers();
            }
            else if (choice == "0") {
                cout << "Logged out.\n";
                currentUserId = -1;
            }
            else {
                cout << "Invalid option.\n";
            }
        }

        cout << "\n";
    }
    return 0;
}
