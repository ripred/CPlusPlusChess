//
// Move object representing a piece move
//

#pragma once

#include <string>

using std::string;

#include <chessutil.h>

namespace chess {
    class Board;

    class Move;
    class Move {
    private:
        Piece captured;
        unsigned int fromCol;
        unsigned int fromRow;
        unsigned int toCol;
        unsigned int toRow;
        unsigned int from;
        unsigned int to;
        int value;

    public:
        Move();

        Move(unsigned int fromCol, unsigned int fromRow, unsigned int toCol, unsigned int toRow,
             int value);

        Move(Move const &ref) = default;

        Move &operator=(Move const &ref) = default;

        bool operator==(Move const &move) const;

        [[nodiscard]] unsigned int getFromCol() const;

        [[nodiscard]] unsigned int getFromRow() const;

        [[nodiscard]] unsigned int getToCol() const;

        [[nodiscard]] unsigned int getToRow() const;

        [[nodiscard]] unsigned int getFrom() const;

        [[nodiscard]] unsigned int getTo() const;

        [[nodiscard]] int getValue() const;

        [[nodiscard]] Piece getCaptured() const;

        [[nodiscard]] bool isCapture() const;

        void setValue(int value);

        void setCaptured(Piece p);

        [[nodiscard]] bool isValid() const;

        [[nodiscard]] bool isValid(Board const &board) const;

        [[nodiscard]] string to_string(unsigned int flag = 0b111u) const;
    };

    using MoveList = vector<Move>;

}  // namespace chess
