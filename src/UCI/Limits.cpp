#include "Limits.h"

UCI::Limits::Limits()
{
}

void UCI::Limits::setDefault()
{
	this->depth = -1;
	this->startTime = std::chrono::steady_clock::now();
	this->btime = -1;
	this->wtime = -1;
	this->winc = -1;
	this->binc = -1;
	this->movetime = -1;

	this->maxSearchTime = -1;
	this->lastItt = -1;
	this->elapsed = 0;

}

long long UCI::Limits::getElapsed()
{
	this->elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count();
	return this->elapsed;
}
long long UCI::Limits::elapsedSinceStart() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count();
}
long long UCI::Limits::elapsedMicrosecondsSinceStart() {
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime).count();
}
void UCI::Limits::startSearch(bool side)
{
    startTime = std::chrono::steady_clock::now();
	startItt = std::chrono::steady_clock::now();

	//Compute maxSearchTime
	if (movetime != -1) {
		maxSearchTime = std::min(movetime, (long)30000);
	}
	else if (side) {
		//current player = white;
		if (wtime != -1) {
			maxSearchTime = std::min(wtime / 20, (long)30000);
		}
		else {
			maxSearchTime = -1;
		}
	}
	else {
		//current player = black;
		if (btime != -1) {
			maxSearchTime = std::min(btime / 20, (long)30000);
		}
		else {
			maxSearchTime = -1;
		}
	}
}
void UCI::Limits::nextItt()
{
	lastItt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startItt).count();
	startItt = std::chrono::steady_clock::now();
}
bool UCI::Limits::exeeded(int depth)
{
	if (depth > this->depth && this->depth != -1) return true;
	elapsed =  std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count();
	if (this->maxSearchTime != -1) {
		if (elapsed >= maxSearchTime) return true;
		//if last itteration took longer than 1/4th of the remaining time => finish search, next itt will probably not be able to finish;
		if (lastItt > (maxSearchTime - elapsed) / 4) return true;
	}
	return false;
}
bool UCI::Limits::exeededTime() {
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count();
	if (elapsed >= this->maxSearchTime && this->maxSearchTime != -1) return true;
	return false;
}
