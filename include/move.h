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
        int fromCol;
        int fromRow;
        int toCol;
        int toRow;
        int from;
        int to;
        int value;

    public:
        Move();

        Move(int fromCol, int fromRow, int toCol, int toRow, int value);

        Move(Move const &ref) = default;

        Move &operator=(Move const &ref) = default;

        bool operator==(Move const &move) const;

        [[nodiscard]] int getFromCol() const;

        [[nodiscard]] int getFromRow() const;

        [[nodiscard]] int getToCol() const;

        [[nodiscard]] int getToRow() const;

        [[nodiscard]] int getFrom() const;

        [[nodiscard]] int getTo() const;

        [[nodiscard]] int getValue() const;

        [[nodiscard]] Piece getCaptured() const;

        [[nodiscard]] bool isCapture() const;

        void setValue(int value);

        void setCaptured(Piece p);

        [[nodiscard]] bool isValid() const;

        [[nodiscard]] bool isValid(Board const &board) const;

        [[nodiscard]] string to_string(unsigned int flag = 0b111) const;
    };

    using MoveList = vector<Move>;

}  // namespace chess
