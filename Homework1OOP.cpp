#define _CRT_SECURE_NO_WARNINGS
#include <iostream> 
#include <fstream>
#include <time.h>
#include "UserArray.h"
#include "BlockArray.h";

unsigned idCounter = 0;
unsigned idBlock = 0;

unsigned computeHash(const unsigned char* memory, int length) {
	unsigned hash = 0xbeaf;

	for (int c = 0; c < length; c++) {
		hash += memory[c];
		hash += hash << 10;
		hash ^= hash >> 6;
	}

	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;
	return hash;
}

int findIndex(TransactionBlock block) {
	int counter = 0;
	for (int i = 0; i < 16; i++) {
		if (block.transactions[i].coins < 0) {
			break;
		}
		++counter;
	}
	return counter;
}

double calculateCoins(unsigned id, BlockArray& blocks) {
	double coins = 0.0;
	for (int i = 0; i < blocks.getSize(); i++) {
		for (int j = 0; j < 16; j++) {
			if (blocks[i].transactions[j].receiver == id) {
				coins += blocks[i].transactions[j].coins;
			}
			else if (blocks[i].transactions[j].sender == id) {
				coins -= blocks[i].transactions[j].coins;
			}
		}
	}
	return coins;
}

void create_block(BlockArray& blocks) {
	TransactionBlock block;
	block.id = ++idBlock;
	block.prevBlockId = blocks[blocks.getSize() - 1].id;
	block.validTransactions = 0;
	int size = strlen(reinterpret_cast<const char*>(&blocks[blocks.getSize() - 1]));
	block.prevBlockHash = computeHash(reinterpret_cast<const unsigned char*>(&blocks[blocks.getSize() - 1]), size);
	blocks.push(block);
}

void send_coins(const unsigned& sender, const unsigned& receiver, double coins, BlockArray& blocks) {
	if (calculateCoins(sender, blocks) < coins) {
		std::cout << "Not enough coins to make a transfer! " << std::endl;
		return;
	}
	Transaction transaction;
	transaction.coins = coins;
	transaction.receiver = receiver;
	transaction.sender = sender;
	transaction.time = std::time(nullptr);
	int index = findIndex(blocks[blocks.getSize() - 1]);
	if (index == 16) {
		create_block(blocks);
	}
	blocks[blocks.getSize() - 1].transactions[index] = transaction;
	blocks[blocks.getSize() - 1].validTransactions += 1;
}

void create_user(UserArray& users, BlockArray& blocks) {
	std::cout << "Enter username" << std::endl;
	char name[128];
	std::cin.getline(name, 128);
	std::cout << "Enter startint investment" << std::endl;
	double sum = 0;
	std::cin >> sum;
	User user;
	strcpy_s(user.name, sizeof(user.name), name);
	user.id = ++idCounter;
	users.push(user);
	send_coins(0, user.id, sum*420, blocks);
}

unsigned getId(const char* name, UserArray& users) {
	for (int i = 0; i < users.getSize(); i++) {
		if (!strcmp(users[i].name, name)) {
			return users[i].id;
		}
	}
}

void remove_user(const char* name, UserArray& users, BlockArray& blocks) {
	unsigned id = getId(name, users);
	users.removeUser(name);
	double coins = calculateCoins(id , blocks);
	send_coins(id, 0, coins, blocks);
}

bool verify_transactions(BlockArray& blocks) {
	if (blocks.getSize() - 1 == 0) {
		return true;
	}

	for (int i = blocks.getSize() - 1; i > 0; i--) {
		int size = strlen(reinterpret_cast<const char*>(&blocks[i - 1]));
		if (blocks[i].prevBlockHash != computeHash(reinterpret_cast<const unsigned char*>(&blocks[i - 1]), size)) {
			std::cout << "Invalid hash detected!" << std::endl;
			return false;
		}
	}
	return true;
}

void wealthiest_users(const unsigned& num, UserArray& users, BlockArray& blocks) {
	char generatedName[128] = { '\0' };
	std::sprintf(generatedName, "wealthiest-users_%ld.txt", std::time(nullptr));
	std::ofstream file(generatedName);
	if (!(file.is_open())) {
		std::cout << "Wealthiest users file couldnt opne!" << std::endl;
		return;
	}
	UserArray sortedUsers(num);
	double maxCoins = 0;
	int index = 0;
	for (int i = 0; i < num; i++) {
		for (int j = 1; j < users.getSize(); j++) {
			bool flag = true;
			if (calculateCoins(users[j].id, blocks) > maxCoins) {
				for (int k = 0; k < sortedUsers.getSize(); k++) {
					if (sortedUsers[k].id == users[j].id) {
						flag = false;
						break;
					}
				}
				if (flag) {
					maxCoins = calculateCoins(users[j].id, blocks);
					index = j;
				}
			}
		}
		User tmp;
		tmp.id = users[index].id;
		strcpy_s(tmp.name, 128, users[index].name);
		sortedUsers.push(tmp);
		std::cout << "Wealthiest user #" << i + 1 << ": "<< tmp.name << std:: endl;
		file << "Wealthiest user #" << i + 1 << ": " << tmp.name << std::endl;
		std::cout << "The coins they posses are: " << maxCoins << std::endl;
		file << "The coins they posses are: " << maxCoins << std::endl;
		maxCoins = 0;
	}
}

void biggest_blocks(const unsigned& num, BlockArray& blocks) {
	char generatedName[128] = { '\0' };
	std::sprintf(generatedName, "biggest-blocks_%ld.txt", std::time(nullptr));
	std::ofstream file(generatedName);
	if (!(file.is_open())) {
		std::cout << "Biggest blocks file couldnt opne!" << std::endl;
		return;
	}
	BlockArray sortedBlocks(num);
	double maxCoins = 0;
	int index = 0;
	for (int i = 0; i < num; i++) {
		for (int j = 0; j < blocks.getSize(); j++) {
			bool flag = true;
			double sum = 0;
			for (int k = 0; k < 16; k++) {
				if (blocks[j].transactions[k].coins < 0) {
					break;
				}
				sum += blocks[j].transactions[k].coins;
			}
			if (maxCoins < sum) {
				for (int n = 0; n < sortedBlocks.getSize(); n++) {
					if (blocks[j].id == sortedBlocks[n].id) {
						flag = false;
						break;
					}
				}
				if (flag) {
					maxCoins = sum;
					index = j;
				}
			}
		}
		TransactionBlock tmp;
		tmp.id = blocks[index].id;
		sortedBlocks.push(tmp);
		std::cout << "Biggest block #" << i + 1 << ": Block number #" << tmp.id << std::endl;
		file << "Wealthiest user #" << i + 1 << ": Block number #" << tmp.id << std::endl;
		std::cout << "The coins that are in transactions are : " << maxCoins << std::endl;
		file << "The coins that are in transactions are : " << maxCoins << std::endl;
		maxCoins = 0;
	}
}

unsigned sizeOfArrayUsers() {
	std::ifstream usersFile("users.dat", std::ios::binary);
	if (!(usersFile.is_open())) {
		std::cout << "usersFile couldnt open!" << std::endl;
		return 0;
	}
	usersFile.seekg(0, std::ios::end);
	int sizeOfFile = usersFile.tellg();
	usersFile.seekg(0, std::ios::beg);
	return sizeOfFile / sizeof(User);
}

unsigned sizeOfArrayBlocks() {
	std::ifstream blocksFile("blocks.dat", std::ios::binary);
	if (!(blocksFile.is_open())) {
		std::cout << "blocksFile couldnt open!" << std::endl;
		return 0;
	}
	blocksFile.seekg(0, std::ios::end);
	int sizeOfFile = blocksFile.tellg();
	blocksFile.seekg(0, std::ios::beg);
	return sizeOfFile / sizeof(TransactionBlock);
}

void readUsers(UserArray& users) {
	std::ifstream file("users.dat", std::ios::binary);
	if (!(file.is_open())) {
		std::cout << "users.dat couldnt open!" << std::endl;
		return;
	}
	User user;
	for (int i = 0; i < sizeOfArrayUsers(); i++) {
		file.read(reinterpret_cast<char*>(&user), sizeof(user));
		users.push(user);
	}
}

void readBlocks(BlockArray& blocks) {
	std::ifstream file("blocks.dat", std::ios::binary);
	if (!(file.is_open())) {
		std::cout << "blocks.dat couldnt open!" << std::endl;
		return;
	}
	TransactionBlock block;
	for (int i = 0; i < sizeOfArrayBlocks(); i++) {
		file.read(reinterpret_cast<char*>(&block), sizeof(block));
		blocks.push(block);
	}
}


void exit(UserArray& users, BlockArray& blocks) {
	std::ofstream file1("idFile.dat", std::ios::binary);
	if (!(file1.is_open())) {
		std::cout << "idFile couldnt open!" << std::endl;
		return;
	}
	file1.write(reinterpret_cast<const char*>(&idCounter), sizeof(idCounter));
	file1.close();
	std::ofstream file2("idBlock.dat", std::ios::binary);
	if (!(file2.is_open())) {
		std::cout << "idBlock couldnt open!" << std::endl;
		return;
	}
	file2.write(reinterpret_cast<const char*>(&idBlock), sizeof(idBlock));
	file2.close();
	std::ofstream file3("users.dat", std::ios::binary);
	if (!(file3.is_open())) {
		std::cout << "users couldnt open!" << std::endl;
		return;
	}
	for (int i = 0; i < users.getSize(); i++) {
		file3.write(reinterpret_cast<const char*>(&users[i]), sizeof(users[i]));
	}
	file3.close();
	std::ofstream file4("blocks.dat", std::ios::binary);
	if (!(file4.is_open())) {
		std::cout << "blocks couldnt open!" << std::endl;
		return;
	}
	for (int i = 0; i < blocks.getSize(); i++) {
		file4.write(reinterpret_cast<const char*>(&blocks[i]), sizeof(blocks[i]));
	}
	file4.close();
}

void ifUsersIsEmpty(UserArray& users, BlockArray& blocks) {
	User systemUser;
	systemUser.id = idCounter;
	char name[11] = { 's', 'y','s','t','e','m','U','s','e','r', '\0'};
	strcpy_s(systemUser.name, 128, name);
	users.push(systemUser);
	Transaction transaction;
	transaction.receiver = 0;
	transaction.sender = 0;
	transaction.coins = DBL_MAX;
	transaction.time = std::time(nullptr);
	TransactionBlock block;
	block.id = 0;
	block.prevBlockHash = 0;
	block.prevBlockId = 0;
	block.transactions[0] = transaction;
	block.validTransactions = 1;
	blocks.push(block);
}

void printUsers(UserArray& users, BlockArray& blocks) {
	for (int i = 0; i < users.getSize(); i++) {
		std::cout << "#" << i << " " << users[i].name << " with id "<< users[i].id << " has coins: " << calculateCoins(users[i].id, blocks) << std::endl;
	}
}

unsigned userExists(UserArray& users, unsigned id) {
	for (int i = 0; i < users.getSize(); i++) {
		if (users[i].id == id) {
			return 1;
		}
	}
	return 0;
}

int main()
{
	std::ifstream idFile("idFile.dat", std::ios::binary);
	if (!(idFile.is_open())) {
		std::cout << "idFile couldnt open!" << std::endl;
		
	}
	else {
		idFile.read(reinterpret_cast<char*>(&idCounter), sizeof(idCounter));
		idFile.close();
	}

	std::ifstream idBlockFile("idBlock.dat", std::ios::binary);
	if (!(idBlockFile.is_open())) {
		std::cout << "idBlockFile couldnt open!" << std::endl;
		
	}
	else {
		idBlockFile.read(reinterpret_cast<char*>(&idBlock), sizeof(idBlock));
		idBlockFile.close();
	}
	
	UserArray users(1);
	BlockArray blocks(1);
	if (sizeOfArrayUsers() == 0 && sizeOfArrayBlocks() == 0) {
		ifUsersIsEmpty(users, blocks);
	}
	else {
		readUsers(users);
		readBlocks(blocks);
	}
	
	bool end = true;
	while (end) {
		char choice[64];
		std::cout << "Choseone of the following options: print-users/ create-user/ remove-user/ send-coins/ wealthiest-users/ biggest-blocks/ verify-transactions/ exit \n";
		std::cin.getline(choice, 64);

		if (!strcmp(choice, "create-user")) {
			create_user(users, blocks);
		}
		else if (!strcmp(choice, "print-users")) {
			printUsers(users, blocks);
		}
		else if (!strcmp(choice, "remove-user")) {
			std::cout << "Enter the name of the users you want to remove" << std::endl;
			char name[28];
			std::cin.getline(name, 28);
			remove_user(name, users, blocks);
		}
		else if (!strcmp(choice, "send-coins")) {
			std::cout << "Enter the id of the sender" << std::endl;
			unsigned sender = 0;
			while (true) {
				std::cin >> sender;
				if (sender > 0 && sender <= idCounter) {
					if (userExists(users, sender) == 1) {
						break;
					}
					else std::cout << "User with this id currently doesn't exist" << std::endl;
				}
				std::cout << "Enter a positive id please" << std::endl;
			}
			std::cout << "Enter the id of the receiver" << std::endl;
			unsigned receiver = 0;
			while (true) {
				std::cin >> receiver;
				if (receiver > 0 && receiver <= idCounter) {
					if (userExists(users, receiver) == 1) {
						break;
					}
					else std::cout << "User with this id currently doesn't exist" << std::endl;
				}
				std::cout << "Enter a positive id please" << std::endl;
			}
			std::cout << "Enter the amount of coins you want to transfer" << std::endl;
			double coins = 0.0;
			while (true) {
				std::cin >> coins;
				if (coins > 0.0) {
					break;
				}
				std::cout << "Enter a positive amount of couins please" << std::endl;
			}
			send_coins(sender, receiver, coins, blocks);
		}
		else if (!strcmp(choice, "wealthiest-users")) {
			std::cout << "Enter number of users to output" << std::endl;
			int n = 0;
			while (true) {
				std::cin >> n;
				if (n > 0) {
					break;
				}
				std::cout << "Enter a positive number please" << std::endl;
			}
			wealthiest_users(n, users, blocks);
		}
		else if (!strcmp(choice, "biggest-blocks")) {
			std::cout << "Enter number of blocks to output" << std::endl;
			int n = 0;
			while(true) {
				std::cin >> n;
				if (n > 0) {
					break;
				}
				std::cout << "Enter a positive number please" << std::endl;
			}
			std::cin >> n;
			biggest_blocks(n, blocks);
		}
		else if (!strcmp(choice, "verify-transactions")) {
			if (verify_transactions(blocks)) {
				std::cout << "The verification was comepleted successfully" << std::endl;
			}
			else std::cout << "The verification was unsuccessful!" << std::endl; 
		}
		else if (!strcmp(choice, "exit")) {
			exit(users, blocks);
			end = false;
		}
		else {
			std::cout << "You have entered something incorrect, please enter again!";
		}
		
	}
	return 0;
}

