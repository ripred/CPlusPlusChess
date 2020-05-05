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
    vector<Move> moves1;
    vector<Move> moves2;
    vector<Move> history;
    vector<unsigned int> taken1;
    vector<unsigned int> taken2;
    vector<unsigned int> pieces1;
    vector<unsigned int> pieces2;
    Move lastMove{0, 0, 0, 0, 0};

    int maxRep{};
    unsigned int turns{};
    unsigned int turn{};
    int blkKingLoc{};
    int whtKingLoc{};

  public:
    Board();
    Board(Board const& ref) = default;

    [[nodiscard]] bool isEmpty(int ndx) const;
    [[nodiscard]] unsigned int getType(int ndx) const;
    [[nodiscard]] unsigned int getSide(int ndx) const;
    [[nodiscard]] bool hasMoved(int ndx) const;
    [[nodiscard]] int getValue(int ndx) const;
    [[nodiscard]] bool inCheck(int ndx) const;
    void setType(int ndx, unsigned int type);
    void setSide(int ndx, unsigned int side);
    void setMoved(int ndx, bool hasMoved);
    void setCheck(int ndx, bool inCheck);
    static vector<string> to_string(Board const& b);

    void generateMoveLists();

    [[nodiscard]] bool checkDrawByRepetition(Move const& move, int maxRepetitions) const;

    bool kingInCheck(unsigned int side);

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

    vector<Move> getMovesSorted(unsigned int side);

    vector<Move> getMoves(unsigned int side, bool checkKing);

    vector<Move> cleanupMoves(vector<Move>& moves, unsigned int side) const;

    static bool isValidSpot(int col, int row);

    void addMoveIfValid(vector<Move>& moves, int fromCol, int fromRow, int toCol, int toRow) const;

    /**
     * Get a list of all possible moves for a pawn at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new vector<Move> containing all possible moves a pawn could make from the given
     * spot
     */
    [[nodiscard]] vector<Move> getPawnMoves(int col, int row) const;

    [[nodiscard]] bool addSlider(vector<Move>& moves, int col, int row, int x, int y) const;

    /**
     * Get a list of all possible moves for a rook at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new vector<Move> containing all possible moves a rook could make from the given
     * spot
     */
    [[nodiscard]] vector<Move> getRookMoves(int col, int row) const;

    /**
     * Get a list of all possible moves for a knight at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new vector<Move> containing all possible moves a knight could make from the given
     * spot
     */
    [[nodiscard]] vector<Move> getKnightMoves(int col, int row) const;

    /**
     * Get a list of all possible moves for a bishop at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new vector<Move> containing all possible moves a bishop could make from the given
     * spot
     */
    [[nodiscard]] vector<Move> getBishopMoves(int col, int row) const;

    /**
     * Get a list of all possible moves for a queen at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new vector<Move> containing all possible moves a queen could make from the given
     * spot
     */
    [[nodiscard]] vector<Move> getQueenMoves(int col, int row) const;

    /**
     * Get a list of all possible moves for a king at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new vector<Move> containing all possible moves a king could make from the given
     * spot
     */
    [[nodiscard]] vector<Move> getKingMoves(int col, int row) const;
  };

}  // namespace chess
