/**
 * minimax.cpp
 *
 * minmax algorithm implemented in c++ for chess board evaluation
 */

/**
 * MinMax Algorithm Template (yet to be written)
 *
 *  Can be applied to game problems if the following are true:
 *
 *      1 - carries a state which is static between player turns
 *      2 - is turn based
 *      3 - has a < operator (for eval)
 *      4 - transitions between states use concepts called 'moves'
 *      5 - one or more states define a win (or loss)
 *
 *      Game problem domains in this group include:
 *          * tic-tac-toe
 *          * card games
 *          * chess, checkers &c.  Basically any board game that is turn-based
 */

#include <minimax.h>
#include <movecache.h>

#include <chrono>
#include <memory>
#include <mutex>
#include <thread>

using namespace chess;

MoveCache Minimax::cache;

Minimax::Minimax(int max_depth) : best(true) {
  maxDepth = max_depth;
  qMaxDepth = 0;
  movesExamined = 0L;
}

Move Minimax::bestMove(Board const& board) {
  bool const maximize = (board.turn == White);
  best = BestMove(maximize);
  movesExamined = 0L;

  if (board.moves1.size() == 1) {
    best = BestMove(board.moves1[0], board.moves1[0].getValue());
    movesExamined = best.movesExamined = 1;
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

  if (useCache) {
    auto move = cache.lookup(board);
    if (move.isValid(board)) {
      return move;
    }
  }

  auto move = useThreads ? searchWithThreads(board, maximize, pieceMap)
                         : searchWithNoThreads(board, maximize, pieceMap);

  if (move.isValid(board)) {
    cache.offer(board, move);
  }

  return move;
}

struct ThreadArgs {
  Board const& board;
  Move const& move;
  Minimax& agent;
  int depth;
  bool maximize;
  size_t id;

  ThreadArgs() = delete;
  ThreadArgs(Board const& b, Move const& m, Minimax& mm, int d, bool max, int tid)
      : board(b), move(m), agent(mm), depth(d), maximize(max), id(tid) {}
};

struct ThreadResult {
  int value;
  Move move;
  ThreadResult() { value = 0; }
  ThreadResult(int const i, Move const& m) : value(i), move(m) {}
};

map<int, ThreadResult> threadResults;

using std::mutex;
using std::thread;

vector<thread> threads;
mutex lock1;

void lock() { lock1.lock(); }
void unlock() { lock1.unlock(); }

void* threadFunc(void* ptr) {
  ThreadArgs* pArgs = (ThreadArgs*)ptr;

  Move move = pArgs->move;
  int id = pArgs->id;
  bool maximize = pArgs->maximize;
  int depth = pArgs->depth;
  Minimax& agent = pArgs->agent;
  Board board = pArgs->board;
  delete pArgs;
  pArgs = nullptr;

  board.executeMove(move);
  board.advanceTurn();
  lock();
  agent.movesExamined++;
  unlock();

  int value = agent.minmax(board, MIN_VALUE, MAX_VALUE, depth, maximize);
  ThreadResult result(value, move);

  lock();
  threadResults[id] = result;
  unlock();

  return nullptr;
}

Move Minimax::searchWithThreads(Board const& board, bool maximize, PieceMap& pieceMap) {
  UNUSED(pieceMap);
  best = BestMove(maximize);

  threadResults.clear();
  threads.clear();

  size_t count = board.moves1.size();
  for (size_t i = 0; i < count; ++i) {
    Move const& move = board.moves1[i];
    ThreadArgs* pArgs = new ThreadArgs(board, move, *this, maxDepth, !maximize, i);

    threads.push_back(thread(threadFunc, pArgs));
  }

  for (size_t i = 0; i < count; ++i) {
    threads[i].join();
  }

  for (size_t i = 0; i < count; ++i) {
    ThreadResult const& result = threadResults[i];

    if (result.move.isValid(board)) {
      if ((maximize && result.value > best.value) || (!maximize && result.value < best.value)) {
        best = BestMove(result.move, result.value);
      }
    }
  }

  return best.move;
}

/**
 * Iterate over all available moves for the current player and decide which move is the best.
 * This executes on the current thread and is a blocking call.
 *
 * @param board the board state to examine each move on
 * @param pieceMap board pieces mapped by type and side
 * @return the best move for this board
 */
Move Minimax::searchWithNoThreads(Board const& board, bool maximize, PieceMap& pieceMap) {
  // We are not using threads.
  // Walk through all moves and find the best and return it in this calling thread.
  UNUSED(pieceMap);
  best = BestMove(maximize);

  for (Move const& move : board.moves1) {
    Board currentBoard(board);
    currentBoard.executeMove(move);
    currentBoard.advanceTurn();
    movesExamined++;

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
int Minimax::minmax(Board const& origBoard, int alpha, int beta, int depth, bool maximize) {
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
        lock();
        movesExamined += mmBest.movesExamined;
        unlock();
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

  lock();
  movesExamined += mmBest.movesExamined;
  unlock();

  return mmBest.value;
}
