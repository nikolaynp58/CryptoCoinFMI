#include "UserArray.h"
#include <iostream>

UserArray::UserArray(): data(nullptr), size(0), cappacity(0) {}

UserArray::UserArray(unsigned size): data(new User[size*2]), size(0), cappacity(size*2) {}

UserArray::UserArray(const UserArray& other) :data(new User[other.cappacity]), size(other.size), cappacity(other.cappacity) {
	for (int i = 0; i < other.size; i++) {
		this->data[i] = other.data[i];
	}
}

UserArray::~UserArray() {
	delete[] data;
}

UserArray& UserArray::operator=(const UserArray& other) {
	if (this != &other) {
		delete[] this->data;
		this->data = new User[other.cappacity];
		this->size = other.size;
		this->cappacity = other.cappacity;
		for (int i = 0; i < other.size; i++) {
			this->data[i] = other.data[i];
		}
	}
	return *this;
}

User& UserArray::operator[](unsigned index) {
	if (index < 0 || index > this->cappacity) {
		///exception?
	}
	return data[index];
}

void UserArray::push(const User& newUser) {
	if (this->size == this->cappacity) {
		User* tmp = new User[this->cappacity * 2];
		for (int i = 0; i < this->size; i++) {
			tmp[i] = this->data[i];
		}
		delete[] this->data;
		this->data = tmp;
		this->cappacity = cappacity * 2;
	}
	this->data[size] = newUser;
	size++;
}

void UserArray::removeUser(const char* name) {
	for (int i = 0; i < this->size; i++) {
		if (!strcmp(name, data[i].name)) {
			for (int j = i; j < this->size - 1; j++) {
				data[j].id = data[j + 1].id;
				this->data[j] = this->data[j + 1];
			}
		}
	}
	this->size--;
	return;
}

unsigned UserArray::getSize() {
	return this->size;
}