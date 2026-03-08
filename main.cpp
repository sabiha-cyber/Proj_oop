#include "User.h"
#include "PostManager.h"
#include "LikeManager.h"
#include "CommentManager.h"
#include "NotificationManager.h"
#include "NewsFeed.h"
#include "FriendService.h"
#include "AuthenticationService.h"
#include "PageManager.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>
#include <filesystem>

using namespace std;

// ── Forward declarations ──────────────────────────────────────────────────────

static void userSession(User* currentUser,
                        AuthenticationService& authService,
                        PostManager& postManager,
                        FriendService& friendService,
                        NotificationManager& notificationManager,
                        LikeManager& likeManager,
                        CommentManager& commentManager,
                        PageManager& pageManager);

static void showLoginMenu(AuthenticationService& authService,
                          PostManager& postManager,
                          FriendService& friendService,
                          NotificationManager& notificationManager,
                          LikeManager& likeManager,
                          CommentManager& commentManager,
                          PageManager& pageManager);

// ── Login Menu ────────────────────────────────────────────────────────────────

static void showLoginMenu(AuthenticationService& authService,
                          PostManager& postManager,
                          FriendService& friendService,
                          NotificationManager& notificationManager,
                          LikeManager& likeManager,
                          CommentManager& commentManager,
                          PageManager& pageManager)
{
    int choice = -1;

    while (choice != 0) {
        cout << "\n+======================================================+\n";
        cout << "|              WELCOME TO LocalO                         |\n";
        cout << "+======================================================+\n";
        cout << "|  1. Login                                            |\n";
        cout << "|  2. Create New Account                               |\n";
        cout << "|  3. List All Users                                   |\n";
        cout << "|  0. Exit                                             |\n";
        cout << "+======================================================+\n";
        cout << "Enter choice: ";

        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input.\n";
            choice = -1;
            continue;
        }

        switch (choice) {
            case 1: {
                string username, password;
                cout << "Username: ";
                cin >> username;
                cout << "Password: ";
                cin >> password;

                int userId = authService.login(username, password);
                if (userId != -1) {
                    User* user = authService.findUserById(userId);
                    if (user) {
                        userSession(user, authService, postManager,
                                    friendService, notificationManager,
                                    likeManager, commentManager, pageManager);
                    }
                }
                break;
            }

            case 2: {
                string username, password, confirmPassword;
                cout << "Choose a username: ";
                cin >> username;
                cout << "Choose a password: ";
                cin >> password;
                cout << "Confirm password: ";
                cin >> confirmPassword;

                if (password != confirmPassword) {
                    cout << "ERROR: Passwords do not match.\n";
                    break;
                }

                authService.registerUser(username, password);
                break;
            }

            case 3:
                authService.listAllUsers();
                break;

            case 0:
                cout << "Goodbye!\n";
                break;

            default:
                cout << "Invalid choice.\n";
        }
    }
}

// ── User Session ──────────────────────────────────────────────────────────────

static void userSession(User* currentUser,
                        AuthenticationService& authService,
                        PostManager& postManager,
                        FriendService& friendService,
                        NotificationManager& notificationManager,
                        LikeManager& likeManager,
                        CommentManager& commentManager,
                        PageManager& pageManager)
{
    int choice = -1;

    while (choice != 0) {
        cout << "\n+---------------- MAIN MENU ----------------+\n";
        cout << "Logged in as: @" << currentUser->getUsername() << "\n";
        cout << "1. News Feed\n";
        cout << "2. Manage Friends\n";
        cout << "3. My Posts\n";
        cout << "4. View My Profile\n";
        cout << "0. Logout\n";
        cout << "Choice: ";

        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input.\n";
            choice = -1;
            continue;
        }

        switch (choice) {
            case 1: {
                NewsFeed newsFeed(currentUser, &postManager, &authService,
                                  &friendService, &notificationManager,
                                  &likeManager, &commentManager, &pageManager);
                newsFeed.showNewsFeedMenu();
                postManager.saveToFile("posts.txt");
                pageManager.saveToFile(PageManager::PAGES_FILE);
                break;
            }

            case 2: {
                int fc = -1;
                while (fc != 0) {
                    cout << "\n--------- FRIENDS ---------\n";
                    cout << "1. Send Friend Request\n";
                    cout << "2. Accept Friend Request\n";
                    cout << "3. Reject Friend Request\n";
                    cout << "4. View Pending Requests\n";
                    cout << "5. View My Friends\n";
                    cout << "6. Remove Friend\n";
                    cout << "7. View All Users\n";
                    cout << "0. Back\n";
                    cout << "Choice: ";
                    cin >> fc;

                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid input.\n";
                        fc = -1;
                        continue;
                    }

                    switch (fc) {
                        case 1: {
                            authService.listAllUsers();
                            int targetId;
                            cout << "Enter user ID to send request to: ";
                            cin >> targetId;
                            User* target = authService.findUserById(targetId);
                            if (!target) { cout << "User not found.\n"; break; }
                            friendService.sendFriendRequest(currentUser->getUserId(), target->getUsername());
                            break;
                        }

                        case 2: {
                            friendService.showPendingRequestsForUser(currentUser->getUserId());
                            int senderId;
                            cout << "Enter sender ID to accept: ";
                            cin >> senderId;
                            friendService.acceptFriendRequest(currentUser->getUserId(), senderId);
                            break;
                        }

                        case 3: {
                            friendService.showPendingRequestsForUser(currentUser->getUserId());
                            int senderId;
                            cout << "Enter sender ID to reject: ";
                            cin >> senderId;
                            friendService.rejectFriendRequest(currentUser->getUserId(), senderId);
                            break;
                        }

                        case 4:
                            friendService.showPendingRequestsForUser(currentUser->getUserId());
                            break;

                        case 5: {
                            auto friendIds = friendService.getFriendIdsOf(currentUser->getUserId());
                            if (friendIds.empty()) {
                                cout << "No friends yet.\n";
                            } else {
                                for (int fid : friendIds) {
                                    User* u = authService.findUserById(fid);
                                    if (u) cout << "ID: " << fid << " | @" << u->getUsername() << "\n";
                                }
                            }
                            break;
                        }

                        case 6: {
                            int fid;
                            cout << "Enter friend ID to remove: ";
                            cin >> fid;
                            friendService.removeFriend(currentUser->getUserId(), fid);
                            break;
                        }

                        case 7:
                            authService.listAllUsers();
                            break;

                        case 0:
                            break;

                        default:
                            cout << "Invalid choice.\n";
                    }
                }
                break;
            }

            case 3:
                currentUser->showMyPosts();
                break;

            case 4:
                currentUser->printBasicInfo();
                break;

            case 0:
                cout << "Logging out...\n";
                break;

            default:
                cout << "Invalid choice.\n";
        }
    }
}

// ── Main ──────────────────────────────────────────────────────────────────────

int main() {
    AuthenticationService authService;
    cout << "DEBUG current_path = " << filesystem::current_path() << "\n";
    cout << "DEBUG users loaded = " << authService.getUsers().size() << "\n";
    authService.listAllUsers();

    PostManager postManager(500);
    LikeManager likeManager;
    CommentManager commentManager;
    PageManager pageManager;

    NotificationManager notificationManager(&likeManager, &commentManager,
                                            &postManager, &authService, nullptr);
    FriendService friendService(authService, &notificationManager);

    // Load posts using the same User objects owned by authService
    vector<User*> usersForLoading;
    for (User& u : authService.getUsers()) usersForLoading.push_back(&u);
    postManager.loadFromFile("posts.txt", usersForLoading);

    // Load pages
    pageManager.loadFromFile(PageManager::PAGES_FILE, authService);

    showLoginMenu(authService, postManager, friendService,
                  notificationManager, likeManager, commentManager, pageManager);

    postManager.saveToFile("posts.txt");
    likeManager.saveToFile("likes.txt");
    commentManager.saveToFile("comments.txt");
    pageManager.saveToFile(PageManager::PAGES_FILE);

    return 0;
}