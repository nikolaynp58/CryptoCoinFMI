#pragma once

struct User {
	unsigned id;
	char name[128];
};

class UserArray {
public:
	UserArray();
	UserArray(unsigned);
	UserArray(const UserArray&);
	~UserArray();
	UserArray& operator=(const UserArray&);
	User& operator[](unsigned);
	void push(const User&);
	unsigned getSize();
	void removeUser(const char*);
private:
	User* data;
	unsigned size;
	unsigned cappacity;
};