#include "TranspositionTable.h"
#include <iostream>

chess::TranspositionTable::TranspositionTable() {
    this->maxEntries = 1ull<<22;
    this->mask = maxEntries -1;
	this->TTable.resize(maxEntries);
}
chess::TranspositionTable::TranspositionTable(size_t tableSize)
{
    this->maxEntries = 1ull<<22;
    this->mask = maxEntries -1;
	this->TTable.resize(maxEntries);
}

