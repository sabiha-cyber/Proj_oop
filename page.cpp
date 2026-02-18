#include "Page.h"
#include "User.h"
#include "Post.h"
#include <iostream>
#include<limits>

using namespace std;

Page::Page(const string& name) : pageName(name) {}

bool Page::addFollower(User* user)
{
    for (auto* u : followers)
        if (u == user)
            return false;

    followers.push_back(user);
    cout << user->getUsername() << " followed " << pageName << endl;
    return true;
}

void Page::addPost(Post* post)
{
    posts.push_back(post);
}

void Page::showPageInfo() const
{
    cout << "\nPage Name : " << pageName << endl;
    cout << "Followers : " << followers.size() << endl;
}

void Page::showTimeline() const
{
    cout << "\n===== " << pageName << " Timeline =====\n";
    for (auto* p : posts)
        p->viewPost();   // FROM Post.cpp
}

void Page::showLatestPost() const
{
    if (!posts.empty())
        posts.back()->viewPost();
}

void Page::showMenu(vector<User*>& users)
{
    int choice = -1;

    while (choice != 0)
    {
        cout << "\n=============================\n";
        cout << " Facebook Page Menu\n";
        cout << "=============================\n";
        cout << "1. View Page Info\n";
        cout << "2. View Timeline\n";
        cout << "3. Add Post\n";
        cout << "4. Follow Page\n";
        cout << "5. View Latest Post\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case 1:
            showPageInfo();
            break;

        case 2:
            showTimeline();
            break;

        case 3:
        {
            int uid;
            string text;

            cout << "Enter user ID: ";
            cin >> uid;
            cin.ignore();

            cout << "Enter post text: ";
            getline(cin, text);

            for (auto* u : users)
            {
                if (u->getUserId() == uid)
                {
                    Post* p = new Post(posts.size() + 1, u, text);
                    addPost(p);
                    break;
                }
            }
            break;
        }

        case 4:
       {
        int uid;
         cout << "Enter user ID: ";
        cin >> uid;

        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        bool found = false;

        for (auto* u : users)
        {
           if (u->getUserId() == uid)
            {
            addFollower(u);
            found = true;
            break;   // stop searching users
            }
        }

       if (!found)
           cout << "User not found!\n";

   
    //cin.ignore(numeric_limits<streamsize>::max(), '\n');

        break;  
        }
       


        case 5:
            showLatestPost();
            break;

        case 0:
            cout << "Exiting...\n";
            break;

        default:
            cout << "Invalid choice\n";
        }
    }
}
