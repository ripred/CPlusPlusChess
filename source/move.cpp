//
// Move object representing a piece move
//

#include <move.h>

#include <string>
using std::string;
#include <iostream>
using std::cout, std::cerr, std::endl;

namespace chess {
  Move::Move() : fromCol(0), fromRow(0), toCol(0), toRow(0), from(0), to(0), value(0) {}

  Move::Move(int fromCol, int fromRow, int toCol, int toRow, int value)
      : fromCol(fromCol), fromRow(fromRow), toCol(toCol), toRow(toRow), value(value) {
    this->from = fromCol + fromRow * 8;
    this->to = toCol + toRow * 8;
  }

  Move::Move(Move const& ref)
      : fromCol(ref.fromCol),
        fromRow(ref.fromRow),
        toCol(ref.toCol),
        toRow(ref.toRow),
        from(ref.from),
        to(ref.to),
        value(ref.value) {}

  int Move::getFromCol() const { return fromCol; }
  int Move::getFromRow() const { return fromRow; }
  int Move::getToCol() const { return toCol; }
  int Move::getToRow() const { return toRow; }
  int Move::getFrom() const { return from; }
  int Move::getTo() const { return to; }
  int Move::getValue() const { return value; }

  void Move::setValue(int val) { value = val; }

  bool Move::operator==(Move const& move) const {
    if (this == &move) {
      return true;
    }
    return from == move.from && to == move.to && value == move.value;
  }

  string Move::to_string() const {
    static string const rows[8] = {"a", "b", "c", "d", "e", "f", "g", "h"};

    static string const notations[64]
        = {"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "a7", "b7", "c7", "d7", "e7",
           "f7", "g7", "h7", "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "a5", "b5",
           "c5", "d5", "e5", "f5", "g5", "h5", "a4", "b4", "c4", "d4", "e4", "f4", "g4",
           "h4", "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "a2", "b2", "c2", "d2",
           "e2", "f2", "g2", "h2", "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"};

    static string const coords[64] = {
        "0,0", "1,0", "2,0", "3,0", "4,0", "5,0", "6,0", "7,0", "0,1", "1,1", "2,1", "3,1", "4,1",
        "5,1", "6,1", "7,1", "0,2", "1,2", "2,2", "3,2", "4,2", "5,2", "6,2", "7,2", "0,3", "1,3",
        "2,3", "3,3", "4,3", "5,3", "6,3", "7,3", "0,4", "1,4", "2,4", "3,4", "4,4", "5,4", "6,4",
        "7,4", "0,5", "1,5", "2,5", "3,5", "4,5", "5,5", "6,5", "7,5", "0,6", "1,6", "2,6", "3,6",
        "4,6", "5,6", "6,6", "7,6", "0,7", "1,7", "2,7", "3,7", "4,7", "5,7", "6,7", "7,7"};

    string result = "[Move from " + coords[from] + " to " + coords[to];
    result += " (" + notations[from] + " to " + notations[to] + ") value:" + std::to_string(value)
              + "]";

    return result;
  }
}  // namespace chess
