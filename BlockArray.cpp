#include "BlockArray.h"
#include <iostream>

BlockArray::BlockArray() : data(nullptr), size(0), cappacity(0) {}

BlockArray::BlockArray(unsigned size) : data(new TransactionBlock[size*2]), size(0), cappacity(size*2) {}

BlockArray::BlockArray(const BlockArray& other) :data(new TransactionBlock[other.cappacity]), size(other.size), cappacity(other.cappacity) {
	for (int i = 0; i < this->size; i++) {
		this->data[i] = other.data[i];
	}
}

BlockArray::~BlockArray() {
	delete[] data;
}

BlockArray& BlockArray::operator=(const BlockArray& other) {
	if (this != &other) {
		delete[] this->data;
		this->data = new TransactionBlock[other.cappacity];
		this->size = other.size;
		this->cappacity = other.cappacity;
		for (int i = 0; i < other.size; i++) {
			this->data[i] = other.data[i];
		}
	}
	return *this;
}

TransactionBlock& BlockArray::operator[](unsigned index) {
	if (index < 0 || index > this->cappacity) {
		///exception?
	}
	return data[index];
}

void BlockArray::push(const TransactionBlock& newBlock) {
	if (this->size == this->cappacity) {
		TransactionBlock* tmp = new TransactionBlock[this->cappacity * 2];
		for (int i = 0; i < this->size; i++) {
			tmp[i] = this->data[i];
		}
		delete[] this->data;
		this->data = tmp;
		this->cappacity = cappacity * 2;
	}
	data[size] = newBlock;
	size++;
}

unsigned BlockArray::getSize() {
	return this->size;
}