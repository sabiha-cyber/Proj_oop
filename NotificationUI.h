#pragma once

#include "NotificationManager.h"
#include "Notification.h"
#include <string>

// ── NotificationUI ────────────────────────────────────────────────────────────
// Provides a full terminal-based interactive notification panel.
// Call showNotificationPanel(userId) from your logged-in user menu.
// ─────────────────────────────────────────────────────────────────────────────

class NotificationUI {
public:
    explicit NotificationUI(NotificationManager& manager);

    // ── Main entry point — call this from your user menu ──────────────────────
    // Shows the bell badge + launches the interactive panel
    void showNotificationPanel(int userId, const std::string& username);

    // ── Bell badge only (for showing in main menu header) ─────────────────────
    void showBadge(int userId) const;

private:
    NotificationManager& mgr;

    // ── Sub-menus ─────────────────────────────────────────────────────────────
    void menuMain        (int userId, const std::string& username);
    void menuViewAll     (int userId);
    void menuViewUnread  (int userId);
    void menuMarkRead    (int userId);
    void menuDelete      (int userId);
    void menuFilter      (int userId);

    // ── Helpers ───────────────────────────────────────────────────────────────
    void printHeader     (const std::string& title, int userId) const;
    void printDivider    () const;
    int  getIntInput     (const std::string& prompt) const;
    bool confirm         (const std::string& prompt) const;
    NotificationType pickType() const;
};