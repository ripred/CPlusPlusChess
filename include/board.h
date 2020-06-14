//
// Board.h
// properties and methods for managing a chess board
//

#pragma once

#include <chessutil.h>
#include <move.h>

#include <array>
#include <cmath>
#include <vector>

namespace chess {
    using std::array;
    using std::vector;

    class Board {
    private:
    public:
        array<Piece, BOARD_SIZE> board{};
        MoveList moves1;
        MoveList moves2;
        PieceList taken1;
        PieceList taken2;
        MoveList history;

        int maxRep{3};
        unsigned int turns{};
        unsigned int turn{White};

    public:
        Board();
        Board(Board const& ref) = default;
        Board& operator=(Board const& ref) = default;

        [[nodiscard]] bool isEmpty(unsigned int ndx) const;
        [[nodiscard]] Piece getType(unsigned int ndx) const;
        [[nodiscard]] Color getSide(unsigned int ndx) const;
        [[nodiscard]] bool hasMoved(unsigned int ndx) const;
        [[nodiscard]] int getValue(unsigned int ndx) const;
        [[nodiscard]] bool inCheck(unsigned int ndx) const;
        [[nodiscard]] bool isPromoted(unsigned int ndx) const;
        void setType(unsigned int ndx, Piece type);
        void setSide(unsigned int ndx, Color side);
        void setMoved(unsigned int ndx, bool hasMoved);
        void setCheck(unsigned int ndx, bool inCheck);
        void setPromoted(unsigned int ndx, bool promoted);
        static vector<string> to_string(Board const& b);

        [[nodiscard]] Move lastMove() const { return history.empty() ? Move() : history.back(); }

        void generateMoveLists();

        [[nodiscard]] bool checkDrawByRepetition(Move const& move, int limit = -1) const;

        [[nodiscard]] bool kingIsInCheck(Color side) const;

        void executeMove(Move& move);

        /**
         * Advance the total number of moves in the game.
         * Also toggle which players turn it is, and generates
         * the following changes in the board:
         *
         *      + .turn  contains the color of the new player to move next
         *      + .turns contains the total number of moves made so far
         *
         *      + move list for current     moving player are in moves1     (turn's Side color:
         * Black or White)
         *      + move list for current non-moving player are in moves2     ( ((turn + 1) % 2)'s
         * Side color: Black or White)
         *      + piece list for current     moving player are in pieces1
         *      + piece list for current non-moving player are in pieces2
         *
         */
        void advanceTurn();

        [[nodiscard]] MoveList getMoves(Color side, bool checkKing) const;
        [[nodiscard]] MoveList getMovesSorted(Color side) const;

        MoveList cleanupMoves(MoveList& moves, Color side) const;

        static bool isValidSpot(unsigned int col, unsigned int row);

        void addMoveIfValid(MoveList& moves, unsigned int fromCol, unsigned int fromRow,
                            unsigned int toCol, unsigned int toRow) const;

        /**
         * Get a list of all possible moves for a pawn at the given location on the board.
         *
         * @param col The column on the board to get moves from
         * @param row The row on the board to get moves from
         * @return A new vector<Move> containing all possible moves a pawn could make from the given
         * spot
         */
        [[nodiscard]] MoveList getPawnMoves(unsigned int col, unsigned int row) const;

        [[nodiscard]] bool addSlider(MoveList& moves, unsigned int col, unsigned int row,
                                     unsigned int x, unsigned int y) const;

        /**
         * Get a list of all possible moves for a rook at the given location on the board.
         *
         * @param col The column on the board to get moves from
         * @param row The row on the board to get moves from
         * @return A new vector<Move> containing all possible moves a rook could make from the given
         * spot
         */
        [[nodiscard]] MoveList getRookMoves(int col, int row) const;

        /**
         * Get a list of all possible moves for a knight at the given location on the board.
         *
         * @param col The column on the board to get moves from
         * @param row The row on the board to get moves from
         * @return A new vector<Move> containing all possible moves a knight could make from the
         * given spot
         */
        [[nodiscard]] MoveList getKnightMoves(int col, int row) const;

        /**
         * Get a list of all possible moves for a bishop at the given location on the board.
         *
         * @param col The column on the board to get moves from
         * @param row The row on the board to get moves from
         * @return A new vector<Move> containing all possible moves a bishop could make from the
         * given spot
         */
        [[nodiscard]] MoveList getBishopMoves(int col, int row) const;

        /**
         * Get a list of all possible moves for a queen at the given location on the board.
         *
         * @param col The column on the board to get moves from
         * @param row The row on the board to get moves from
         * @return A new vector<Move> containing all possible moves a queen could make from the
         * given spot
         */
        [[nodiscard]] MoveList getQueenMoves(int col, int row) const;

        /**
         * Get a list of all possible moves for a king at the given location on the board.
         *
         * @param col The column on the board to get moves from
         * @param row The row on the board to get moves from
         * @return A new vector<Move> containing all possible moves a king could make from the given
         * spot
         */
        [[nodiscard]] MoveList getKingMoves(int col, int row) const;
    };

}  // namespace chess
