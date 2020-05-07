#include <doctest/doctest.h>

#if defined(_WIN32) || defined(WIN32)
// apparently this is required to compile in MSVC++
#  include <sstream>
#endif

#include <bestmove.h>
#include <board.h>
#include <chessutil.h>
#include <evaluator.h>
#include <minimax.h>
#include <move.h>

#include <iostream>
#include <string>

using namespace chess;

void showBoard(Board& game) {
  using std::cout, std::endl;
  cout << endl;
  cout << "Turn: " << game.turns << " ";
  cout << game.lastMove.to_string() << endl;
  vector<std::string> lines = game.to_string(game);
  for (auto line : lines) {
    cout << line << endl;
  }
  cout << endl;
}

TEST_CASE("chess::chessutil") {
  int const whitePawn = makeSpot(Pawn, White, false, false);
  int const blackPawn = makeSpot(Pawn, Black, false, false);
  int const movedWhitePawn = setCheck(setMoved(whitePawn, true), true);
  int const movedBlackPawn = setCheck(setMoved(blackPawn, true), true);

  CHECK(getType(whitePawn) == Pawn);
  CHECK(getType(blackPawn) == Pawn);
  CHECK(getType(Rook) == Rook);
  CHECK(getType(Knight) == Knight);
  CHECK(getType(Bishop) == Bishop);
  CHECK(getType(Queen) == Queen);
  CHECK(getType(King) == King);

  CHECK(isEmpty(0) == true);
  CHECK(isEmpty(Empty) == true);
  CHECK(isEmpty(whitePawn) == false);
  CHECK(isEmpty(blackPawn) == false);
  CHECK(isEmpty(Rook) == false);
  CHECK(isEmpty(Knight) == false);
  CHECK(isEmpty(Bishop) == false);
  CHECK(isEmpty(Queen) == false);
  CHECK(isEmpty(King) == false);

  CHECK(getValue(whitePawn) == values[Pawn]);
  CHECK(getValue(blackPawn) == values[Pawn]);
  CHECK(getValue(Rook) == values[Rook]);
  CHECK(getValue(Knight) == values[Knight]);
  CHECK(getValue(Bishop) == values[Bishop]);
  CHECK(getValue(Queen) == values[Queen]);
  CHECK(getValue(King) == values[King]);

  CHECK(getSide(whitePawn) == 1);
  CHECK(getSide(blackPawn) == 0);

  CHECK(hasMoved(whitePawn) == false);
  CHECK(hasMoved(blackPawn) == false);
  CHECK(hasMoved(movedWhitePawn) == true);
  CHECK(hasMoved(movedBlackPawn) == true);

  CHECK(inCheck(whitePawn) == false);
  CHECK(inCheck(blackPawn) == false);
  CHECK(inCheck(movedWhitePawn) == true);
  CHECK(inCheck(movedBlackPawn) == true);

  CHECK(setType(Empty, Pawn) == getType(whitePawn));
  CHECK(setType(Empty, Pawn) == getType(blackPawn));

  CHECK(getSide(setSide(setType(Empty, Pawn), White)) == White);
  CHECK(getSide(setSide(setType(Empty, Pawn), Black)) == Black);

  CHECK(setMoved(setSide(setType(Empty, Pawn), White), true) == (Moved | Pawn | Side));
  CHECK(setMoved(setSide(setType(Empty, Pawn), Black), true) == (Moved | Pawn));
}

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

  vector<Move> pawnMoves = game.getPawnMoves(fromCol, fromRow);  // get all legal moves for the pawn

  size_t numPawnMoves = pawnMoves.size();

  // ensure that 2 moves were found: single-push and en-passant capture
  // (double-push is invalid since pawn has moved)
  CHECK(numPawnMoves == 2);
  CHECK(numPawnMoves > 0);
  if (numPawnMoves > 0) {
    Move move{0, 0, 0, 0, 0};
    CHECK(numPawnMoves > 1);
    if (numPawnMoves > 1) {
      move = pawnMoves[1];
      if (move.getFromCol() == move.getToCol()) {
        // this is a single-push
      }
      CHECK(toCol == move.getToCol());
      CHECK(toRow == move.getToRow());
      if (toCol == move.getToCol()) {
        // this should be an en-passant capture on the right
        CHECK(move.getValue()
              != 0);  // the move should capture a piece and so should have some value

        auto numTaken = game.taken2.size();
        game.executeMove(move);
        size_t correctTaken = ((side == White) ? game.taken1.size() : game.taken2.size()) + 1;
        CHECK(numTaken + 1 == correctTaken);
      }
    }
  }
}

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

  vector<Move> kingMoves = game.getKingMoves(kingCol, kingRow);  // king with no moves at start
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
  Board game2(game);
  CHECK(isEmpty(game2.board[0]) == false);
  CHECK(isEmpty(game2.board[17]) == true);

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

  CHECK(game.pieces1.size() == 16);
  CHECK(game.pieces2.size() == 16);

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

  vector<Move> moves{};

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

  // make the first 100 moves (50 successive turns for each side)
  int expectedTurns = 100;
  bool drawByRepetition = false;
  for (int i = 0; i < 100; i++) {
    if (game.moves1.size() > 0) {
      game.advanceTurn();
      game.executeMove(game.moves1[0]);
      drawByRepetition = game.checkDrawByRepetition(game.lastMove, 3);
      if (drawByRepetition) {
        expectedTurns = game.turns;
        break;
      }
    }
  }
  CHECK(drawByRepetition == true);

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
  vector<Move> moveList{};

  game.addMoveIfValid(moveList, 4, 1, 3, 2);  // black pawn
  // check that the add did not succeed
  CHECK(moveList.size() == 0);

  game.addMoveIfValid(moveList, 3, 6, 4, 5);  // white pawn
  // check that the add did not succeed
  CHECK(moveList.size() == 0);
}

TEST_CASE("chess::Move") {
  Move move1(1, 2, 3, 4, 1000);

  CHECK(move1.getFromCol() == 1);
  CHECK(move1.getFromRow() == 2);
  CHECK(move1.getToCol() == 3);
  CHECK(move1.getToRow() == 4);

  CHECK(move1.getFrom() == 1 + 2 * 8);
  CHECK(move1.getTo() == 3 + 4 * 8);

  CHECK(move1.getValue() == 1000);

  move1.setValue(2000);
  CHECK(move1.getValue() == 2000);

  Move move2(move1);
  CHECK(move1.getFromCol() == move2.getFromCol());
  CHECK(move1.getFromRow() == move2.getFromRow());
  CHECK(move1.getToCol() == move2.getToCol());
  CHECK(move1.getToRow() == move2.getToRow());
  CHECK(move1.getFrom() == move2.getFrom());
  CHECK(move1.getTo() == move2.getTo());
  CHECK(move1.getValue() == move2.getValue());

  CHECK((move1 == move1) == true);
  CHECK((move1 == move2) == true);

  string correct = "[Move from 1,2 to 3,4 (b6 to d4) value:2000]";
  CHECK(move1.to_string() == correct);

  move1 = Move();
  CHECK(move1.isValid() == false);
  move1 = Move(4, 1, 4, 2, 0);
  CHECK(move1.isValid() == true);
}

TEST_CASE("chess::minimax") {
  Board game;
  Minimax agent(0);

  Move move = agent.bestMove(game);

  CHECK(move.isValid() == true);
  CHECK(agent.movesProcessed == 20);

  for (Bits& bits : game.board) {
    bits = Empty;
  }

  game.board[4 + 4 * 8] = makeSpot(Pawn, White, false, false);
  game.turn = White;
  game.generateMoveLists();
  move = agent.bestMove(game);
  CHECK(move.isValid() == true);
  CHECK(game.moves1.size() == 2);
  CHECK(agent.movesProcessed == 2);

  game.setMoved(4 + 4 * 8, true);
  game.generateMoveLists();
  move = agent.bestMove(game);
  CHECK(move.isValid() == true);
  CHECK(game.moves1.size() == 1);
  CHECK(agent.movesProcessed == 1);

  game.board[4 + 4 * 8] = makeSpot(Rook, White, false, false);
  game.turn = White;
  game.generateMoveLists();
  move = agent.bestMove(game);
  CHECK(move.isValid() == true);
  CHECK(game.moves1.size() == 14);
  CHECK(agent.movesProcessed == 14);

  game.board[4 + 4 * 8] = makeSpot(Knight, White, false, false);
  game.turn = White;
  game.generateMoveLists();
  move = agent.bestMove(game);
  CHECK(move.isValid() == true);
  CHECK(game.moves1.size() == 8);
  CHECK(agent.movesProcessed == 8);

  game.board[4 + 4 * 8] = makeSpot(Bishop, White, false, false);
  game.turn = White;
  game.generateMoveLists();
  move = agent.bestMove(game);
  CHECK(move.isValid() == true);
  CHECK(game.moves1.size() == 13);
  CHECK(agent.movesProcessed == 13);

  game.board[4 + 4 * 8] = makeSpot(Queen, White, false, false);
  game.turn = White;
  game.generateMoveLists();
  move = agent.bestMove(game);
  CHECK(move.isValid() == true);
  CHECK(game.moves1.size() == 27);
  CHECK(agent.movesProcessed == 27);

  game.board[4 + 4 * 8] = makeSpot(King, White, false, false);
  game.turn = White;
  game.generateMoveLists();
  move = agent.bestMove(game);
  CHECK(move.isValid() == true);
  CHECK(game.moves1.size() == 8);
  CHECK(agent.movesProcessed == 8);
}
