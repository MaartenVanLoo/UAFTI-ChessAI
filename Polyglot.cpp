#include "Polyglot.h"
#include <fstream>
#include <iostream>
#include <iomanip>

chess::Polyglot::Polyglot()
{
	srand((unsigned int)time(0)); //init random number generator;
	book.reserve(1000);
}

chess::Polyglot::Polyglot(const std::string& path)
{
	loadBook(path);
}

void chess::Polyglot::loadBook(const std::string& path)
{
	std::ifstream file;
	std::ifstream file2;
	file.open(path, std::ifstream::in | std::ifstream::binary);
	file2.open(path, std::ifstream::in | std::ifstream::binary);
	//std::ifstream file{path};
	char buffer[16];
	PolyglotEntry entry;
	BookEntry temp;
	std::cout << sizeof(entry) << std::endl;
	std::cout << sizeof(buffer) << std::endl;
	if (!file.fail()) {
		while (!file.eof())
		{
			file2.read((char*)buffer, sizeof(buffer));
			//file.close();
			//file.open(path, std::ios::in | std::ios::binary);
			file.read((char*)&entry, sizeof(entry));
			
			//file.read((char*)&entry.hash, sizeof(entry.hash));
			//file.read((char*)&entry.move, sizeof(entry.move));
			//file.read((char*)&entry.weight, sizeof(entry.weight));
			//file.read((char*)&entry.learn, sizeof(entry.learn));
			entry.hash = _byteswap_uint64(entry.hash);
			entry.move = _byteswap_ushort(entry.move);
			entry.weight = _byteswap_ushort(entry.weight);
			entry.learn = _byteswap_ulong(entry.learn);
			
			if (temp.hash == entry.hash) {
				temp.totalWeight += entry.weight;
				temp.data.push_back(MoveEntry(entry.move,entry.weight));
			}
			else {
				this->book.push_back(temp); // store previous
				temp = BookEntry();  //clear temp
				temp.hash = entry.hash;
				temp.totalWeight += entry.weight;
				temp.data.push_back(MoveEntry(entry.move, entry.weight)); // add current to temp
			}
		}
		//this->book.push_back(temp); // drop last; somehow goes wrong see gm2600.bin las entry = invallid move????
		file.close();
	}
}

bool chess::Polyglot::getMove(const uint64_t& hash, std::string& move)
{
	if (book.size() == 0) return false;
	//binary search:
	int64_t low = 0;
	int64_t high = this->book.size();
	int64_t mid;
	int64_t target = -1;
	while (low <= high) {
		mid = (low + high) / 2;
		if (this->book[mid].hash < hash) {
			low = mid + 1;
		}
		else if (this->book[mid].hash > hash){
			high = mid - 1;
		}
		else {
			target = mid; 
			break;
		}
	}
	if (target == -1) {
		//No hit => return nothing
		move = "";
		return false;
	}
	else {
		//Compute chance
		int index = rand() % this->book[target].totalWeight;
		// go over possibilities:
		for (auto& m : this->book[target].data) {
			if (index < m.weight) {
				move = toLAN(m.move);
				return true;
			}
			index -= m.weight;
		}
	}
	//Normally this part of the code should not be reached.
	move = ""; 
	return false;
}
bool chess::Polyglot::getMove(const uint64_t& hash, Move& move, ClassicBitBoard& board) {
	std::string m;
	if (getMove(hash, m)) {
		move = board.LAN2Move(m);
		return true;
	}
	move = Move();
	return false;
}
void chess::Polyglot::printBook()
{
	for (auto& e : book) {
		std::cout << std::setfill('0') << std::setw(16) << std::hex << e.hash << "\n";
		for (auto& data : e.data) {
			std::cout << "\t" << std::setw(4) << std::hex << toLAN(data.move) << "\t" << std::dec << data.weight << "\t" << "\n";
		}
		uint64_t h = 0x823c9b50fd114196;
		//uint64_t h = 0x964111fd509b3c82;
		
		//h = 4287009119873091189;
		if (e.hash == h) {
			system("pause");
		}
		 
	}
	std::cout.flush();
}

size_t chess::Polyglot::size()
{
	return book.size();
}

std::string chess::Polyglot::toLAN(uint16_t move)
{
	// http://hgm.nubati.net/book_format.html
	int to_file = move & 0b111;
	int to_rank= (move>>3) & 0b111;
	int from_file = (move >> 6) & 0b111;
	int from_rank = (move >> 9) & 0b111;
	int promote = (move >> 12) &  0b111;
	std::string lan;
	lan += "abcdefgh"[from_file]; //Note: different numbering compared to ClassicBitBoard!
	lan += "12345678"[from_rank];
	lan += "abcdefgh"[to_file]; //Note: different numbering compared to ClassicBitBoard!
	lan += "12345678"[to_rank];
	
	if (promote) {
		lan += "XnbrqXX"[promote];
	}
	return lan;
}

chess::MoveEntry::MoveEntry(uint16_t move, uint16_t weight) : move(move), weight(weight) {}

chess::PolyglotPonder::PolyglotPonder()
{
	this->book = Polyglot();
}

chess::PolyglotPonder::PolyglotPonder(Polyglot& book) : book(book)
{}

void chess::PolyglotPonder::computePonder(ClassicBitBoard board)
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	
	//clear prev ponder
	ponderlist.clear();
	//create a copy of current board (this method must run independant of the parrant)
	//ClassicBitBoard brd = board;  => function call without reference enough?

	//go over every possible move and check if the result is in book;
	Move move;
	std::string ponder;
	std::vector<Move> moves;
	board.generate_moves(moves);
	for (auto& m : moves) {
		board.makeMove(m);
		uint64_t key = chess::ClassicBitBoard::HashUtil::createHash(board);
		if (book.getMove(key, ponder)) {
			this->ponderlist.push_back(std::pair<uint64_t, Move>(key, board.LAN2Move(ponder)));
		}
		board.undoMove();
	}
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "ponder time: " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << std::endl;
}

std::thread chess::PolyglotPonder::computePonderAsync(ClassicBitBoard board) {
	//return std::thread(&computePonder, board);
	return std::thread(&PolyglotPonder::computePonder, this,board);
}
bool chess::PolyglotPonder::getPonder(uint64_t key, Move& move)
{
	for (auto& m : this->ponderlist) {
		if (m.first == key) {
			std::cout << "info bookhit" << std::endl;
			move = m.second;
			return true;
		}
	}
	return false;
}
