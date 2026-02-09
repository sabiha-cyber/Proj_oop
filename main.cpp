#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>

#include "AuthenticationService.h"
#include "FriendService.h"
#include "User.h"
#include "Post.h"

using namespace std;

int main() {
    AuthenticationService auth;         // loads users
    FriendService friendService(auth);  // loads friends & requests

    int currentUserId = -1;
    string inputLine;
    static int nextPostId = 1000;

    while (true) {
        cout << "\n";

        if (currentUserId == -1) {
            // ───────────── Not logged in ─────────────
            cout << "=== Social Network ===\n";
            cout << "1. Register\n";
            cout << "2. Login\n";
            cout << "0. Exit\n";
            cout << "Choice: ";

            getline(cin, inputLine);
            inputLine.erase(0, inputLine.find_first_not_of(" \t"));

            if (inputLine == "1") {
                string username, password;
                cout << "Username: ";
                getline(cin, username);
                cout << "Password: ";
                getline(cin, password);

                if (username.empty() || password.empty()) {
                    cout << "Username and password cannot be empty.\n";
                    continue;
                }

                if (auth.registerUser(username, password)) {
                    cout << "Registration successful! You can now log in.\n";
                } else {
                    cout << "Registration failed (username may already exist).\n";
                }
            }
            else if (inputLine == "2") {
                string username, password;
                cout << "Username: ";
                getline(cin, username);
                cout << "Password: ";
                getline(cin, password);

                currentUserId = auth.login(username, password);
                if (currentUserId != -1) {
                    cout << "Login successful!\n";
                } else {
                    cout << "Login failed – wrong username or password.\n";
                }
            }
            else if (inputLine == "0") {
                cout << "Goodbye!\n";
                break;
            }
            else {
                cout << "Invalid choice. Please try again.\n";
            }
        }
        else {
            // ───────────── Logged in ─────────────
            User* currentUser = auth.findUserById(currentUserId);
            if (!currentUser) {
                cout << "Session error: user not found. Logging out...\n";
                currentUserId = -1;
                continue;
            }

            cout << "=== Welcome, @" << currentUser->getUsername()
                 << " (ID: " << currentUserId << ") ===\n";
            cout << string(50, '-') << "\n";

            cout << " 1. Send friend request\n";
            cout << " 2. Show pending requests\n";
            cout << " 3. Accept friend request\n";
            cout << " 4. Reject friend request\n";
            cout << " 5. Show my friends\n";
            cout << " 6. List all users\n";
            cout << " 7. Create new post\n";
            cout << " 8. View my posts\n";
            cout << " 9. View news feed\n";
            cout << " 0. Logout\n";
            cout << "Choice: ";

            getline(cin, inputLine);
            inputLine.erase(0, inputLine.find_first_not_of(" \t"));

            if (inputLine == "1") {
                string target;
                cout << "Enter target username: ";
                getline(cin, target);
                if (!target.empty()) {
                    friendService.sendFriendRequest(currentUserId, target);
                } else {
                    cout << "Username cannot be empty.\n";
                }
            }
            else if (inputLine == "2") {
                friendService.showPendingRequestsForUser(currentUserId);
            }
            else if (inputLine == "3") {
                string senderName;
                cout << "Enter sender username: ";
                getline(cin, senderName);
                if (senderName.empty()) {
                    cout << "Username cannot be empty.\n";
                    continue;
                }
                User* sender = auth.findUserByUsername(senderName);
                if (sender) {
                    friendService.acceptFriendRequest(currentUserId, sender->getUserId());
                } else {
                    cout << "User not found.\n";
                }
            }
            else if (inputLine == "4") {
                string senderName;
                cout << "Enter sender username: ";
                getline(cin, senderName);
                if (senderName.empty()) {
                    cout << "Username cannot be empty.\n";
                    continue;
                }
                User* sender = auth.findUserByUsername(senderName);
                if (sender) {
                    friendService.rejectFriendRequest(currentUserId, sender->getUserId());
                } else {
                    cout << "User not found.\n";
                }
            }
            else if (inputLine == "5") {
                vector<int> friendIds = friendService.getFriendIdsOf(currentUserId);
                if (friendIds.empty()) {
                    cout << "You have no friends yet.\n";
                } else {
                    cout << "\nYour friends:\n";
                    cout << string(40, '-') << "\n";
                    for (int fid : friendIds) {
                        User* f = auth.findUserById(fid);
                        if (f) f->printBasicInfo();
                    }
                    cout << string(40, '-') << "\n";
                }
            }
            else if (inputLine == "6") {
                auth.listAllUsers();
            }
            else if (inputLine == "7") {
                // Clear leftover newline before interactive input
                //cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << "\n--- Create New Post ---\n";
                Post* newPost = Post::createPost(nextPostId++, currentUser);
                if (newPost) {
                    currentUser->addPost(newPost);
                    cout << "Your post has been published!\n";
                } else {
                    cout << "Post creation cancelled or failed.\n";
                }
            }
            else if (inputLine == "8") {
                cout << "\n--- Your Posts ---\n";
                currentUser->showMyPosts();
            }
            else if (inputLine == "9") {
                cout << "\n=== News Feed ===\n";
                cout << string(50, '-') << "\n";

                // Your own posts
                cout << "Your recent posts:\n";
                currentUser->showMyPosts();

                // Friends' posts
                vector<int> friendIds = friendService.getFriendIdsOf(currentUserId);
                if (!friendIds.empty()) {
                    cout << "\nFriends' posts:\n";
                    cout << string(50, '-') << "\n";
                    for (int fid : friendIds) {
                        User* friendUser = auth.findUserById(fid);
                        if (friendUser) {
                            cout << "@" << friendUser->getUsername() << ":\n";
                            friendUser->showMyPosts();
                            cout << "\n";
                        }
                    }
                } else {
                    cout << "\nNo friends yet. Add some friends to see their posts!\n";
                }
            }
            else if (inputLine == "0") {
                cout << "Logged out successfully.\n";
                currentUserId = -1;
            }
            else {
                cout << "Invalid choice. Please enter a number from 0-9.\n";
            }
        }
    }

    // Optional: could add cleanup of all posts here if you want
    // for now it's fine for learning project (memory leak is acceptable)

    return 0;
}