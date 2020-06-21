#include <doctest/doctest.h>

#if defined(_WIN32) || defined(WIN32)
// apparently this is required to compile in MSVC++
#    include <sstream>
#endif

#include <board.h>
#include <minimax.h>

namespace chess {
    /**
     * display the current board state
     *
     */
    void showBoard(Board &game) {
        using std::cout;
        using std::endl;
        using std::string;

        cout << endl;
        cout << "Turn: " << game.turns << " ";
        cout << game.lastMove().to_string() << endl;
        vector<string> lines = Board::to_string(game);
        for (auto &line : lines) {
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
        unsigned int fromCol = 0, fromRow = 0;
        unsigned int toCol = 0, toRow = 0;

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
            game.history.emplace_back(Move(4, 1, 4, 3, 0));  // set last move to black move of pawn
            game.turn = 1;                                   // set game to white's turn
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
            game.history.emplace_back(Move(3, 6, 3, 3, 0));  // set last move to white move of pawn
            game.turn = 0;                                   // set game to black's turn
        }

        MoveList pawnMoves;
        game.getPawnMoves(pawnMoves, fromCol, fromRow);  // get all legal moves for the pawn

        size_t numPawnMoves = pawnMoves.size();

        // ensure that 2 moves were found: single-push and en-passant capture
        // (double-push is invalid since pawn has moved)
        CHECK(numPawnMoves == 2);
        CHECK(numPawnMoves > 0);
        if (numPawnMoves > 0) {
            CHECK(numPawnMoves > 1);
            if (numPawnMoves > 1) {
                Move &move = pawnMoves[1];
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
                    size_t correctTaken
                        = ((side == White) ? game.taken1.size() : game.taken2.size()) + 1;
                    CHECK(numTaken + 1 == correctTaken);
                    CHECK(move.isCapture());
                    CHECK(chess::getType(move.getCaptured()) == Pawn);
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

        MoveList kingMoves;
        kingMoves.clear();
        game.getKingMoves(kingMoves, kingCol, kingRow);  // king with no moves at start
        size_t numKingMoves = kingMoves.size();
        CHECK(numKingMoves == 0);

        // remove pieces between left rook and king
        game.board[1 + kingRow * 8] = Empty;
        game.board[2 + kingRow * 8] = Empty;
        game.board[3 + kingRow * 8] = Empty;
        kingMoves.clear();
        game.getKingMoves(kingMoves, 4, kingRow);  // king with castling on left possible
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
        CHECK(castleCheck);
        castleCheck = false;

        // remove pieces between right rook and king
        game.board[5 + kingRow * 8] = Empty;
        game.board[6 + kingRow * 8] = Empty;
        kingMoves.clear();
        game.getKingMoves(kingMoves, 4, kingRow);  // king with castling on left and right possible
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
        CHECK(castleCheck);
    }

    /**
     * unit tests for game board operations
     *
     */
    TEST_CASE("chess::Board") {
        // test default constructor
        Board game;
        CHECK(game.board[0] == Rook);
        CHECK(!isEmpty(game.board[0]));
        CHECK(isEmpty(game.board[17]));
        CHECK(getValue(game.board[0]) == pieceValues[Rook]);
        CHECK(getValue(game.board[1]) == pieceValues[Knight]);
        CHECK(getValue(game.board[2]) == pieceValues[Bishop]);
        CHECK(getValue(game.board[3]) == pieceValues[Queen]);
        CHECK(getValue(game.board[4]) == pieceValues[King]);
        CHECK(getValue(game.board[8]) == pieceValues[Pawn]);
        CHECK(getValue(game.board[63]) == pieceValues[Rook]);
        CHECK(getSide(game.board[0]) == Black);
        CHECK(getSide(game.board[63]) == White);
        CHECK(!hasMoved(game.board[0]));
        CHECK(!inCheck(game.board[0]));

        // test copy constructor
        {
            Board game2(game);
            CHECK(!isEmpty(game2.board[0]));
            CHECK(isEmpty(game2.board[17]));
        }

        // test chessutil fascade
        CHECK(game.getType(0) == Rook);
        CHECK(game.getType(17) == Empty);
        CHECK(!game.isEmpty(0));
        CHECK(game.isEmpty(17));
        CHECK(game.getValue(0) == pieceValues[Rook]);
        CHECK(game.getValue(1) == pieceValues[Knight]);
        CHECK(game.getValue(2) == pieceValues[Bishop]);
        CHECK(game.getValue(3) == pieceValues[Queen]);
        CHECK(game.getValue(4) == pieceValues[King]);
        CHECK(game.getValue(8) == pieceValues[Pawn]);
        CHECK(game.getValue(63) == pieceValues[Rook]);
        CHECK(game.getSide(0) == Black);
        CHECK(game.getSide(63) == White);
        CHECK(!game.hasMoved(0));
        CHECK(!game.inCheck(0));

        game.setType(0, Pawn);
        CHECK(game.getType(0) == Pawn);
        CHECK(game.getSide(0) == Black);
        game.setSide(0, White);
        CHECK(game.getSide(0) == White);
        game.setSide(0, Black);
        game.setType(0, Rook);
        CHECK(game.getType(0) == Rook);
        game.setCheck(0, true);
        CHECK(game.inCheck(0));
        game.setMoved(0, true);
        CHECK(game.hasMoved(0));
        game.setCheck(0, true);
        CHECK(game.inCheck(0));

        vector<string> lines = Board::to_string(game);
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
        CHECK(game.isValidSpot(0, 0));
        CHECK(game.isValidSpot(0, 7));
        CHECK(game.isValidSpot(7, 0));
        CHECK(game.isValidSpot(7, 7));
        CHECK(!game.isValidSpot(0, 8));
        CHECK(!game.isValidSpot(8, 0));
        CHECK(!game.isValidSpot(8, 8));

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
        moves.clear();
        game.getPawnMoves(moves, 0, 1);
        CHECK(moves.size() == 2);  // black
        moves.clear();
        game.getPawnMoves(moves, 1, 1);
        CHECK(moves.size() == 2);
        moves.clear();
        game.getPawnMoves(moves, 2, 1);
        CHECK(moves.size() == 2);
        moves.clear();
        game.getPawnMoves(moves, 3, 1);
        CHECK(moves.size() == 2);
        moves.clear();
        game.getPawnMoves(moves, 4, 1);
        CHECK(moves.size() == 2);
        moves.clear();
        game.getPawnMoves(moves, 5, 1);
        CHECK(moves.size() == 2);
        moves.clear();
        game.getPawnMoves(moves, 6, 1);
        CHECK(moves.size() == 2);
        moves.clear();
        game.getPawnMoves(moves, 7, 1);
        CHECK(moves.size() == 2);
        moves.clear();
        game.getPawnMoves(moves, 0, 6);
        CHECK(moves.size() == 2);  // white
        moves.clear();
        game.getPawnMoves(moves, 1, 6);
        CHECK(moves.size() == 2);
        moves.clear();
        game.getPawnMoves(moves, 2, 6);
        CHECK(moves.size() == 2);
        moves.clear();
        game.getPawnMoves(moves, 3, 6);
        CHECK(moves.size() == 2);
        moves.clear();
        game.getPawnMoves(moves, 4, 6);
        CHECK(moves.size() == 2);
        moves.clear();
        game.getPawnMoves(moves, 5, 6);
        CHECK(moves.size() == 2);
        moves.clear();
        game.getPawnMoves(moves, 6, 6);
        CHECK(moves.size() == 2);
        moves.clear();
        game.getPawnMoves(moves, 7, 6);
        CHECK(moves.size() == 2);

        // rook moves
        moves.clear();
        game.getRookMoves(moves, 0, 0);
        CHECK(moves.size() == 0);  // black
        moves.clear();
        game.getRookMoves(moves, 7, 0);
        CHECK(moves.size() == 0);
        moves.clear();
        game.getRookMoves(moves, 0, 7);
        CHECK(moves.size() == 0);  // white
        moves.clear();
        game.getRookMoves(moves, 7, 7);
        CHECK(moves.size() == 0);

        // knight moves
        moves.clear();
        game.getKnightMoves(moves, 1, 0);
        CHECK(moves.size() == 2);  // black
        moves.clear();
        game.getKnightMoves(moves, 6, 0);
        CHECK(moves.size() == 2);
        moves.clear();
        game.getKnightMoves(moves, 1, 7);
        CHECK(moves.size() == 2);  // white
        moves.clear();
        game.getKnightMoves(moves, 6, 7);
        CHECK(moves.size() == 2);

        // bishop moves
        moves.clear();
        game.getBishopMoves(moves, 2, 0);
        CHECK(moves.size() == 0);  // black
        moves.clear();
        game.getBishopMoves(moves, 5, 0);
        CHECK(moves.size() == 0);
        moves.clear();
        game.getBishopMoves(moves, 2, 7);
        CHECK(moves.size() == 0);  // white
        moves.clear();
        game.getBishopMoves(moves, 5, 7);
        CHECK(moves.size() == 0);

        // queen moves
        moves.clear();
        game.getQueenMoves(moves, 3, 0);
        CHECK(moves.size() == 0);  // black
        moves.clear();
        game.getQueenMoves(moves, 3, 7);
        CHECK(moves.size() == 0);  // white

        // king moves
        moves.clear();
        game.getKingMoves(moves, 4, 0);
        CHECK(moves.size() == 0);  // black
        moves.clear();
        game.getKingMoves(moves, 4, 7);
        CHECK(moves.size() == 0);  // white

        // all moves
        CHECK(game.moves1.size() == 20);
        CHECK(game.moves2.size() == 20);

        // make the first 20 moves (10 successive turns for each side)
        unsigned int expectedTurns = 20;
        bool drawByRepetition = false;
        game.generateMoveLists();
        for (int i = 0; i < 20; i++) {
            if (!game.moves1.empty()) {
                game.executeMove(game.moves1[0]);
                game.advanceTurn();
                drawByRepetition = game.checkDrawByRepetition(game.lastMove());
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
        for (auto &ref : game.board) ref = Empty;

        game.board[4 + 1 * 8] = makeSpot(Pawn, White);
        Move move = Move{4, 1, 4, 0, 0};
        game.executeMove(move);
        CHECK(game.getType(4 + 0 * 8) == Queen);
        CHECK(game.isPromoted(4 + 0 * 8));

        game.board[4 + 6 * 8] = makeSpot(Pawn, Black);
        game.generateMoveLists();
        move = Move{4, 6, 4, 7, 0};
        game.executeMove(move);
        CHECK(game.getType(4 + 7 * 8) == Queen);
        CHECK(game.isPromoted(4 + 7 * 8));

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

        // check that we left the opponent with no moves
        CHECK(game.moves2.empty());
        game.turn = White;
        move = agent.bestMove(game);

        // check that we searched fewer moves
        CHECK(agent.movesExamined < movesExamined);

        // Test max repetitions allowed. Manually set up two pieces and
        // move them back and forth enough times that it should be triggered:
        game.maxRep = 3;
        game.turns = 0;
        game.turn = White;
        game.history.clear();
        game.board.fill(Empty);
        game.board[3 + 0 * 8] = makeSpot(Queen, Black);
        game.board[3 + 7 * 8] = makeSpot(Queen, White);
        Move move1 = Move(3, 7, 3, 6, 0);
        Move move2 = Move(3, 0, 3, 1, 0);
        Move move3 = Move(3, 6, 3, 7, 0);
        Move move4 = Move(3, 1, 3, 0, 0);
        // repeat this sequence 3 times and make sure it is not flagged on the first two times,
        // and make sure it gets flagged after the 3rd one.  To keep the code cleaner we'll
        // use a local lambda functor to define the repetitive pattern:
        auto take2Turns = [&]() -> bool {
            bool result = game.checkDrawByRepetition(move1);
            game.executeMove(move1);
            game.advanceTurn();

            result &= game.checkDrawByRepetition(move2);
            game.executeMove(move2);
            game.advanceTurn();

            result &= game.checkDrawByRepetition(move3);
            game.executeMove(move3);
            game.advanceTurn();

            result &= game.checkDrawByRepetition(move4);
            game.executeMove(move4);
            game.advanceTurn();

            return result;
        };

        CHECK(!take2Turns());
        CHECK(!take2Turns());
        CHECK(!take2Turns());

        // Test that if we tried the first move once more it would be caught:
        CHECK(game.checkDrawByRepetition(move1));
    }
}  // namespace chess
