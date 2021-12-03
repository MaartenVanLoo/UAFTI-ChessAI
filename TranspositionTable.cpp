#include "TranspositionTable.h"
#include <iostream>

chess::TranspositionTable::TranspositionTable() {
	this->TTable.resize(32 * (1ull << 20) / sizeof(TTentry));
	this->maxEntries = 32 * (1ull << 20) / sizeof(TTentry);
}
chess::TranspositionTable::TranspositionTable(size_t tableSize)
{
	this->TTable.resize(tableSize * (1ull << 20) / sizeof(TTentry));
	this->maxEntries = tableSize * (1ull << 20) / sizeof(TTentry);
}

