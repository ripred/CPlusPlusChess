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

static void playGame(Board &, Minimax &);
static void showGameEndSummary();
static void showBoard(Board &, Minimax const &);
static void sig_handler(int);

Minimax &getAgent() {
  static Minimax agent;
  return agent;
}

Board &getBoard() {
  static Board board;
  return board;
}

int main(int argc, char **argv) {
  signal(SIGINT, sig_handler);

  // temp:
  // force ref to chess:pieceValues until I figure out why this
  // is being reported as optimized out.
  //  int unused = pieceValues[Pawn];
  //  int unused2 = unused;
  //  unused = unused2;

  Options options(--argc, ++argv);

  Minimax &agent = getAgent();
  Board &board = getBoard();

  agent = Minimax(options.getInt("ply", 1));
  agent.useCache = options.getBool("cache", false);
  agent.useThreads = options.getBool("threads", true);
  agent.reserve = options.getInt("reserve", 0);
  agent.qMaxDepth = 0 - options.getInt("qply", 2);
  agent.timeout = options.getInt("timeout", 10);
  board.maxRep = options.getInt("maxrep", 3);

  cout << "use threads       : " << agent.useThreads << endl;
  cout << "use cache         : " << agent.useCache << endl;
  cout << "max ply depth     : " << agent.maxDepth << endl;
  cout << "max quiescent ply : " << agent.qMaxDepth << endl;
  cout << "reserve           : " << agent.reserve << endl;
  cout << "timeout           : " << agent.timeout << endl;
  cout << "max repetitions   : " << board.maxRep << endl;

  playGame(board, agent);

  cout << "\r   \r" << endl << endl << "Finished!" << endl << endl;
  showGameEndSummary();

  return 0;
}

static void showGameEndSummary() {
  if (getAgent().useCache) {
    chess::MoveCache::showMetrics();
  }
  cout << endl;
}

static void sig_handler(int /* sig */) {
  cout << "\r   \r" << endl << endl << "stopped by user.." << endl << endl;
  showGameEndSummary();
  exit(0);
}

static void showBoard(Board &board, Minimax const &minimax) {
  static char const types[7] = {' ', 'p', 'n', 'b', 'r', 'q', 'k'};

  cout << endl;

  Move const &lastMove = board.lastMove;
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
  string winningSide = (score < 0) ? getColor(Black) : (score > 0) ? getColor(White) : "even";
  score = abs(score);
  string scoreStr = addCommas(score);
  string numExaminedStr = addCommas(minimax.movesExamined);

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

static void playGame(Board &board, Minimax &agent) {
  showBoard(board, agent);

  Move move = agent.bestMove(board);

  while (move.isValid(board)) {
    if (board.checkDrawByRepetition(move)) break;
    board.executeMove(move);
    board.advanceTurn();

    showBoard(board, agent);

    move = agent.bestMove(board);
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
