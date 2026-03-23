#include "ContentItem.h"
#include <ctime>

ContentItem::ContentItem(User* auth, const std::string& cont)
    : author(auth), text(cont), isDeleted(false), createdAt(std::time(nullptr)) {}

void ContentItem::deleteItem() { isDeleted = true; }

User* ContentItem::getAuthor() const { return author; }
std::string ContentItem::getText() const { return text; }
time_t ContentItem::getCreationTime() const { return createdAt; }
bool ContentItem::isDeletedItem() const { return isDeleted; }   