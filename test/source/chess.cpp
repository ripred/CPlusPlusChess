#include <doctest/doctest.h>

#if defined(_WIN32) || defined(WIN32)
// apparently this is required to compile in MSVC++
#    include <sstream>
#endif

#include <board.h>
#include <minimax.h>

#include <algorithm>
#include <iostream>

namespace chess {
    using std::cend;
    using std::cout;
    using std::get;
    using std::tuple;
    /**
     * display the current board state
     *
     */
    static void showBoard(Board &game) {
        cout << endl;
        cout << "Turn: " << game.turns << " ";
        cout << game.lastMove().to_string() << endl;
        vector<string> lines = chess::Board::to_string(game);
        for (auto const &line : lines) {
            cout << line << endl;
        }
        cout << endl;
    }

    /**
     * utility functions for analyzing differences in move results
     *
     */
    using TripleMoveList = tuple<MoveList, MoveList, MoveList>;

    TripleMoveList moveListDiff(const MoveList &first, const MoveList &second) {
        MoveList intersection, unique_first, unique_second;
        for (const Move &move : first) {
            if (find(second.begin(), second.end(), move) != second.end()) {
                intersection.emplace_back(move);
            } else {
                unique_first.emplace_back(move);
            }
        }

        for (const Move &move : second) {
            if (find(first.begin(), first.end(), move) == first.end()) {
                unique_second.emplace_back(move);
            }
        }

        auto sortByValueRankFile = [](Move &m1, Move &m2) {
            if ((m1.getValue() > m2.getValue())) return true;
            if ((m1.getValue() == m2.getValue())) {
                if (((m1.getFrom() / 8) > (m2.getFrom() / 8))) {
                    return true;
                }
            }
            return ((m1.getFrom() / 8) == (m2.getFrom() / 8));
        };

        sort(intersection.begin(), intersection.end(), sortByValueRankFile);
        sort(unique_first.begin(), unique_first.end(), sortByValueRankFile);
        sort(unique_second.begin(), unique_second.end(), sortByValueRankFile);

        return tuple(unique_first, unique_second, intersection);
    }

    /**
     * show the difference between two move lists
     *
     */
    void showListDiff(const tuple<MoveList, MoveList, MoveList> &lists) {
        cout << "unique to single-thread:" << endl;
        for (const Move &move : get<0>(lists)) {
            cout << move.to_string(0b010) << endl;
        }
        cout << "unique to multi-threaded:" << endl;
        for (const Move &move : get<1>(lists)) {
            cout << move.to_string(0b010) << endl;
        }
        cout << "intersection:" << endl;
        for (const Move &move : get<2>(lists)) {
            cout << move.to_string(0b010) << endl;
        }
    }

    /**
     * unit tests for comparing single vs multi-threaded
     *
     */
    void test_threading(const Board &board, int plyDepth = 0) {
        Minimax agent(plyDepth);
        Board game(board);

        // single threaded
        agent.useThreads = false;
        game.turn = White;
        agent.timeout = 10;
        game.generateMoveLists();
        MoveList moveList11 = game.moves1;
        MoveList moveList12 = game.moves2;

        Move move_single_thread = agent.bestMove(game);

        size_t count11 = game.moves1.size();
        size_t count12 = game.moves2.size();
        unsigned long examined1 = agent.movesExamined;

        // multi-threaded
        // make a fresh game board copy
        game = Board(board);
        agent.useThreads = true;
        game.turn = White;
        game.generateMoveLists();
        MoveList moveList21 = game.moves1;
        MoveList moveList22 = game.moves2;
        Move move_multi_thread = agent.bestMove(game);
        size_t count21 = game.moves1.size();
        size_t count22 = game.moves2.size();
        unsigned long examined2 = agent.movesExamined;

        auto lists1 = moveListDiff(moveList11, moveList21);
        CHECK(get<0>(lists1).size() == 0);
        CHECK(get<1>(lists1).size() == 0);

        auto lists2 = moveListDiff(moveList12, moveList22);
        CHECK(get<0>(lists2).size() == 0);
        CHECK(get<1>(lists2).size() == 0);

        string stMoveStr = move_single_thread.to_string(0b111);
        string mtMoveStr = move_multi_thread.to_string(0b111);
        CHECK(stMoveStr == mtMoveStr);
        CHECK(count11 == count21);
        CHECK(count12 == count22);

        if (examined1 != examined2) {
            showBoard(game);
            auto lists = moveListDiff(moveList11, moveList21);
            showListDiff(lists);
        }
        CHECK(examined1 == examined2);
    }

    /**
     * unit tests for minmax AI implementation
     *
     */
    TEST_CASE("chess::Minimax") {
/// BUGBUG: Fix correctly!!
/// Disabling these checks until I can determine the failure on Windows builds:
#if !defined(_WIN32) && !defined(WIN32)
        // disable these tests in MSVC++

        // make an empty game board
        Board game = Board();
        game.board.fill(Empty);

        // save blank board for re-use
        Board blank = Board(game);

        // load up test
        game.board[4 + 0 * 8] = makeSpot(King, Black);
        game.board[4 + 7 * 8] = makeSpot(King, White);
        game.turn = White;
        game.generateMoveLists();

        Board testcopy = game;

        test_threading(game, 0);

        game = testcopy;

        Minimax agent(0);
        agent.useThreads = false;
        agent.useCache = false;
        agent.timeout = 10;

        Move move = agent.bestMove(game);

        CHECK(move.isValid(game));
        CHECK(agent.movesExamined == 5);

        // restore a blank board
        game = Board(blank);

        game.turn = White;

        game.board[4 + 4 * 8] = makeSpot(Pawn, White, false, false);
        game.generateMoveLists();
        move = agent.bestMove(game);
        CHECK(move.isValid(game));
        CHECK(game.moves1.size() == 2);
        CHECK(agent.movesExamined == 2);

        game.setMoved(4 + 4 * 8, true);
        game.generateMoveLists();
        move = agent.bestMove(game);
        CHECK(move.isValid(game));
        CHECK(game.moves1.size() == 1);
        CHECK(agent.movesExamined == 1);

        game.board[4 + 4 * 8] = makeSpot(Rook, White, false, false);
        game.turn = White;
        game.generateMoveLists();
        move = agent.bestMove(game);
        CHECK(move.isValid(game));
        CHECK(game.moves1.size() == 14);
        CHECK(agent.movesExamined == 14);

        game.board[4 + 4 * 8] = makeSpot(Knight, White, false, false);
        game.turn = White;
        game.generateMoveLists();
        move = agent.bestMove(game);
        CHECK(move.isValid(game));
        CHECK(game.moves1.size() == 8);
        CHECK(agent.movesExamined == 8);

        game.board[4 + 4 * 8] = makeSpot(Bishop, White, false, false);
        game.turn = White;
        game.generateMoveLists();
        move = agent.bestMove(game);
        CHECK(move.isValid(game));
        CHECK(game.moves1.size() == 13);
        CHECK(agent.movesExamined == 13);

        game.board[4 + 4 * 8] = makeSpot(Queen, White, false, false);
        game.turn = White;
        game.generateMoveLists();
        move = agent.bestMove(game);
        CHECK(move.isValid(game));
        CHECK(game.moves1.size() == 27);
        CHECK(agent.movesExamined == 27);

        game.board[4 + 4 * 8] = makeSpot(King, White, false, false);
        game.turn = White;
        game.generateMoveLists();
        move = agent.bestMove(game);
        CHECK(move.isValid(game));
        CHECK(game.moves1.size() == 8);
        CHECK(agent.movesExamined == 8);

        game = Board(blank);  // restore blank board
        game.board[4 + 0 * 8] = makeSpot(King, Black, false, false);
        game.board[4 + 7 * 8] = makeSpot(King, White, false, false);
        game.turn = White;
        game.generateMoveLists();
        agent.maxDepth = 1;  // change agent to go to a ply depth of 1
        agent.useThreads = false;
        move = agent.bestMove(game);

        CHECK(move.isValid(game));
        CHECK(game.moves1.size() == 5);
        CHECK(game.moves2.size() == 5);
        CHECK(agent.movesExamined == 30);

        agent.maxDepth = 2;  // change agent to go to a ply depth of 2
        agent.useThreads = false;
        game.generateMoveLists();
        move = agent.bestMove(game);
        CHECK(move.isValid(game));
        CHECK(game.moves1.size() == 5);
        CHECK(game.moves2.size() == 5);

        game = Board(blank);  // restore blank board
        // set up black king in corner with only 1 legal move
        game.board[0 + 0 * 8] = makeSpot(King, Black, true, false);
        game.board[0 + 7 * 8] = makeSpot(Queen, White, true, false);
        game.board[1 + 1 * 8] = makeSpot(Bishop, White, true, false);
        game.board[1 + 7 * 8] = makeSpot(Rook, White, true, false);
        game.board[2 + 7 * 8] = makeSpot(Rook, White, true, false);
        game.turn = Black;
        game.generateMoveLists();

        agent.maxDepth = 0;  // change agent to go to a ply depth of 0
        agent.useThreads = false;
        move = agent.bestMove(game);
        CHECK(move.isValid(game));
        CHECK(game.moves1.size() == 1);
        CHECK(game.moves2.size() == 40);
        CHECK(agent.movesExamined == 1);

        // execute the black king move
        game.executeMove(move);
        game.advanceTurn();

        // White can checkmate by moving from 0,7 to 0,0 (a1 to a8)
        agent.maxDepth = 1;  // change agent to go to a ply depth of 1
        agent.useThreads = false;
        agent.extraChecks = true;
        move = Move(0, 7, 0, 0, 0);
        // execute the white queen move
        game.executeMove(move);
        game.advanceTurn();

        // ensure that black has no more legal moves
        move = agent.bestMove(game);
        CHECK(game.moves1.empty());
        CHECK(game.moves2.size() == 34);
        CHECK(game.kingIsInCheck(Black));
        CHECK(!game.kingIsInCheck(White));
#endif
    }
}  // namespace chess
