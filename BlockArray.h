#pragma once


struct Transaction {
	unsigned sender;
	unsigned receiver;
	double coins;
	long long time;
};

struct TransactionBlock {
	unsigned id;
	unsigned prevBlockId;
	unsigned prevBlockHash;
	int validTransactions;
	Transaction transactions[16];
};

class BlockArray {
public:
	BlockArray();
	BlockArray(unsigned);
	BlockArray(const BlockArray&);
	~BlockArray();
	BlockArray& operator=(const BlockArray&);
	TransactionBlock& operator[](unsigned);
	void push(const TransactionBlock&);
	unsigned getSize();
private:
	TransactionBlock* data;
	unsigned size;
	unsigned cappacity;
};