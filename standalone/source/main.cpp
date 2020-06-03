/**
 * main entry point for standalone C++17 chess implementation
 *
 * written 2020 trent m wyatt
 *
 */

#include <minimax.h>
#include <options.h>

#include <algorithm>
#include <csignal>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

using namespace chess;

static void playGame(Board&, Minimax&);
static void showBoard(Board&, Minimax const&);
static void sig_handler(int);

static Minimax agent;
static Board game;

#include <stdio.h>

#include <algorithm>

int main(int argc, char** argv) {
  signal(SIGINT, sig_handler);

  Options options(--argc, ++argv);

  agent = Minimax(options.getInt("ply", 1));
  agent.useCache = options.getBool("cache", false);
  agent.useThreads = options.getBool("threads", true);
  agent.timeout = options.getInt("timeout", 10);
  game.maxRep = options.getInt("maxrep", 3);

  cout << "use threads     : " << agent.useThreads << endl;
  cout << "use cache       : " << agent.useCache << endl;
  cout << "max ply depth   : " << agent.maxDepth << endl;
  cout << "timeout         : " << agent.timeout << endl;
  cout << "max repetitions : " << game.maxRep << endl;

  playGame(game, agent);

  sig_handler(0);
}

void sig_handler(int /* sig */) {
  cout << "\r   \r" << endl << endl << "Finished!" << endl << endl;
  chess::MoveCache::showMetrics();
  cout << endl;
  exit(0);
}

static void showBoard(Board& board, Minimax const& minimax) {
  static char const types[7] = {' ', 'p', 'n', 'b', 'r', 'q', 'k'};

  cout << endl;

  Move const& lastMove = board.lastMove;
  if (lastMove.isValid()) {
    Piece lastPieceMoved = board.board[lastMove.getTo()];
    string playerString = getColor(lastPieceMoved);
    string piece = getName(lastPieceMoved);
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
    if (chess::isPromoted(lastPieceMoved)) {
      cout << " and was promoted to Queen! ";
      setPromoted(lastMove.getTo(), false);  // remove promoted flag so we won't mention it again
    }
    cout << endl;
  }
  auto lines = chess::Board::to_string(board);
  int line_num = 4;
  lines[1] += "      Taken1 : ";
  lines[2] += "      Taken2 : ";
  lines[3] += "      Score  : " + std::to_string(lastMove.getValue());
  lines[line_num++] += "    Examined : " + std::to_string(minimax.movesExamined);
  if (board.kingIsInCheck(White)) {
    lines[line_num++] += "    White is in Check!";
  }
  if (board.kingIsInCheck(Black)) {
    lines[line_num++] += "    Black is in Check!";
  }

  for (auto piece : board.taken1) {
    lines[1] += types[getType(piece)];
  }
  for (auto piece : board.taken2) {
    lines[2] += toupper(types[getType(piece)]);
  }

  for_each(begin(lines), end(lines), [](auto const& line) { cout << line << endl; });
}

static void playGame(Board& board, Minimax& minimax) {
  showBoard(board, minimax);

  Move move = minimax.bestMove(board);

  while (move.isValid(board)) {
    if (board.checkDrawByRepetition(move)) break;
    board.executeMove(move);
    board.advanceTurn();

    showBoard(board, minimax);

    move = minimax.bestMove(board);
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
