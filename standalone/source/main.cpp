#include <minimax.h>
#include <signal.h>

#include <algorithm>
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
  int maxDepth = 1;

  signal(SIGINT, sig_handler);

  Board board;
  Minimax agent(maxDepth);

  if (argc > 1) {
    agent.useCache = false;
    cout << endl << "not using cache" << endl;
  }

  playGame(board, agent);

  return 0;
}

void sig_handler(int sig) {
  cout << "\r   \r" << endl << endl << "Finished!" << endl << endl;
  exit(0);
}

static void showBoard(Board& board, Minimax const& agent) {
  static string const names[]
      = {"", "Pawn   ", "Rook   ", "Knight  ", "Bishop ", "Queen  ", "King   "};
  cout << endl;
  if (board.lastMove.isValid()) {
    string player = (board.getSide(board.lastMove.getTo()) == White) ? "White " : "Black ";
    string piece = names[board.getType(board.lastMove.getTo())];
    cout << "Turn: " << (board.turns + 1) << " ";
    cout << board.lastMove.to_string(0b110) << " " << player << piece << " ";
    cout << agent.movesExamined << " examined" << endl;
  }
  auto lines = board.to_string(board);
  for_each(begin(lines), end(lines), [](auto const& line) { cout << line << endl; });
}

static void playGame(Board& board, Minimax& agent) {
  static int const maxRepetitions = 3;

  showBoard(board, agent);

  Move move = agent.bestMove(board);

  while (move.isValid()) {
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
