/**
 * minimax.cpp
 *
 * minmax algorithm implemented in C++17 for chess board evaluation
 */

/**
 * MinMax Algorithm Template (yet to be written)
 *
 *  Can be applied to game problems if the following are true:
 *
 *      1 - carries a state which is static between player turns
 *      2 - is turn based
 *      3 - has a < operator for evaluation
 *      4 - transitions between states use concepts called 'moves'
 *      5 - one or more states define a win (or loss)
 *
 *      Game problem domains in this group include:
 *          * tic-tac-toe
 *          * card games
 *          * chess, checkers &c.  Basically any board game that is turn-based
 */

#include <evaluator.h>
#include <minimax.h>
#include <movecache.h>

#include <deque>
#include <future>
#include <mutex>

namespace chess {
    using std::async;
    using std::deque;
    using std::future;
    using std::launch;
    using std::thread;
    using std::chrono::duration;
    using std::chrono::steady_clock;
    using std::this_thread::yield;

    mutex examinedMutex;

    // free-standing function to atomically update the number of moves evaluated
    void updateNumMoves(Minimax &agent, int delta) {
        std::lock_guard<std::mutex> guard(examinedMutex);
        agent.movesExamined += delta;
    }

    /**
     * free-standing function to see if move search thread has reached the time limit
     *
     * If time limits are in effect we distribute the allowed time non-linearly
     * across the depths so that searches at the furthest depths time out sooner
     * which allows more time for earlier depths to consider more results
     *
     */
    bool hasTimedOut(Minimax const &agent, int currentDepth) {
        if (agent.timeout == 0) return false;

        // always complete the first set of moves
        if (currentDepth == agent.maxDepth) return false;

        duration<double> timeSpent = steady_clock::now() - agent.startTime;
        return timeSpent.count() >= agent.timeout;
    }

    Minimax::Minimax(int max_depth) : useThreads(false), useCache(false), best(true) {
        acceptableRiskLevel = 0.25;
        maxDepth = max_depth;
        extraChecks = false;
        movesExamined = 0L;
        qMaxDepth = -2;
        timeout = 0;
        reserve = 0;
    }

    Move Minimax::bestMove(Board const &board) {
        bool const maximize = (board.turn == White);
        best = BestMove(maximize);
        movesExamined = 0;

        // return immediately if there are 1 or 0 moves
        if (board.moves1.size() <= 1) {
            if (!board.moves1.empty()) {
                best = BestMove(board.moves1[0], board.moves1[0].getValue());
                updateNumMoves(*this, 1);
            }
            return best.move;
        }

        startTime = steady_clock::now();

        // See if we have a cached move if we aren't in an end game situation
        if (useCache && board.moves1.size() > 5) {
            auto entry = cache.lookup(board, board.turn);
            if (entry.isValid(board)) {
                movesExamined += entry.movesExamined;
                return entry.move;
            }
        }

        PieceMap pieceMap;

        if (extraChecks) {
            for (unsigned int ndx = 0; ndx < BOARD_SIZE; ndx++) {
                if (board.isEmpty(ndx)) continue;
                Piece piece = board.board[ndx];
                pieceMap[getSide(piece)][getType(piece)].push_back(ndx);
            }
        }

        auto move = useThreads ? searchWithThreads(board, maximize, pieceMap)
                               : searchWithNoThreads(board, maximize, pieceMap);

        if (useCache && move.isValid(board)) {
            cache.offer(board, move, board.turn, move.getValue(), movesExamined);
        }

        return move;
    }

    ThreadResult threadFunc(ThreadArgs *pArgs) {
        Move move = pArgs->move;
        bool maximize = pArgs->maximize;
        int depth = pArgs->depth;
        Minimax &agent = pArgs->agent;
        Board board = pArgs->board;
        delete pArgs;

        board.executeMove(move);
        board.advanceTurn();
        updateNumMoves(agent, 1);

        int value = agent.minmax(board, MIN_VALUE, MAX_VALUE, depth, maximize);
        return ThreadResult(value, move);
    }

    Move Minimax::searchWithThreads(Board const &board, bool maximize, PieceMap & /* pieceMap */) {
        best = BestMove(maximize);

        vector<ThreadResult> threadResults;
        deque<future<ThreadResult>> futures;

        // This functor will wait on the first thread, process it,
        // and remove it from the front of the deque.
        auto waitForNextResult = [&board, &maximize, this, &futures]() {
            if (!futures.empty()) {
                auto const result = futures.front().get();
                futures.pop_front();
                if (result.isValid(board)) {
                    if ((maximize && result.value > best.value)
                        || (!maximize && result.value < best.value)) {
                        best = BestMove(result.move, result.value);
                    }
                }
            }
        };

        unsigned int core_count = thread::hardware_concurrency();

        /// Number of cpu cores to leave out of the pool.
        /// Note that if it is equal to or greater than
        /// the number of available cores then the program
        /// is effectively single threaded.
        if (reserve > 0 && core_count >= reserve) {
            core_count -= reserve;
        }

        for (Move const &m : board.moves1) {
            if (reserve > 0) {
                // brain-dead implementation to limit total cpu load
                while (futures.size() > core_count) {
                    yield();
                    waitForNextResult();
                }
            }
            futures.emplace_back(future<ThreadResult>(async(
                launch::async, threadFunc, new ThreadArgs(board, m, *this, maxDepth, !maximize))));
        }

        while (!futures.empty()) {
            yield();
            waitForNextResult();
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
    Move Minimax::searchWithNoThreads(Board const &board, bool maximize,
                                      PieceMap & /* pieceMap */) {
        // We are not using threads.
        // Walk through all moves and find the best and return it in this calling thread.
        best = BestMove(maximize);

        for (Move move : board.moves1) {
            if (hasTimedOut(*this, maxDepth)) return best.move;

            Board currentBoard(board);
            currentBoard.executeMove(move);
            currentBoard.advanceTurn();
            movesExamined++;

            int lookAheadVal = minmax(currentBoard, MIN_VALUE, MAX_VALUE, maxDepth, !maximize);

            if ((maximize && lookAheadVal > best.value)
                || (!maximize && lookAheadVal < best.value)) {
                best.value = lookAheadVal;
                best.move = move;
                best.move.setValue(best.value);
            }
        }

        return best.move;
    }

    //    void unused_int(int /* unused */) {}
    //    void unused_bool(bool /* unused */) {}

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
    int Minimax::minmax(Board &origBoard, int alpha, int beta, int const depth,
                        bool const maximize) {
        BestMove mmBest(maximize);
        int value = mmBest.value;
        bool gotCacheHit;
        int cachedValue = value;
        Entry check;

        for (auto &move : origBoard.moves1) {
            yield();

            /// BUGBUG: fix quiescent search checking.  It *should* be able to examine
            /// the last move *this* side made (which we don't have available presently)
            /// to see if it was a capture move and to keep searching if so *unless* we've
            /// reached the additional max quiescent ply depth allowed
            if (depth <= 0) {
                if ((move.getCaptured() == Empty) || depth <= qMaxDepth) {
                    updateNumMoves(*this, mmBest.movesExamined);
                    return Evaluator::evaluate(origBoard);
                }
            }

            if (hasTimedOut(*this, depth)) {
                // see if we have changed the best move from it's default (worst value for our side)
                if ((maximize && mmBest.value == MIN_VALUE)
                    || (!maximize && mmBest.value == MAX_VALUE)) {
                    // since our 'worst' move will be favored by our recursive caller due to the
                    // fact that their 'maximize' is our !maximize, we will return 0 as a neutral
                    // value so that other positive or negative results from our other peer threads
                    // can override it if their results are better
                    return 0;
                }
                return mmBest.value;
            }

            ///////////////////////////////////////////////////////////////////
            // Before we try to find our own best move for this board state, see
            // if one is already cached:

            gotCacheHit = false;
            check = Entry();

            // We force moves to be manually evaluated via minmax when we get down to the end game.
            if (useCache && origBoard.moves1.size() > 5) {
                check = cache.lookup(origBoard, origBoard.turn);

                if (check.isValid()) {
                    gotCacheHit = true;
                    value = check.getValue();
                    cachedValue = value;
                    mmBest.move = check.move;
                    mmBest.value = value;
                    mmBest.move.setValue(value);
                    mmBest.movesExamined += check.movesExamined;

                    if (check.getRisk() > acceptableRiskLevel) {
                        // The risk is too high so we will do this manually and increase the count
                        // of how many times we have re-evaluated this move for this board
                        cache.increaseMoveUsedCount(origBoard, origBoard.turn);
                        check = Entry();
                    }
                }
            }

            if (!check.isValid()) {
                // We did not get a cached move so evaluate this one fresh

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
                value = minmax(currentBoard, alpha, beta, depth - 1, !maximize);

                // See if this move is better than any we've seen for this board:
                //
                if ((!maximize && value < mmBest.value) || (maximize && value > mmBest.value)) {
                    mmBest.value = value;
                    mmBest.move = move;
                    mmBest.move.setValue(value);

                    if (useCache) {
                        cache.offer(origBoard, move, origBoard.turn, value, mmBest.movesExamined);
                    }
                }

                // See if we had a cache hit but evaluated it anyway, and whether this improved the
                // existing move
                if (gotCacheHit) {
                    if ((maximize && value > cachedValue) || (!maximize && value < cachedValue)) {
                        cache.increaseMoveImprovedCount(origBoard, origBoard.turn);
                    }
                }
            }

            // The alpha-beta pruning step
            // Continually tighten the low and high range of what we see this accomplishes
            // in the future and stop now if all future results are worse than the best move
            // we already have.
            if (maximize) {
                alpha = (value > alpha) ? value : alpha;
            } else {
                beta = (value < beta) ? value : beta;
            }
            if (alpha >= beta) {
                break;
            }
        }

        updateNumMoves(*this, mmBest.movesExamined);

        return mmBest.value;
    }

    ThreadArgs::ThreadArgs(Board const &b, Move const &m, Minimax &mm, int d, bool max)
        : board(b), move(m), agent(mm), depth(d), maximize(max) {}

    ThreadResult::ThreadResult() { value = 0; }

    ThreadResult::ThreadResult(int const i, Move const &m) : value(i), move(m) {}

}  // namespace chess
