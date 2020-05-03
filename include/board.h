//
// Board.h
// properties and methods for managing a chess board
//

#pragma once

#include <chessutil.h>
#include <move.h>

#include <cmath>
#include <vector>

using std::vector;

namespace chess {
  class Board {
  private:
  public:
    unsigned int board[BOARD_SIZE]{};
    vector<Move> moves1{};
    vector<Move> moves2{};
    vector<Move> history{};
    vector<unsigned int> taken1{};
    vector<unsigned int> taken2{};
    vector<unsigned int> pieces1{};
    vector<unsigned int> pieces2{};
    Move lastMove{0, 0, 0, 0, 0};

    int maxRep{};
    unsigned int turns{};
    unsigned int turn{};
    int blkKingLoc{};
    int whtKingLoc{};

  public:
    Board();
    Board(Board const& ref) = default;

    bool isEmpty(int const ndx) const;
    unsigned int getType(int ndx) const;
    unsigned int getSide(int ndx) const;
    bool hasMoved(int ndx) const;
    int getValue(int ndx) const;
    bool inCheck(int ndx) const;
    void setType(int ndx, unsigned int type);
    void setSide(int ndx, unsigned int side);
    void setMoved(int ndx, bool hasMoved);
    void setCheck(int ndx, bool inCheck);
    static vector<string> to_string(Board const& b);

    void generateMoveLists();

    bool checkDrawByRepetition(Move const& move, int const maxRepetitions) const;

    bool kingInCheck(unsigned int const side);

    void executeMove(Move const& move);

    /**
     * Advance the total number of moves in the game.
     * Also toggle which players turn it is, and generates
     * the following changes in the board:
     *
     *      + .turn  contains the color of the new player to move next
     *      + .turns contains the total number of moves made so far
     *
     *      + move list for current     moving player are in moves1     (turn's Side color: Black or
     * White)
     *      + move list for current non-moving player are in moves2     ( ((turn + 1) % 2)'s Side
     * color: Black or White)
     *      + piece list for current     moving player are in pieces1
     *      + piece list for current non-moving player are in pieces2
     *
     */
    void advanceTurn();

    vector<Move> getMovesSorted(unsigned int const side);

    vector<Move> getMoves(unsigned int const side, bool checkKing);

    vector<Move> cleanupMoves(vector<Move>& moves, unsigned int const side) const;

    static bool isValidSpot(int const col, int const row);

    void addMoveIfValid(vector<Move>& moves, int fromCol, int fromRow, int toCol, int toRow) const;

    /**
     * Get a list of all possible moves for a pawn at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new vector<Move> containing all possible moves a pawn could make from the given
     * spot
     * @throws IllegalArgumentException if the specified location is invalid or if it does not
     * contain a piece to move.
     */
    vector<Move> getPawnMoves(int col, int row) const;

    bool addSlider(vector<Move> moves, int col, int row, int x, int y) const;

    /**
     * Get a list of all possible moves for a rook at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new vector<Move> containing all possible moves a rook could make from the given
     * spot
     * @throws IllegalArgumentException if the specified location is invalid or if it does not
     * contain a piece to move.
     */
    vector<Move> getRookMoves(int col, int row) const;

    /**
     * Get a list of all possible moves for a knight at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new vector<Move> containing all possible moves a knight could make from the given
     * spot
     * @throws IllegalArgumentException if the specified location is invalid or if it does not
     * contain a piece to move.
     */
    vector<Move> getKnightMoves(int col, int row) const;

    /**
     * Get a list of all possible moves for a bishop at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new vector<Move> containing all possible moves a bishop could make from the given
     * spot
     * @throws IllegalArgumentException if the specified location is invalid or if it does not
     * contain a piece to move.
     */
    vector<Move> getBishopMoves(int col, int row) const;

    /**
     * Get a list of all possible moves for a queen at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new vector<Move> containing all possible moves a queen could make from the given
     * spot
     * @throws IllegalArgumentException if the specified location is invalid or if it does not
     * contain a piece to move.
     */
    vector<Move> getQueenMoves(int col, int row) const;

    /**
     * Get a list of all possible moves for a king at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new vector<Move> containing all possible moves a king could make from the given
     * spot
     * @throws IllegalArgumentException if the specified location is invalid or if it does not
     * contain a piece to move.
     */
    vector<Move> getKingMoves(int col, int row) const;
  };

}  // namespace chess
