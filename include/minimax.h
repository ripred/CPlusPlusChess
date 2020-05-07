//
// minimax.h
//
// minimax algorithm implemented in c++ for chess board evaluation
//

#pragma once

#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

#include <bestmove.h>
#include <board.h>
#include <chessutil.h>
#include <evaluator.h>
#include <move.h>

using namespace chess;

typedef unsigned int Bits;
typedef vector<Bits> PieceList;
typedef map<Bits, PieceList> SidePieceMap;
typedef map<Bits, SidePieceMap> PieceMap;

#include <iostream>
using std::cout, std::endl;

class Minimax {
public:
  int maxDepth;
  int qMaxDepth;
  BestMove best;
  long movesProcessed;

  explicit Minimax(int max_depth = 0) : best(true) {
    maxDepth = max_depth;
    qMaxDepth = 0;
    movesProcessed = 0L;
  }

  Move bestMove(Board const& board) {
    bool maximize = (board.turn == White);
    best = BestMove(maximize);
    movesProcessed = 0L;

    if (board.moves1.size() == 1) {
      best.move = board.moves1[0];
      best.value = best.move.getValue();
      return best.move;
    }

    if (board.moves1.empty()) {
      return best.move;
    }

    PieceMap pieceMap;

    if (false) {
      for (int ndx = 0; ndx < BOARD_SIZE; ndx++) {
        if (board.isEmpty(ndx)) continue;
        Bits bits = board.board[ndx];
        pieceMap[getSide(bits)][getType(bits)].push_back(ndx);
      }
    }

    // no-thread version implemented first
    return searchWithNoThreads(board, maximize, pieceMap);
  }

  /**
   * Iterate over all available moves for the current player and decide which move is the best.
   * This executes on the current thread and is a blocking call.
   *
   * @param board the board state to examine each move on
   * @param pieceMap board pieces mapped by type and side
   * @return the best move for this board
   */
  Move searchWithNoThreads(Board const& board, bool maximize, PieceMap& pieceMap) {
    // We are not using threads.
    // Walk through all moves and find the best and return it in this calling thread.

    UNUSED(pieceMap);

    for (Move const& move : board.moves1) {
      Board currentBoard(board);
      currentBoard.executeMove(move);
      currentBoard.advanceTurn();
      movesProcessed++;

      int lookAheadVal = minmax(currentBoard, MIN_VALUE, MAX_VALUE, maxDepth, !maximize);

      if ((maximize && lookAheadVal > best.value) || (!maximize && lookAheadVal < best.value)) {
        best.value = lookAheadVal;
        best.move = move;
        best.move.setValue(best.value);
      }
    }
    return best.move;
  }

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
   * player's turn) false if we are looking for a board state with the lowest score (black player's
   * turn)
   * @return the best score this move (and all consequential response/exchanges up to the allowed
   *         look-ahead depth or time limit for searching).
   */
  int minmax(Board const& origBoard, int alpha, int beta, int depth, bool maximize) {
    BestMove mmBest(maximize);
    int lookAheadValue = 0;

    for (Move const& move : origBoard.moves1) {
      ///////////////////////////////////////////////////////////////////
      // See if we are at the end of our allowed depth to search and if so,
      // continue search if we still have made capture moves that we want to
      // see the responses for.  That way we don't take a pawn with our queen
      // at the end of a ply, and not know that in response we lose our queen!
      //
      // This is known as quiescent searching.

      if (depth <= 0) {
        if ((move.getValue() == 0) || depth <= qMaxDepth) {
          movesProcessed += mmBest.movesExamined;
          return Evaluator::evaluate(origBoard);
        }
      }

      Board currentBoard(origBoard);
      currentBoard.executeMove(move);
      currentBoard.advanceTurn();
      mmBest.movesExamined++;

      // See if the move we just made leaves the other player with no moves
      // and if so, return it as the best value we'll ever see on this search:
      if (currentBoard.moves1.empty()) {
        mmBest.move = move;
        mmBest.value = maximize ? MAX_VALUE - (100 - depth) : MIN_VALUE + (100 - depth);
        break;
      }

      // The recursive minimax step
      // While we have the depth keep looking ahead to see what this move accomplishes
      lookAheadValue = minmax(currentBoard, alpha, beta, depth - 1, !maximize);

      // See if this move is better than any we've seen for this board:
      //
      if ((!maximize && lookAheadValue < mmBest.value)
          || (maximize && lookAheadValue > mmBest.value)) {
        mmBest.value = lookAheadValue;
        mmBest.move = move;
      }

      // The alpha-beta pruning step
      // Continually tighten the low and high range of what we see this accomplishes
      // in the future and stop now if all future results are worse than the best move
      // we already have.
      if (maximize) {
        alpha = (lookAheadValue > alpha) ? lookAheadValue : alpha;
      } else {
        beta = (lookAheadValue < beta) ? lookAheadValue : beta;
      }
      if (alpha >= beta) {
        break;
      }
    }

    movesProcessed += mmBest.movesExamined;

    return mmBest.value;
  }
};
