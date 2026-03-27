CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

TARGET = main

# ── Source files ──────────────────────────────────────────────────────────────
SRCS = main.cpp \
       AuthenticationService.cpp \
       Comment.cpp \
       CommentManager.cpp \
       ContentItem.cpp \
       FriendService.cpp \
       Like.cpp \
       LikeManager.cpp \
       newsfeed.cpp \
       notification.cpp \
       NotificationManager.cpp \
       NotificationUI.cpp \
       page.cpp \
       Pagemanager.cpp \
       Post.cpp \
       PostManager.cpp \
       User.cpp \
       UserManager.cpp

OBJS = $(SRCS:.cpp=.o)

# ── Default target ────────────────────────────────────────────────────────────
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# ── Pattern rule: compile each .cpp to .o ────────────────────────────────────
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ── Header dependencies ───────────────────────────────────────────────────────
main.o:                 main.cpp \
                        User.h UserManager.h \
                        AuthenticationService.h \
                        newsfeed.h \
                        Post.h PostManager.h \
                        Comment.h CommentManager.h \
                        Like.h LikeManager.h \
                        FriendService.h \
                        notification.h NotificationManager.h NotificationUI.h \
                        page.h Pagemanager.h \
                        messenger_system.h messenger_manager.h messenger_ui.h \
                        SocialExceptions.h GenericManager.h

AuthenticationService.o: AuthenticationService.cpp AuthenticationService.h \
                         User.h SocialExceptions.h

Comment.o:              Comment.cpp Comment.h ContentItem.h SocialExceptions.h

CommentManager.o:       CommentManager.cpp CommentManager.h Comment.h \
                        GenericManager.h SocialExceptions.h

ContentItem.o:          ContentItem.cpp ContentItem.h

FriendService.o:        FriendService.cpp FriendService.h User.h SocialExceptions.h

Like.o:                 Like.cpp Like.h ContentItem.h

LikeManager.o:          LikeManager.cpp LikeManager.h Like.h \
                        GenericManager.h SocialExceptions.h

newsfeed.o:             newsfeed.cpp newsfeed.h Post.h Comment.h Like.h \
                        User.h SocialExceptions.h

notification.o:         notification.cpp notification.h User.h

NotificationManager.o:  NotificationManager.cpp NotificationManager.h \
                        notification.h GenericManager.h SocialExceptions.h

NotificationUI.o:       NotificationUI.cpp NotificationUI.h \
                        NotificationManager.h notification.h

page.o:                 page.cpp page.h Post.h User.h SocialExceptions.h

Post.o:                 Post.cpp Post.h ContentItem.h User.h SocialExceptions.h

PostManager.o:          PostManager.cpp PostManager.h Post.h \
                        GenericManager.h SocialExceptions.h

User.o:                 User.cpp User.h SocialExceptions.h

UserManager.o:          UserManager.cpp UserManager.h User.h \
                        GenericManager.h SocialExceptions.h

Pagemanager.o:          Pagemanager.cpp Pagemanager.h page.h User.h \
                        AuthenticationService.h SocialExceptions.h

# ── Utility targets ───────────────────────────────────────────────────────────
clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all

run: all
	./$(TARGET)

.PHONY: all clean rebuild run
