#include <doctest/doctest.h>

#if defined(_WIN32) || defined(WIN32)
// apparently this is required to compile in MSVC++
#  include <sstream>
#endif

#include <board.h>
#include <chessutil.h>
#include <minimax.h>
#include <move.h>

#include <algorithm>
using std::find;
using std::get;
using std::tuple;

#include <string>

using namespace chess;

/**
 * display the current board state
 *
 */
void showBoard(Board& game) {
  using std::cout;
  using std::endl;
  using std::string;

  cout << endl;
  cout << "Turn: " << game.turns << " ";
  cout << game.lastMove.to_string() << endl;
  vector<string> lines = game.to_string(game);
  for (auto line : lines) {
    cout << line << endl;
  }
  cout << endl;
}

/**
 * unit tests for en passant moves
 *
 */
// ensure en passant moves are generated and tested
void checkEnPassant(unsigned int side) {
  Board game;

  // column and row to test en passant move from. changes if white or black
  int fromCol = 0, fromRow = 0;
  int toCol = 0, toRow = 0;

  if (side == White) {
    fromCol = 3;
    fromRow = 3;
    toCol = 4;
    toRow = 2;
    game.board[4 + 3 * 8] = game.board[4 + 1 * 8];  // move black pawn
    game.board[4 + 1 * 8] = Empty;
    game.board[3 + 3 * 8] = game.board[3 + 6 * 8];  // move white pawn
    game.board[3 + 6 * 8] = Empty;
    game.setMoved(4 + 3 * 8, true);  // mark pawns as moved
    game.setMoved(3 + 3 * 8, true);
    game.lastMove = Move(4, 1, 4, 3, 0);  // set last move to black move of pawn
    game.turn = 1;                        // set game to white's turn
  } else {
    fromCol = 4;
    fromRow = 3;
    toCol = 3;
    toRow = 4;
    game.board[3 + 3 * 8] = game.board[3 + 6 * 8];  // move white pawn
    game.board[3 + 6 * 8] = Empty;
    game.board[4 + 3 * 8] = game.board[4 + 1 * 8];  // move black pawn
    game.board[4 + 1 * 8] = Empty;
    game.setMoved(4 + 3 * 8, true);  // mark pawns as moved
    game.setMoved(3 + 3 * 8, true);
    game.lastMove = Move(3, 6, 3, 3, 0);  // set last move to white move of pawn
    game.turn = 0;                        // set game to black's turn
  }

  MoveList pawnMoves = game.getPawnMoves(fromCol, fromRow);  // get all legal moves for the pawn

  size_t numPawnMoves = pawnMoves.size();

  // ensure that 2 moves were found: single-push and en-passant capture
  // (double-push is invalid since pawn has moved)
  CHECK(numPawnMoves == 2);
  CHECK(numPawnMoves > 0);
  if (numPawnMoves > 0) {
    CHECK(numPawnMoves > 1);
    if (numPawnMoves > 1) {
      Move& move = pawnMoves[1];
      if (move.getFromCol() == move.getToCol()) {
        // this is a single-push
      }
      CHECK(toCol == move.getToCol());
      CHECK(toRow == move.getToRow());
      if (toCol == move.getToCol()) {
        // this should be an en-passant capture on the right
        CHECK(move.getValue() != 0);  // the move should should have some value

        auto numTaken = ((side == White) ? game.taken1.size() : game.taken2.size()) + 1;
        game.executeMove(move);
        size_t correctTaken = ((side == White) ? game.taken1.size() : game.taken2.size()) + 1;
        CHECK(numTaken + 1 == correctTaken);
      }
    }
  }
}

/**
 * unit tests for castling moves
 *
 */
// ensure that castling is tested
void checkCastle(unsigned int side) {
  Board game;

  int kingCol = 0;
  int kingRow = 0;

  if (side == White) {
    kingCol = 4;
    kingRow = 7;
  } else {
    kingCol = 4;
    kingRow = 0;
  }

  MoveList kingMoves = game.getKingMoves(kingCol, kingRow);  // king with no moves at start
  size_t numKingMoves = kingMoves.size();
  CHECK(numKingMoves == 0);

  // remove pieces between left rook and king
  game.board[1 + kingRow * 8] = Empty;
  game.board[2 + kingRow * 8] = Empty;
  game.board[3 + kingRow * 8] = Empty;
  kingMoves = game.getKingMoves(4, kingRow);  // king with castling on left possible
  numKingMoves = kingMoves.size();
  CHECK(numKingMoves == 2);  // should be 2 moves: castle and move on left

  // ensure that we execute and test castling on the left
  bool castleCheck = false;
  if (numKingMoves == 2) {
    Move move = kingMoves[1];
    if (move.getToCol() == 2) {
      // this move is the castle on the left
      Board current = Board(game);  // make temp copy of current board state
      game.executeMove(move);
      castleCheck = true;
      game = current;  // restore board state
    }
  }
  CHECK(castleCheck == true);
  castleCheck = false;

  // remove pieces between right rook and king
  game.board[5 + kingRow * 8] = Empty;
  game.board[6 + kingRow * 8] = Empty;
  kingMoves = game.getKingMoves(4, kingRow);  // king with castling on left and right possible
  numKingMoves = kingMoves.size();
  CHECK(numKingMoves == 4);  // should be 4 moves: castle and move on left and right

  if (numKingMoves == 4) {
    Move move = kingMoves[2];
    if (move.getToCol() == 6) {
      // this move is the castle on the right
      game.executeMove(move);
      castleCheck = true;
    }
  }
  CHECK(castleCheck == true);
}

/**
 * unit tests for game board operations
 *
 */
TEST_CASE("chess::Board") {
  // test default constructor
  Board game;

  CHECK(game.board[0] == Rook);

  CHECK(isEmpty(game.board[0]) == false);
  CHECK(isEmpty(game.board[17]) == true);
  CHECK(getValue(game.board[0]) == values[Rook]);
  CHECK(getValue(game.board[1]) == values[Knight]);
  CHECK(getValue(game.board[2]) == values[Bishop]);
  CHECK(getValue(game.board[3]) == values[Queen]);
  CHECK(getValue(game.board[4]) == values[King]);
  CHECK(getValue(game.board[8]) == values[Pawn]);
  CHECK(getValue(game.board[63]) == values[Rook]);
  CHECK(getSide(game.board[0]) == Black);
  CHECK(getSide(game.board[63]) == White);
  CHECK(hasMoved(game.board[0]) == false);
  CHECK(inCheck(game.board[0]) == false);

  // test copy constructor
  {
    Board game2(game);
    CHECK(isEmpty(game2.board[0]) == false);
    CHECK(isEmpty(game2.board[17]) == true);
  }

  // test chessutil fascade
  CHECK(game.getType(0) == Rook);
  CHECK(game.getType(17) == Empty);
  CHECK(game.isEmpty(0) == false);
  CHECK(game.isEmpty(17) == true);
  CHECK(game.getValue(0) == values[Rook]);
  CHECK(game.getValue(1) == values[Knight]);
  CHECK(game.getValue(2) == values[Bishop]);
  CHECK(game.getValue(3) == values[Queen]);
  CHECK(game.getValue(4) == values[King]);
  CHECK(game.getValue(8) == values[Pawn]);
  CHECK(game.getValue(63) == values[Rook]);
  CHECK(game.getSide(0) == Black);
  CHECK(game.getSide(63) == White);
  CHECK(game.hasMoved(0) == false);
  CHECK(game.inCheck(0) == false);

  game.setType(0, Pawn);
  CHECK(game.getType(0) == Pawn);
  CHECK(game.getSide(0) == Black);
  game.setSide(0, White);
  CHECK(game.getSide(0) == White);
  game.setSide(0, Black);
  game.setType(0, Rook);
  CHECK(game.getType(0) == Rook);
  game.setCheck(0, true);
  CHECK(game.inCheck(0) == true);
  game.setMoved(0, true);
  CHECK(game.hasMoved(0) == true);
  game.setCheck(0, true);
  CHECK(game.inCheck(0) == true);

  vector<string> lines = game.to_string(game);
  vector<string> correct{"\"8  r  n  b  q  k  b  n  r \"", "\"7  p  p  p  p  p  p  p  p \"",
                         "\"6  .     .     .     .    \"", "\"5     .     .     .     . \"",
                         "\"4  .     .     .     .    \"", "\"3     .     .     .     . \"",
                         "\"2  P  P  P  P  P  P  P  P \"", "\"1  R  N  B  Q  K  B  N  R \"",
                         "\"   a  b  c  d  e  f  g  h\""};

  CHECK(lines.size() == correct.size());

  for (size_t i = 0; i < lines.size(); i++) {
    string const answer = '"' + lines[i] + '"';
    CHECK(answer == correct[i]);
  }

  // reset board to new game
  game = Board();

  // test piece moves on default constructor (new game)

  // test move primitives
  CHECK(game.isValidSpot(0, 0) == true);
  CHECK(game.isValidSpot(0, 7) == true);
  CHECK(game.isValidSpot(7, 0) == true);
  CHECK(game.isValidSpot(7, 7) == true);
  CHECK(game.isValidSpot(0, 8) == false);
  CHECK(game.isValidSpot(8, 0) == false);
  CHECK(game.isValidSpot(8, 8) == false);
  CHECK(game.isValidSpot(0, -1) == false);
  CHECK(game.isValidSpot(-1, 0) == false);
  CHECK(game.isValidSpot(-1, -1) == false);
  CHECK(game.isValidSpot(8, -1) == false);
  CHECK(game.isValidSpot(-1, 8) == false);

  MoveList moves{};

  // test valid first moves of leftmost black pawn
  game.addMoveIfValid(moves, 0, 1, 0, 2);  // single push
  CHECK(moves.size() == 1);
  game.addMoveIfValid(moves, 0, 1, 0, 3);  // double push
  CHECK(moves.size() == 2);

  // test remaining black pawns
  game.addMoveIfValid(moves, 1, 1, 1, 2);  // single push
  CHECK(moves.size() == 3);
  game.addMoveIfValid(moves, 1, 1, 1, 3);  // double push
  CHECK(moves.size() == 4);
  game.addMoveIfValid(moves, 2, 1, 2, 2);  // single push
  CHECK(moves.size() == 5);
  game.addMoveIfValid(moves, 2, 1, 2, 3);  // double push
  CHECK(moves.size() == 6);
  game.addMoveIfValid(moves, 3, 1, 3, 2);  // single push
  CHECK(moves.size() == 7);
  game.addMoveIfValid(moves, 3, 1, 3, 3);  // double push
  CHECK(moves.size() == 8);
  game.addMoveIfValid(moves, 4, 1, 4, 2);  // single push
  CHECK(moves.size() == 9);
  game.addMoveIfValid(moves, 4, 1, 4, 3);  // double push
  CHECK(moves.size() == 10);
  game.addMoveIfValid(moves, 5, 1, 5, 2);  // single push
  CHECK(moves.size() == 11);
  game.addMoveIfValid(moves, 5, 1, 5, 3);  // double push
  CHECK(moves.size() == 12);
  game.addMoveIfValid(moves, 6, 1, 6, 2);  // single push
  CHECK(moves.size() == 13);
  game.addMoveIfValid(moves, 6, 1, 6, 3);  // double push
  CHECK(moves.size() == 14);
  game.addMoveIfValid(moves, 7, 1, 7, 2);  // single push
  CHECK(moves.size() == 15);
  game.addMoveIfValid(moves, 7, 1, 7, 3);  // double push
  CHECK(moves.size() == 16);

  // test valid moves of black knights  // "it's only a scratch!"
  game.addMoveIfValid(moves, 1, 0, 0, 2);
  CHECK(moves.size() == 17);
  game.addMoveIfValid(moves, 1, 0, 2, 2);
  CHECK(moves.size() == 18);
  game.addMoveIfValid(moves, 6, 0, 5, 2);
  CHECK(moves.size() == 19);
  game.addMoveIfValid(moves, 6, 0, 7, 2);
  CHECK(moves.size() == 20);

  // pawn moves
  CHECK(game.getPawnMoves(0, 1).size() == 2);  // black
  CHECK(game.getPawnMoves(1, 1).size() == 2);
  CHECK(game.getPawnMoves(2, 1).size() == 2);
  CHECK(game.getPawnMoves(3, 1).size() == 2);
  CHECK(game.getPawnMoves(4, 1).size() == 2);
  CHECK(game.getPawnMoves(5, 1).size() == 2);
  CHECK(game.getPawnMoves(6, 1).size() == 2);
  CHECK(game.getPawnMoves(7, 1).size() == 2);
  CHECK(game.getPawnMoves(0, 6).size() == 2);  // white
  CHECK(game.getPawnMoves(1, 6).size() == 2);
  CHECK(game.getPawnMoves(2, 6).size() == 2);
  CHECK(game.getPawnMoves(3, 6).size() == 2);
  CHECK(game.getPawnMoves(4, 6).size() == 2);
  CHECK(game.getPawnMoves(5, 6).size() == 2);
  CHECK(game.getPawnMoves(6, 6).size() == 2);
  CHECK(game.getPawnMoves(7, 6).size() == 2);

  // rook moves
  CHECK(game.getRookMoves(0, 0).size() == 0);  // black
  CHECK(game.getRookMoves(7, 0).size() == 0);
  CHECK(game.getRookMoves(0, 7).size() == 0);  // white
  CHECK(game.getRookMoves(7, 7).size() == 0);

  // knight moves
  CHECK(game.getKnightMoves(1, 0).size() == 2);  // black
  CHECK(game.getKnightMoves(6, 0).size() == 2);
  CHECK(game.getKnightMoves(1, 7).size() == 2);  // white
  CHECK(game.getKnightMoves(6, 7).size() == 2);

  // bishop moves
  CHECK(game.getBishopMoves(2, 0).size() == 0);  // black
  CHECK(game.getBishopMoves(5, 0).size() == 0);
  CHECK(game.getBishopMoves(2, 7).size() == 0);  // white
  CHECK(game.getBishopMoves(5, 7).size() == 0);

  // queen moves
  CHECK(game.getQueenMoves(3, 0).size() == 0);  // black
  CHECK(game.getQueenMoves(3, 7).size() == 0);  // white

  // king moves
  CHECK(game.getKingMoves(4, 0).size() == 0);  // black
  CHECK(game.getKingMoves(4, 7).size() == 0);  // white

  // all moves
  CHECK(game.moves1.size() == 20);
  CHECK(game.moves2.size() == 20);

  // make the first 20 moves (10 successive turns for each side)
  int expectedTurns = 20;
  bool drawByRepetition = false;
  game.generateMoveLists();
  for (int i = 0; i < 20; i++) {
    if (game.moves1.size() > 0) {
      game.executeMove(game.moves1[0]);
      game.advanceTurn();
      drawByRepetition = game.checkDrawByRepetition(game.lastMove, 3);
      if (drawByRepetition) {
        expectedTurns = game.turns;
        break;
      }
    }
  }
  // CHECK(drawByRepetition == true);

  // test game history was remembered
  CHECK(game.history.size() == expectedTurns);

  // ensure en passant moves are generated and tested
  checkEnPassant(White);
  checkEnPassant(Black);

  // ensure that castling is tested
  checkCastle(White);
  checkCastle(Black);

  using std::cout, std::endl;

  game = Board();

  // attempt to add an illegal en passant capture to a move list. This should be stopped by
  // addIfValid(...)
  MoveList moveList{};

  game.addMoveIfValid(moveList, 4, 1, 3, 2);  // black pawn
  // check that the add did not succeed
  CHECK(moveList.size() == 0);

  game.addMoveIfValid(moveList, 3, 6, 4, 5);  // white pawn
  // check that the add did not succeed
  CHECK(moveList.size() == 0);

  // check that pawns are promoted to queens on back row
  game = Board();
  for (auto& ref : game.board) ref = Empty;

  game.board[4 + 1 * 8] = makeSpot(Pawn, White);
  Move move = Move{4, 1, 4, 0, 0};
  game.executeMove(move);
  CHECK(game.getType(4 + 0 * 8) == Queen);

  game.board[4 + 6 * 8] = makeSpot(Pawn, Black);
  game.generateMoveLists();
  move = Move{4, 6, 4, 7, 0};
  game.executeMove(move);
  CHECK(game.getType(4 + 7 * 8) == Queen);

  // test that move returned immediately if it leaves opponent with no moves
  // White has queen at 4,0 (promoted from pawn)
  // Black has queen at 4,7 (promoted from pawn)
  game.generateMoveLists();
  Minimax agent(1);
  game.turn = White;
  move = agent.bestMove(game);
  long movesExamined = agent.movesExamined;
  CHECK(movesExamined > 1);

  // remove the only piece the Black side has leaving it with no moves
  game.board[4 + 7 * 8] = Empty;
  game.turn = White;
  game.generateMoveLists();
  CHECK(game.moves2.empty() == true);
  game.turn = White;
  move = agent.bestMove(game);
  CHECK(agent.movesExamined < movesExamined);
}
