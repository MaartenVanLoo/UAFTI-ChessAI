#pragma once
#include <cstdint>
#include <vector>
#include <iostream>
#include "../chess/ClassicBitBoard.h"
namespace chess{
	enum class TTtype { PV, CUT, ALL, None};
	struct TTentry {
		uint64_t hash = 0;
		//chess::Move move;
		uint32_t move;
		uint8_t ID = 0;
		int depth = INT_MAX;
		int eval = 0;
		TTtype type = TTtype::None;
		//possible? => evaluated if they are usefull
		//int alpha = INT_MAX;
		//int beta = INT_MIN;
	};
	
	class TranspositionTable
	{
	private:
		uint64_t maxEntries;
		std::vector<TTentry> TTable;
		uint64_t collisions = 0;
		uint64_t entries = 0;
		uint64_t pvloss = 0;
        uint64_t mask = 0; //Mask to compute index
	public:
		TranspositionTable();
		/// <summary>
		/// tablesize in MB.
		/// </summary>
		/// <param name="tableSize"></param>
		TranspositionTable(size_t tableSize);
		
		
		__forceinline void update(uint64_t key,uint8_t ID,TTtype type, int eval, int depth, Move& move) {
		    if (eval == INT_MAX || eval == INT_MIN){
		        //INT_MAX & INT_MIN => PVSRazor sometimes tries to push this when search is terminated due to time.
		        //Don't know why but this hopefully solves the problem. (it is stupid but it works)
		        return;
		        std::cout << "info wrong update\n";
		    }
            uint64_t index = key & this->mask;
            //uint64_t index = key % this->maxEntries;
			if (key != TTable[index].hash && TTable[index].hash != 0) {
				if (ID == TTable[index].ID) {
					this->collisions++;
				}
				this->entries--;
			}
			// replacement scheme = always replace unless => new entry has same key and lower depth OR
			//												 the existing entry is a PV of the current searchID;
			if (TTable[index].depth <= depth && key == TTable[index].hash && TTable[index].type != TTtype::PV) {
                //Update existing node
                TTable[index].eval = eval;
                TTable[index].depth = depth;
                Move::compress(move,TTable[index].move);
                TTable[index].type = type;
                TTable[index].ID = ID;
			}
			else {
				if (key != TTable[index].hash) this->entries++;
				if (TTable[index].type == TTtype::PV && TTable[index].ID==ID) {
					// Do not replace existing PV nodes of currentsearchID
					if (type == TTtype::PV) this->pvloss++; //A PV node could not be inserted due to an other PV node of the current search already present
					this->collisions++;
					return;
				}
				//push new entry
				TTable[index].hash = key;
				TTable[index].eval = eval;
				TTable[index].depth = depth;
                Move::compress(move,TTable[index].move);
				TTable[index].type = type;
				TTable[index].ID = ID;
			}
		}
		__forceinline bool get(uint64_t key, TTentry& entry, int depth)
		{
			uint64_t index = key % this->maxEntries;
			if (key != TTable[index].hash) {
				return false;
			}
			//if (depth > TTable[index].depth) return false; // node wasn't evaluated at current depth => this would invalidate the 'eval value' but won't invalidate the actual move
			// depth check shouldn't happen in hash table
			entry = TTable[index];
			return true;
		}
		__forceinline void clear(){
			TTable.clear();
			this->TTable.resize(maxEntries);
			this->collisions = 0;
			this->entries = 0;
			this->pvloss = 0;
		}

		/// <summary>
		/// Search if transposition table contains a given key.
		/// </summary>
		/// <param name="key">Hash value</param>
		/// <returns></returns>
		__forceinline bool contains(uint64_t key) {
			uint64_t index = key & this->mask;
			if (key != TTable[index].hash) {
				return false;
			}
			return true;
		}
		/// <summary>
		/// Adds extra condition, only return true if ID AND key are found
		/// </summary>
		/// <param name="key">Hash value</param>
		/// <param name="ID">search ID</param>
		/// <returns></returns>
		__forceinline bool contains(uint64_t key, uint8_t ID) {
			uint64_t index = key & this->mask;
			if (key != TTable[index].hash || ID != TTable[index].ID) {
				return false;
			}
			return true;
		}
		//next methods assumes key is present
		__forceinline int eval(uint64_t key, uint8_t ID) {
			//TTable[key % this->maxEntries].ID = ID;
			return TTable[key & this->mask].eval;
		}
		__forceinline int depth(uint64_t key, uint8_t ID) {
			//TTable[key % this->maxEntries].ID = ID;
			return TTable[key & this->mask].depth;
		}
		__forceinline TTtype type(uint64_t key, uint8_t ID) {
			//TTable[key % this->maxEntries].ID = ID;
			return TTable[key & this->mask].type;
		}
		__forceinline Move move(uint64_t key, uint8_t ID) {
			//TTable[key % this->maxEntries].ID = ID;
			Move move;
			Move::decompress(TTable[key & this->mask].move, move);
            return move;
		}


		uint64_t getCollisions() const{ return this->collisions; }
		uint64_t getpvloss() const{ return this->pvloss; }
		void clearCollisions() { collisions = 0; pvloss = 0; }
		uint64_t getEntries() const{ return this->entries; }
		size_t size() { return TTable.size(); }
		
	};


}


