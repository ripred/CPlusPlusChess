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

static void playGame(Board& board, Minimax& agent);
static void showBoard(Board& board);
static void sig_handler(int);

int main(int argc, char** argv) {
  signal(SIGINT, sig_handler);

  Options options(argc, argv);

  Minimax agent(options.getInt("ply", 1));

  agent.useCache = options.getBool("cache", false);
  agent.useThreads = options.getBool("threads", true);

  cout << "use threads: " << (agent.useThreads ? "yes" : "no") << endl;
  cout << "max depth = " << agent.maxDepth << endl;
  cout << "use cache: " << (agent.useCache ? "yes" : "no") << endl;

  Board board;
  playGame(board, agent);

  return 0;
}

void sig_handler(int sig) {
  cout << "\r   \r" << endl << endl << "Finished!" << endl << endl;
  exit(0);
}

static void showBoard(Board& board, Minimax const& agent) {
  static char const types[7] = {' ', 'p', 'n', 'b', 'r', 'q', 'k'};
  static string const names[7]
      = {"", "Pawn   ", "Knight  ", "Bishop ", "Rook   ", "Queen  ", "King   "};
  cout << endl;
  if (board.lastMove.isValid()) {
    string player = (board.getSide(board.lastMove.getTo()) == White) ? "White " : "Black ";
    string piece = names[board.getType(board.lastMove.getTo())];
    cout << "Turn: " << board.turns << " ";
    cout << board.lastMove.to_string(0b010) << " " << player << piece << " ";
    cout << endl;
  }
  auto lines = board.to_string(board);
  lines[1] += "      Taken1 : ";
  lines[2] += "      Taken2 : ";
  lines[3] += "      Score  : " + std::to_string(board.lastMove.getValue());
  int line = 4;
  lines[line++] += "    Examined : " + std::to_string(agent.movesExamined);
  if (board.kingInCheck(White)) {
    lines[line++] += "    White is in Check!";
  }
  if (board.kingInCheck(Black)) {
    lines[line++] += "    Black is in Check!";
  }

  for (auto piece : board.taken1) {
    lines[1] += types[getType(piece)];
  }
  for (auto piece : board.taken2) {
    lines[2] += toupper(types[getType(piece)]);
  }

  for_each(begin(lines), end(lines), [](auto const& line) { cout << line << endl; });
}

static void playGame(Board& board, Minimax& agent) {
  static int const maxRepetitions = 3;

  showBoard(board, agent);

  Move move = agent.bestMove(board);

  while (move.isValid(board)) {
    if (board.checkDrawByRepetition(move, maxRepetitions)) break;
    board.executeMove(move);
    board.advanceTurn();

    showBoard(board, agent);

    move = agent.bestMove(board);
  }

  if (board.checkDrawByRepetition(move, maxRepetitions)) {
    cout << "Draw by repetition!" << endl;
  } else if (board.moves1.size() == 0 && board.moves2.size() == 0) {
    cout << "Stalemate!" << endl;
  } else if (board.kingInCheck(White) || board.kingInCheck(Black)) {
    cout << "Checkmate!" << endl;
    cout << "Player " << std::to_string((board.turn % 2) + 1) << " wins!" << endl;
  }
}
