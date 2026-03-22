// Updated ContentItem.h (ensure members are protected for derived access)
#ifndef CONTENT_ITEM_H
#define CONTENT_ITEM_H

#include <string>
#include <ctime>
#include <iosfwd>

class User;

class ContentItem {
protected:
    User* author;
    std::string text;
    bool isDeleted;
    time_t createdAt;

public:
    ContentItem(User* auth, const std::string& cont);
    virtual ~ContentItem() = default;

    // Abstract methods
    virtual void display() const = 0;
    virtual void edit(const std::string& newText) = 0;

    // Common methods
    void deleteItem();
    User* getAuthor() const;
    std::string getText() const;
    time_t getCreationTime() const;
    bool isDeletedItem() const;
    virtual std::string getType() const { return "ContentItem"; }
};

#endif