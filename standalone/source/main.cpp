/**
 * main entry point for standalone C++17 chess implementation
 *
 * written 2020 trent m wyatt
 *
 */

#include <board.h>
#include <evaluator.h>
#include <minimax.h>
#include <movecache.h>
#include <options.h>

#include <algorithm>
#include <csignal>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

using namespace chess;

static void playGame(Board &, Minimax &, Minimax &);

static void showGameEndSummary();

static void showBoard(Board &, int);

static void sig_handler(int);

Minimax &getAgent() {
    static Minimax agent(1);
    return agent;
}

Board &getBoard() {
    static Board board;
    return board;
}

int main(int argc, char **argv) {
    signal(SIGINT, sig_handler);

    Options options(--argc, ++argv);

    Minimax &agent1 = getAgent();
    Board &board = getBoard();

    agent1.maxDepth = options.getInt("ply", 1);
    agent1.useCache = options.getBool("cache", false);
    agent1.useThreads = options.getBool("threads", true);
    agent1.extraChecks = options.getBool("extra", false);
    agent1.acceptableRiskLevel = options.getFloat("risk", 0.25);
    agent1.reserve = options.getInt("reserve", 0);
    agent1.qMaxDepth = 0 - options.getInt("qmax", 2);
    agent1.timeout = options.getInt("timeout", 10);
    board.maxRep = options.getInt("maxrep", 3);

    cout << "use threads       :  " << agent1.useThreads << endl;
    cout << "use cache         :  " << agent1.useCache << endl;
    cout << "max ply depth     :  " << agent1.maxDepth << endl;
    cout << "timeout           :  " << agent1.timeout << endl;
    cout << "risk level        :  " << agent1.acceptableRiskLevel << endl;
    cout << "max repetitions   :  " << board.maxRep << endl;
    cout << "extra checks      :  " << agent1.extraChecks << endl;
    cout << "reserve           :  " << agent1.reserve << endl;
    cout << "max quiescent ply : " << agent1.qMaxDepth << endl;

    playGame(board, agent1, agent1);

    cout << "\r   \r" << endl << endl << "Finished!" << endl << endl;
    showGameEndSummary();

    return 0;
}

static void showGameEndSummary() {
    if (getAgent().useCache) {
        getAgent().cache.showMetrics();
    }
    cout << endl;
}

static void sig_handler(int /* sig */) {
    cout << "\r   \r" << endl << endl << "stopped by user.." << endl << endl;
    showGameEndSummary();
    exit(0);
}

static void showBoard(Board &board, int movesExamined = 0) {
    static char const types[7] = {' ', 'p', 'n', 'b', 'r', 'q', 'k'};

    cout << endl;

    Move const &lastMove = board.lastMove();
    if (lastMove.isValid()) {
        Piece lastPieceMoved = board.board[lastMove.getTo()];
        bool promotedPawn = isPromoted(lastPieceMoved);
        string playerString = getColor(lastPieceMoved);
        string piece = promotedPawn ? getName(Pawn) : getName(lastPieceMoved);
        cout << "Turn: " << board.turns << " ";
        cout << lastMove.to_string(0b010) << " " << playerString << " " << piece << " ";
        if (lastMove.isCapture()) {
            // The last move captured a piece'
            string opponentString = getColor(lastMove.getCaptured());
            string capturedPieceString = getName(lastMove.getCaptured());
            cout << "captured " << opponentString << " " << capturedPieceString;
        } else {
            cout << "moved ";
        }
        if (isPromoted(lastPieceMoved)) {
            cout << "and was promoted to Queen! ";
        }
        setPromoted(lastMove.getTo(), false);  // remove promoted flag so we won't mention it again
        cout << endl;
    }

    auto lines = Board::to_string(board);
    int appendToLineNumber = 4;
    int score = Evaluator::evaluate(board);
    string winningSide = (score < 0) ? getColor(setSide(0, Black))
                                     : (score > 0) ? getColor(setSide(0, White)) : "even";
    score = abs(score);
    string scoreStr = addCommas(score);
    string numExaminedStr = addCommas(movesExamined);

    lines[1] += "          Taken1 : ";
    lines[2] += "          Taken2 : ";
    lines[3] += "      Advantage  : " + winningSide + ((score == 0) ? "" : (" " + scoreStr));

    lines[appendToLineNumber++] += "        Examined : " + numExaminedStr;
    if (board.kingIsInCheck(White)) {
        lines[appendToLineNumber++] += "        White is in Check!";
    }
    if (board.kingIsInCheck(Black)) {
        lines[appendToLineNumber++] += "        Black is in Check!";
    }

    for (auto piece : board.taken1) {
        lines[1] += types[getType(piece)];
    }
    for (auto piece : board.taken2) {
        lines[2] += toupper(types[getType(piece)]);
    }

    for_each(begin(lines), end(lines), [](auto const &line) { cout << line << endl; });
}

static void playGame(Board &board, Minimax &agent1, Minimax &agent2) {
    showBoard(board);

    Move move = agent1.bestMove(board);

    while (move.isValid(board)) {
        if (board.checkDrawByRepetition(move)) break;
        board.executeMove(move);
        board.advanceTurn();
        showBoard(board, agent1.movesExamined);

        move = agent2.bestMove(board);
        if (move.isValid(board)) {
            if (board.checkDrawByRepetition(move)) break;
            board.executeMove(move);
            board.advanceTurn();
            showBoard(board, agent2.movesExamined);
        }

        move = agent1.bestMove(board);
    }

    if (board.checkDrawByRepetition(move)) {
        cout << "Draw by repetition!" << endl;
    } else if (board.moves1.empty() && board.moves2.empty()) {
        cout << "Stalemate!" << endl;
    } else if (board.kingIsInCheck(White) || board.kingIsInCheck(Black)) {
        cout << "Checkmate!" << endl;
        cout << "Player " << std::to_string((board.turn % 2) + 1) << " wins!" << endl;
    }
}
