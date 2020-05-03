//
// Move object representing a piece move
//

#include <move.h>

#include <string>
using std::string;

namespace chess {
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
    string rows[]{"a", "b", "c", "d", "e", "f", "g", "h"};
    string chessNotation = rows[fromCol] + std::to_string(8 - fromRow) + " to " + rows[toCol]
                           + std::to_string(8 - toRow);

    string result = "[Move from " + std::to_string(fromCol) + "," + std::to_string(fromRow) + " to "
                    + std::to_string(toCol) + "," + std::to_string(toRow) + " (" + chessNotation
                    + ") value:" + std::to_string(value) + "]";

    return result;
  }
}  // namespace chess
