#include <bestmove.h>
#include <board.h>
#include <chessutil.h>
#include <minimax.h>
#include <move.h>

#include <algorithm>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

using namespace chess;

static void playGame(Board& board, Minimax& agent);
static Move getNextPlayersMove(Board& board, Minimax& agent);
static void showBoard(Board& board);

int main(int argc, char** argv) {
  int maxDepth = 3;

  Board board;
  Minimax agent(maxDepth);

  playGame(board, agent);

  return 0;
}

static void showBoard(Board& board) {
  if (board.lastMove.isValid()) {
    cout << "Last: " << board.lastMove.to_string() << endl;
  }
  auto lines = board.to_string(board);
  for_each(begin(lines), end(lines), [](auto const& line) { cout << line << endl; });
}

static void playGame(Board& board, Minimax& agent) {
  static int const maxRepetitions = 3;

  cout << endl;
  showBoard(board);

  Move move = getNextPlayersMove(board, agent);

  while (move.isValid()) {
    board.executeMove(move);
    board.advanceTurn();

    cout << endl;
    showBoard(board);

    move = getNextPlayersMove(board, agent);
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

static Move getNextPlayersMove(Board& board, Minimax& agent) {
  Move move = agent.bestMove(board);

  return move;
}
