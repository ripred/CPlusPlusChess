//
// Move object representing a piece move
//

#pragma once

#include <string>
using std::string;

namespace chess {
  class Board;
  class Move {
  private:
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
    Move(Move const& ref);

    [[nodiscard]] int getFromCol() const;
    [[nodiscard]] int getFromRow() const;
    [[nodiscard]] int getToCol() const;
    [[nodiscard]] int getToRow() const;
    [[nodiscard]] int getFrom() const;
    [[nodiscard]] int getTo() const;
    [[nodiscard]] int getValue() const;

    void setValue(int value);

    bool operator==(Move const& move) const;

    [[nodiscard]] bool isValid(void) const;
    [[nodiscard]] bool isValid(Board const& board) const;

    [[nodiscard]] string to_string(unsigned int flag = 0b111) const;
  };
}  // namespace chess
