# LocalO (OOP Project)

A simple console-based social networking app built in C++ (C++17). This project was created to practice core OOP concepts like inheritance, polymorphism, templates, and exception handling in a practical way.

---

## Overview

LocalO is like a mini social media platform that runs in the terminal. Users can create accounts, log in, make posts, like and comment, manage friends, follow pages, and see a personalized news feed.

All the data is stored in text files, so the work is saved even after we close the program.

---

## Features

- User registration and login  
- Create, edit, and delete posts  
- Like and comment on posts (no double likes allowed)  
- Send and manage friend requests  
- Create and follow pages  
- News feed based on activity (likes, comments, etc.)  
- Notifications for interactions  
- Data saved in files (no database needed)  

---

## Project Structure

The project is divided into different parts so everything stays organized:

- User & Authentication → handles login and user data  
- Posts, Likes, Comments → core social features  
- Friends → friend requests and connections  
- Notifications → alerts for user activity  
- NewsFeed → shows posts in a ranked way  
- Pages → public content areas  
- Messenger → basic chat system  

---

## Class Design

### Inheritance

ContentItem (base class)  
→ Post  
→ Comment  

### Template Usage

GenericManager  
→ PostManager  
→ LikeManager  
→ CommentManager  

This helps reuse code and keep things clean.

---

## OOP Concepts Used (In Simple Words)

- Inheritance → Post and Comment share common features from ContentItem  
- Polymorphism → Different objects behave differently using the same interface  
- Encapsulation → Data is kept private and accessed through functions  
- Abstraction → Complex logic is hidden behind simple functions  
- Templates → One manager class works for multiple data types  
- Exception Handling → Custom errors for safer execution  

---

## How to Run

### Requirements

- g++ (C++17 supported)  
- make  

### Compile

make  

### Run

./main  

---

## How It Works (Step-by-Step)

1. User registers or logs in  
2. After login, user can:
   - View news feed  
   - Add/manage friends  
   - Create posts  
   - Like/comment  
   - Check notifications  
3. All actions are saved in files  

---

## Data Storage

Instead of a database, this project uses simple text files:

- users.txt → user info  
- posts.txt → posts  
- likes.txt → likes  
- comments.txt → comments  
- friends.txt → friend list  
- notifications.txt → notifications  
- pages.txt → pages  
- conversations.csv → chat data  

---

## Exception Handling

Custom exceptions are used to handle errors like:

- Invalid input (empty content, etc.)  
- File read/write issues  
- User not found  
- Unauthorized actions  

This makes the program more stable and easier to debug.

---

## Limitations

- No graphical interface (only terminal)  
- No database (file-based only)  
- Not designed for very large data  

---

## Future Improvements

- Add GUI or web interface  
- Use database  
- Improve performance  
- Add better security (password encryption)  
- Add advanced features  

---

## Author

This project was developed as part of learning Object-Oriented Programming in C++ and building a real-world style application.

---

## Note

Make sure all required text files are present before running the program.

This project is for learning purposes.
