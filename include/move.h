//
// Move object representing a piece move
//

#pragma once

#include <string>
using std::string;

namespace chess {
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
    Move(int fromCol, int fromRow, int toCol, int toRow, int value);
    Move(Move const& ref);

    int  getFromCol() const;
    int  getFromRow() const;
    int  getToCol() const;
    int  getToRow() const;
    int  getFrom() const;
    int  getTo() const;
    int  getValue() const;
    void setValue(int value);

    bool operator == (Move const& move);

    string to_string() const;
  };
}
