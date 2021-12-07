#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "ClassicBitBoard.h"
#include <thread>
namespace chess {
#pragma pack(push, 1)
	struct PolyglotEntry {
		uint64_t hash;
		uint16_t move;
		uint16_t weight;
		uint32_t learn;
	};
#pragma pack(pop)
	struct MoveEntry {
		MoveEntry(uint16_t move, uint16_t weight);
		uint16_t move;
		uint16_t weight;
	};
	struct BookEntry {
		uint64_t hash = 0;
		uint64_t totalWeight = 0; //total count of all weights in data
		std::vector<MoveEntry> data;
	};

	class Polyglot
	{
		std::vector<BookEntry> book;
		
	public:
		Polyglot();
		Polyglot(const std::string& path);
		void loadBook(const std::string& path);
		/// <summary>
		/// If a matching hash is found a random move will be returned. The chance for a move to be returned is based on their weight.
		/// </summary>
		/// <param name="hash">Polyglot hash.</param>
		/// <param name="move">Decompressed move.</param>
		/// <returns></returns>
		bool getMove(const uint64_t& hash, std::string& move);
		bool getMove(const uint64_t& hash, Move& move, ClassicBitBoard& board);
		void printBook();

		size_t size();
	private:
		std::string toLAN(uint16_t move);
	};
	class PolyglotPonder {
		std::vector<std::pair<uint64_t, Move>> ponderlist;
		Polyglot book;
	public:
		PolyglotPonder();
		PolyglotPonder(Polyglot& book);
		void computePonder(ClassicBitBoard board);
		std::thread computePonderAsync(ClassicBitBoard board);
		bool getPonder(uint64_t key,Move& move);		
	};
}

