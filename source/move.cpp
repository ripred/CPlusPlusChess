//
// Move object representing a piece move
//

#include <board.h>

namespace chess {
    using std::to_string;
    Move::Move()
        : captured(Empty), fromCol(0), fromRow(0), toCol(0), toRow(0), from(0), to(0), value(0) {}

    Move::Move(unsigned int fromCol, unsigned int fromRow, unsigned int toCol, unsigned int toRow,
               int value)
        : captured(Empty),
          fromCol(fromCol),
          fromRow(fromRow),
          toCol(toCol),
          toRow(toRow),
          value(value) {
        this->from = fromCol + fromRow * 8;
        this->to = toCol + toRow * 8;
    }

    unsigned int Move::getFromCol() const { return fromCol; }
    unsigned int Move::getFromRow() const { return fromRow; }
    unsigned int Move::getToCol() const { return toCol; }
    unsigned int Move::getToRow() const { return toRow; }
    unsigned int Move::getFrom() const { return from; }
    unsigned int Move::getTo() const { return to; }
    int Move::getValue() const { return value; }
    Piece Move::getCaptured() const { return captured; }
    bool Move::isCapture() const { return captured != Empty; }

    void Move::setValue(int val) { value = val; }
    void Move::setCaptured(Piece p) { captured = p; }

    bool Move::operator==(Move const& move) const {
        if (this == &move) return true;
        return from == move.from && to == move.to;
    }

    string Move::to_string(unsigned int const flag) const {
        string result;
        if (flag & 1u) result += getCoords(from) + " to " + getCoords(to) + " ";
        if (flag & 2u) result += getNotate(from) + " to " + getNotate(to);
        if (flag & 4u) result += " value:" + std::to_string(value);

        return result;
    }

    bool Move::isValid() const { return from != to; }

    bool Move::isValid(Board const& board) const {
        return (from != to) && (board.getType(from) != Empty);
    }

}  // namespace chess
