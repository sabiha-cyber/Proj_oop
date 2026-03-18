#ifndef GENERIC_MANAGER_H
#define GENERIC_MANAGER_H

#include <vector>
#include <string>

template <typename T>
class GenericManager {
protected:
    std::vector<T*> items;

public:
    virtual ~GenericManager() {
        for (T* item : items) delete item;
        items.clear();
    }

    void addItem(T* item) { if (item) items.push_back(item); }
    const std::vector<T*>& getItems() const { return items; }
    // ... add generic save/load if needed ...
};

#endif