//
// Move object representing a piece move
//

#include <board.h>
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
    if (this == &move) return true;
    return from == move.from && to == move.to && value == move.value;
  }

  string Move::to_string(unsigned int const flag) const {
    string result;
    if (flag & 1) result += coords[from] + " to " + coords[to] + " ";
    if (flag & 2) result += notations[from] + " to " + notations[to];
    if (flag & 4) result += " value:" + std::to_string(value);

    return result;
  }

  bool Move::isValid(void) const { return from != to; }

  bool Move::isValid(Board const& board) const {
    //  cout  << "Move::isValid(Board const&board):\n" \
//        << "  from = " << from << ", to = " << to << endl \
//        << "  board.board[" << from << "] = " << board.board[from] << endl \
//        << "  from != to := " << (from != to) << ", board.getType(from) := " << board.getType(from) << endl \
//        << "  board.getType(from) != Empty := " << (board.getType(from) != Empty) << endl;
    return (from != to) && (board.getType(from) != Empty);
  }

}  // namespace chess
