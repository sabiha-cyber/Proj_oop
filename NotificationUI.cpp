#include "NotificationUI.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>

using namespace std;

// ── Constructor ───────────────────────────────────────────────────────────────
NotificationUI::NotificationUI(NotificationManager& manager)
    : mgr(manager) {}

// ── Helpers ───────────────────────────────────────────────────────────────────
void NotificationUI::printDivider() const {
    cout << "  ------------------------------------------------\n";
}

void NotificationUI::printHeader(const string& title, int userId) const {
    int unread = mgr.unreadCount(userId);
    cout << "\n";
    cout << "   +------------------------------------------+\n";
    cout << "   |   " << left << setw(40) << title << "|\n";
    if (unread > 0)
        cout << "|       " << unread << " unread\n";
            // << string(36 - to_string(unread).length() - 7, ' ') << "|\n";
    cout << "   +------------------------------------------+\n";
}

int NotificationUI::getIntInput(const string& prompt) const {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Invalid input. Please enter a number.\n";
    }
}

bool NotificationUI::confirm(const string& prompt) const {
    char c;
    cout << prompt << " (y/n): ";
    cin >> c;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return (c == 'y' || c == 'Y');
}

NotificationType NotificationUI::pickType() const {
    cout << "\n  Select notification type:\n";
    cout << "    1. Likes\n";
    cout << "    2. Comments\n";
    cout << "    3. Friend Requests\n";
    cout << "    4. Friend Accepts\n";
    cout << "    5. Shares\n";
    cout << "    6. Mentions\n";
    int ch = getIntInput("  Choice: ");
    switch (ch) {
        case 1: return NotificationType::LIKE;
        case 2: return NotificationType::COMMENT;
        case 3: return NotificationType::FRIEND_REQUEST;
        case 4: return NotificationType::FRIEND_ACCEPT;
        case 5: return NotificationType::POST_SHARE;
        case 6: return NotificationType::MENTION;
        default:
            cout << "  Unknown type, defaulting to LIKE.\n";
            return NotificationType::LIKE;
    }
}

// ── Badge (for main menu header) ──────────────────────────────────────────────
void NotificationUI::showBadge(int userId) const {
    int unread = mgr.unreadCount(userId);
    if (unread > 0)
        cout << "   You have " << unread << " unread notification(s).\n";
}

// ── Main entry point ──────────────────────────────────────────────────────────
void NotificationUI::showNotificationPanel(int userId, const string& username) {
    menuMain(userId, username);
}

// ── Main menu ─────────────────────────────────────────────────────────────────
void NotificationUI::menuMain(int userId, const string& username) {
    int choice = -1;
    while (choice != 0) {
        printHeader("Notifications  " , userId);
        cout << "\n";
        cout << "    1.  View All Notifications\n";
        cout << "    2.  View Unread Only\n";
        cout << "    3.  Mark Notifications as Read\n";
        cout << "    4.  Delete Notifications\n";
        cout << "    5.  Filter by Type\n";
        cout << "    0.  Back\n";
        cout << "\n";
        choice = getIntInput("  Enter choice: ");

        switch (choice) {
            case 1: menuViewAll    (userId);           break;
            case 2: menuViewUnread (userId);           break;
            case 3: menuMarkRead   (userId);           break;
            case 4: menuDelete     (userId);           break;
            case 5: menuFilter     (userId);           break;
            case 0: cout << "  Returning to main menu...\n"; break;
            default: cout << "  Invalid choice.\n";   break;
        }
    }
}

// ── View All ──────────────────────────────────────────────────────────────────
void NotificationUI::menuViewAll(int userId) {
    auto notifs = mgr.getForUser(userId);

    printHeader("All Notifications", userId);
    cout << "\n";

    if (notifs.empty()) {
        cout << "  No notifications yet.\n";
        return;
    }

    printDivider();
    cout << "  " << left << setw(6) << "ID"
                         << setw(5) << ""
                         << "Message\n";
    printDivider();

    for (Notification* n : notifs) {
        cout << "  #" << left << setw(5) << n->getNotifId();
        n->display();
    }
    printDivider();

    cout << "\n  Options:\n";
    cout << "    1. View detailed info for a notification\n";
    cout << "    2. Mark a specific notification as read\n";
    cout << "    0. Back\n";
    int ch = getIntInput("  Choice: ");

    if (ch == 1) {
        int id = getIntInput("  Enter Notification ID: ");
        Notification* n = mgr.findById(id);
        if (n && n->getRecipientId() == userId) {
            cout << "\n";
            printDivider();
            n->displayDetailed();
            printDivider();
        } else {
            cout << "  Notification not found.\n";
        }
    } else if (ch == 2) {
        int id = getIntInput("  Enter Notification ID to mark as read: ");
        Notification* n = mgr.findById(id);
        if (n && n->getRecipientId() == userId) {
            mgr.markAsRead(id);
            cout << "  Notification #" << id << " marked as read.\n";
        } else {
            cout << "  Notification not found or not yours.\n";
        }
    }
}

// ── View Unread ───────────────────────────────────────────────────────────────
void NotificationUI::menuViewUnread(int userId) {
    auto notifs = mgr.getUnreadForUser(userId);

    printHeader("Unread Notifications", userId);
    cout << "\n";

    if (notifs.empty()) {
        cout << "  All caught up! No unread notifications.\n";
        return;
    }

    printDivider();
    for (Notification* n : notifs) {
        cout << "  #" << left << setw(5) << n->getNotifId();
        n->display();
    }
    printDivider();

    cout << "\n  Mark all as read? ";
    if (confirm("")) {
        mgr.markAllAsRead(userId);
        cout << "  All notifications marked as read.\n";
    }
}

// ── Mark Read ─────────────────────────────────────────────────────────────────
void NotificationUI::menuMarkRead(int userId) {
    printHeader("Mark as Read", userId);
    cout << "\n";
    cout << "    1. Mark ALL as read\n";
    cout << "    2. Mark a specific notification as read\n";
    cout << "    0. Back\n";

    int ch = getIntInput("  Choice: ");

    if (ch == 1) {
        if (confirm("  Mark all notifications as read?")) {
            mgr.markAllAsRead(userId);
        }
    } else if (ch == 2) {
        // Show current unread list first
        auto unread = mgr.getUnreadForUser(userId);
        if (unread.empty()) {
            cout << "  No unread notifications.\n";
            return;
        }
        printDivider();
        for (Notification* n : unread) {
            cout << "  #" << left << setw(5) << n->getNotifId();
            n->display();
        }
        printDivider();

        int id = getIntInput("  Enter Notification ID: ");
        Notification* n = mgr.findById(id);
        if (n && n->getRecipientId() == userId) {
            if (n->isRead()) {
                cout << "  Notification #" << id << " is already read.\n";
            } else {
                mgr.markAsRead(id);
                cout << "  Notification #" << id << " marked as read.\n";
            }
        } else {
            cout << "  Notification not found or not yours.\n";
        }
    }
}

// ── Delete ────────────────────────────────────────────────────────────────────
void NotificationUI::menuDelete(int userId) {
    printHeader("Delete Notifications", userId);
    cout << "\n";
    cout << "    1. Delete a specific notification\n";
    cout << "    2. Delete all notifications of a type\n";
    cout << "    3. Delete ALL my notifications\n";
    cout << "    0. Back\n";

    int ch = getIntInput("  Choice: ");

    if (ch == 1) {
        // Show all first
        auto notifs = mgr.getForUser(userId);
        if (notifs.empty()) {
            cout << "  No notifications to delete.\n";
            return;
        }
        printDivider();
        for (Notification* n : notifs) {
            cout << "  #" << left << setw(5) << n->getNotifId();
            n->display();
        }
        printDivider();

        int id = getIntInput("  Enter Notification ID to delete: ");
        Notification* n = mgr.findById(id);
        if (n && n->getRecipientId() == userId) {
            if (confirm("  Confirm delete notification #" + to_string(id) + "?")) {
                mgr.deleteById(id);
            }
        } else {
            cout << "  Notification not found or not yours.\n";
        }

    } else if (ch == 2) {
        NotificationType type = pickType();
        string typeName = Notification::typeToString(type);
        if (confirm("  Delete all '" + typeName + "' notifications?")) {
            mgr.deleteByType(userId, type);
        }

    } else if (ch == 3) {
        if (confirm("  Delete ALL your notifications? This cannot be undone")) {
            mgr.deleteAllForUser(userId);
        }
    }
}

// ── Filter by Type ────────────────────────────────────────────────────────────
void NotificationUI::menuFilter(int userId) {
    printHeader("Filter by Type", userId);

    NotificationType type = pickType();
    string typeName = Notification::typeToString(type);

    auto all = mgr.getForUser(userId);
    vector<Notification*> filtered;
    for (Notification* n : all) {
        if (n->getType() == type)
            filtered.push_back(n);
    }

    cout << "\n";
    printDivider();
    cout << "  " << typeName << " notifications (" << filtered.size() << "):\n";
    printDivider();

    if (filtered.empty()) {
        cout << "  None found.\n";
        return;
    }

    for (Notification* n : filtered) {
        cout << "  #" << left << setw(5) << n->getNotifId();
        n->display();
    }
    printDivider();
}