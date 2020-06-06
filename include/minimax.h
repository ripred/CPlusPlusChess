//
// minimax.h
//
// minimax algorithm implemented in c++ for chess board evaluation
//

#pragma once

#include <bestmove.h>
#include <board.h>
#include <move.h>

#include <chrono>
#include <mutex>

namespace chess {
  using std::mutex;
  using std::chrono::steady_clock;

  class Minimax {
  public:
    steady_clock::time_point startTime;
    long movesExamined;
    bool extraChecks;
    unsigned reserve;
    bool useThreads;
    int qMaxDepth;
    bool useCache;
    BestMove best;
    int maxDepth;
    int timeout;

    explicit Minimax(int max_depth = 0);

    Move bestMove(Board const &board);

    /**
     * Iterate over all available moves for the current player and decide which move is the best.
     * This executes on the current thread and is a blocking call.
     *
     * @param board the board state to examine each move on
     * @param pieceMap board pieces mapped by type and side
     * @return the best move for this board
     */
    Move searchWithNoThreads(Board const &board, bool maximize, PieceMap &pieceMap);

    // Search With threads
    Move searchWithThreads(Board const &board, bool maximize, PieceMap &pieceMap);

    /**
     * The awesome, one and only, minimax algorithm method which recursively searches
     * for the best moves up to a certain number of moves ahead (plies) or until a
     * move timeout occurs (if any timeouts are in effect).
     *
     * @param origBoard the board state to examine all moves for
     * @param alpha     the lower bounds of the best move and score found so far
     * @param beta      the upper bounds of the best move and score found so far
     * @param depth     the number of plies to search ahead.  Ply is a 'half-turn'
     *                  where one player has moved but the responding move has not
     *                  been made.  A full turn for fair evaluation usually requires
     *                  a balanced number of exchanges.
     * @param maximize  true if we are looking for a board state with the maximum score (white
     * player's turn) false if we are looking for a board state with the lowest score (black
     * player's turn)
     * @return the best score this move (and all consequential response/exchanges up to the allowed
     *         look-ahead depth or time limit for searching).
     */
    int minmax(Board &origBoard, int alpha, int beta, int depth, bool maximize);
  };

  struct ThreadArgs {
    Board const &board;
    Move const &move;
    Minimax &agent;
    int depth;
    bool maximize;

    ThreadArgs() = delete;
    ThreadArgs(Board const &b, Move const &m, Minimax &mm, int d, bool max);
  };

  struct ThreadResult {
    int value;
    Move move;

    ThreadResult();
    ThreadResult(int i, Move const &m);
  };

}  // namespace chess
