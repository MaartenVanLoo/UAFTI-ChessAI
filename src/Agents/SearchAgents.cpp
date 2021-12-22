#include "SearchAgents.h"

namespace chess{


    int SearchAgents::mateIn(int searchdepth, int value) {
        //Mate value = mateValue + depth
        //with depth = searchdepth - ply
        //<=> value = mateValue + searchdepth - ply
        //<=> ply = mateValue + searchdepth - value
        //Number of "moves" not "plies" => integer division by 2
        if (value >= mate_Value) {// Positive mate depth
            return (mate_Value + searchdepth - value)/2;
        }else if (value <= -mate_Value){
            return -(mate_Value + searchdepth  + value) / 2; //- (-value) == + value
        }
        else{
            return 0;
        }

    }

    int SearchAgents::isMate(int value) {
        if (std::abs(value)>=mate_Value){
            return true;
        }else{
            return false;
        }
    }

    void SearchAgents::PVSRazoring::printIteration(int depth, int value, Move& bestMove, Move& ponder, int is_mate, int mate_in) {
        if (is_mate){
            std::cout << "info depth " << depth << " time " << limits.getElapsed() << " nodes " << this->nodes
                      << " score mate " << mate_in << " pv " << bestMove << " " << ponder << std::endl;
            this->logFile << "info depth " << depth << " time " << limits.getElapsed() << " nodes " << this->nodes
                          << " score mate " << mate_in << " pv " << bestMove << " " << ponder << std::endl;
        }else {
            std::cout << "info depth " << depth << " time " << limits.getElapsed() << " nodes " << this->nodes
                      << " score cp " << value << " pv " << bestMove << " " << ponder << std::endl;
            this->logFile << "info depth " << depth << " time " << limits.getElapsed() << " nodes " << this->nodes
                          << " score cp " << value << " pv " << bestMove << " " << ponder << std::endl;
        }
    }

    void SearchAgents::PVSRazoring::printFinalValues(int depth, int value, Move& bestMove, Move& ponder, int is_mate, int mate_in) {
        long long unsigned nps = this->nodes;
        if (limits.getElapsed() > 0) {
            nps = (uint64_t)(this->nodes * 1e3 / limits.getElapsed());
        }
        if (is_mate){
            std::cout << "info depth " << depth
                      << " nodes " << this->nodes
                      << " nps " << nps
                      << " mates " << this->mates
                      << " draws " << this->draws
                      << " score mate " << mate_in
                      << " time " << limits.getElapsed()
                      << std::endl;
            this->logFile << "info depth " << depth
                          << " nodes " << this->nodes
                          << " nps " << nps
                          << " mates " << this->mates
                          << " draws " << this->draws
                          << " score mate " << mate_in
                          << " time " << limits.getElapsed()
                          << std::endl;
        }else {
            std::cout << "info depth " << depth
                      << " nodes " << this->nodes
                      << " nps " << nps
                      << " mates " << this->mates
                      << " draws " << this->draws
                      << " score cp " << value
                      << " time " << limits.getElapsed()
                      << std::endl;
            this->logFile << "info depth " << depth
                          << " nodes " << this->nodes
                          << " nps " << nps
                          << " mates " << this->mates
                          << " draws " << this->draws
                          << " score cp " << value
                          << " time " << limits.getElapsed()
                          << std::endl;
        }
    }
}
