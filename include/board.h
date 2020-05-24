//
// Board.h
// properties and methods for managing a chess board
//

#pragma once

#include <chessutil.h>
#include <move.h>

#include <cmath>
#include <vector>

namespace chess {
  using std::vector;

  class Board {
  private:
  public:
    Piece board[BOARD_SIZE]{};
    MoveList moves1;
    MoveList moves2;
    MoveList history;
    PieceList taken1;
    PieceList taken2;
    Move lastMove{0, 0, 0, 0, 0};

    int maxRep{};
    unsigned int turns{};
    unsigned int turn{};
    int blkKingLoc{};
    int whtKingLoc{};

  public:
    Board();
    Board(Board const& ref) = default;
    Board& operator=(Board const& ref) = default;

    [[nodiscard]] bool isEmpty(int ndx) const;
    [[nodiscard]] Piece getType(int ndx) const;
    [[nodiscard]] Piece getSide(int ndx) const;
    [[nodiscard]] bool hasMoved(int ndx) const;
    [[nodiscard]] int getValue(int ndx) const;
    [[nodiscard]] bool inCheck(int ndx) const;
    [[nodiscard]] bool isPromoted(int ndx) const;
    void setType(int ndx, Piece type);
    void setSide(int ndx, Piece side);
    void setMoved(int ndx, bool hasMoved);
    void setCheck(int ndx, bool inCheck);
    void setPromoted(int ndx, bool promoted);
    static vector<string> to_string(Board const& b);

    void generateMoveLists();

    [[nodiscard]] bool checkDrawByRepetition(Move const& move, int maxRepetitions) const;

    [[nodiscard]] bool kingIsInCheck(Piece side) const;

    void executeMove(Move& move);

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

    MoveList getMovesSorted(Piece side);

    MoveList getMoves(Piece side, bool checkKing) const;

    MoveList cleanupMoves(MoveList& moves, Piece side) const;

    static bool isValidSpot(int col, int row);

    void addMoveIfValid(MoveList& moves, int fromCol, int fromRow, int toCol, int toRow) const;

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
