//!
//! board.cpp
//! properties and methods for managing a chess board
//!

#include <board.h>

#include <algorithm>

using std::find;
using std::toupper;

namespace chess {
    Board::Board() {
        board = array<Piece, BOARD_SIZE>();
        board.fill(Empty);

        board[0 + 0 * 8] = makeSpot(Rook, Black);
        board[1 + 0 * 8] = makeSpot(Knight, Black);
        board[2 + 0 * 8] = makeSpot(Bishop, Black);
        board[3 + 0 * 8] = makeSpot(Queen, Black);
        board[4 + 0 * 8] = makeSpot(King, Black);
        board[5 + 0 * 8] = makeSpot(Bishop, Black);
        board[6 + 0 * 8] = makeSpot(Knight, Black);
        board[7 + 0 * 8] = makeSpot(Rook, Black);
        auto it = begin(board) + 8;
        auto stop = begin(board) + 16;
        while (it < stop) {
            *it++ = makeSpot(Pawn, Black);
        }

        it = begin(board) + 48;
        stop = begin(board) + 56;
        while (it < stop) {
            *it++ = makeSpot(Pawn, White);
        }
        board[0 + 7 * 8] = makeSpot(Rook, White);
        board[1 + 7 * 8] = makeSpot(Knight, White);
        board[2 + 7 * 8] = makeSpot(Bishop, White);
        board[3 + 7 * 8] = makeSpot(Queen, White);
        board[4 + 7 * 8] = makeSpot(King, White);
        board[5 + 7 * 8] = makeSpot(Bishop, White);
        board[6 + 7 * 8] = makeSpot(Knight, White);
        board[7 + 7 * 8] = makeSpot(Rook, White);

        turn = White;

        generateMoveLists();
    }

    bool Board::isEmpty(int const ndx) const { return chess::isEmpty(board[ndx]); }

    Piece Board::getType(int const ndx) const { return chess::getType(board[ndx]); }

    Piece Board::getSide(int const ndx) const { return chess::getSide(board[ndx]); }

    bool Board::hasMoved(int const ndx) const { return chess::hasMoved(board[ndx]); }

    int Board::getValue(int const ndx) const { return chess::getValue(board[ndx]); }

    bool Board::inCheck(int const ndx) const { return chess::inCheck(board[ndx]); }

    bool Board::isPromoted(int const ndx) const { return chess::isPromoted(board[ndx]); }

    void Board::setType(int const ndx, Piece type) {
        board[ndx] = chess::setType(board[ndx], type);
    }

    void Board::setSide(int const ndx, Piece side) {
        board[ndx] = chess::setSide(board[ndx], side);
    }

    void Board::setMoved(int const ndx, bool hasMoved) {
        board[ndx] = chess::setMoved(board[ndx], hasMoved);
    }

    void Board::setCheck(int const ndx, bool inCheck) {
        board[ndx] = chess::setCheck(board[ndx], inCheck);
    }

    void Board::setPromoted(int const ndx, bool promoted = true) {
        board[ndx] = chess::setPromoted(board[ndx], promoted);
    }

    vector<string> Board::to_string(Board const &b) {
        char const blackPieces[7] = {' ', 'p', 'n', 'b', 'r', 'q', 'k'};
        char const whitePieces[7] = {' ', 'P', 'N', 'B', 'R', 'Q', 'K'};

        vector<string> result;
        string line;

        for (auto i = 0; i < BOARD_SIZE; i++) {
            if (i >= 8 && i % 8 == 0) {
                result.push_back(line);
                line.clear();
            }

            auto const type = b.getType(i);
            auto const side = b.getSide(i);

            if (i % 8 == 0) {
                line += std::to_string(8 - (i / 8)) + ' ';
            }
            line += ' ';

            switch (type) {
                case Pawn:
                case Knight:
                case Bishop:
                case Rook:
                case Queen:
                case King:
                    line += ((side == White) ? whitePieces[type] : blackPieces[type]);
                    break;
                default:
                    line += ((((i / 8) + (i % 8)) % 2 == 1) ? ' ' : '.');
                    break;
            }
            line += ' ';
        }
        result.push_back(line);
        result.emplace_back("   a  b  c  d  e  f  g  h");

        return result;
    }

    void Board::generateMoveLists() {
        moves1 = getMovesSorted(turn);
        moves2 = getMovesSorted((turn + 1) % 2);

        // at this point the Side (Black or White) constant is in the .turn value
        // for the board.  Whichever color is to move next, it's moves are in
        // moves1 and the non-moving players moves are in moves2.
    }

    /**
     * Check for draw-by-repetition (same made too many times in a row by a player)
     */
    bool Board::checkDrawByRepetition(Move const &move, int limit /* = -1 */) const {
        if (limit < 0) {
            limit = maxRep;
        }

        // Calculate the number of back and forth moves it would require
        // to repeat ourselves 'limit' times.
        size_t need = static_cast<int>(pow(2.0, (limit + 1)));

        if (history.size() < need) {
            return false;
        }

        int count = 0;
        for (auto i = history.size() - need; i < history.size(); i++) {
            if ((history[i] == move) && (++count >= maxRep)) {
                return true;
            }
        }
        return false;
    }

    /**
     * See if the king is in check for the specified side
     *
     * @param side The side to check for
     * @return true if the king is in check
     * spot
     */
    bool Board::kingIsInCheck(Color const side) const {
        /// find our king
        for (auto ndx = 0; ndx < BOARD_SIZE; ndx++) {
            if (getType(ndx) != King || getSide(ndx) != side) continue;
            /// generate our opponents current moves
            auto const &opponentMoves = getMoves((side + 1) % 2, false);
            /// see if any of our opponents moves can reach the king's spot
            for (auto const &move : opponentMoves) {
                if (move.getTo() == ndx) {
                    return true;
                }
            }
            return false;
        }
        return false;
    }

    /**
     * Make a move of a piece on the board.
     * If the move captures a piece then the Move object is updated with the captured piece.
     *
     * @param move The Move to make
     * @return nothing
     */
    void Board::executeMove(Move &move) {
        int const fx = move.getFromCol();
        int const fy = move.getFromRow();
        int const tx = move.getToCol();
        int const ty = move.getToRow();
        int const fi = move.getFrom();
        int const ti = move.getTo();

        // special check for en passant
        Piece const piece = board[fi];
        Piece const toPiece = board[ti];
        Piece const fromType = chess::getType(piece);
        Piece const toType = chess::getType(toPiece);

        // update the taken pieces list (if necessary)
        PieceList &takenList = (turn == White) ? taken1 : taken2;
        if (fromType == Pawn && toType == Empty && fx != tx) {  // en-passant capture
            takenList.push_back(Pawn);
            move.setCaptured(board[tx + fy * 8]);
            board[tx + fy * 8] = Empty;
        } else {
            if (toType != Empty) {
                // This move captures a piece
                move.setCaptured(toPiece);
                takenList.push_back(toType);
            }
        }

        Color fromSide = chess::getSide(piece);

        /// make the move on the board
        board[ti] = piece;
        board[fi] = Empty;
        setMoved(ti, true);

        // See if this is a Castling move:
        if (fromType == King) {
            int const delta = tx - fx;
            // if this king is moving more than one spot away then we're castling
            if (abs(delta) == 2) {
                // calculate the spot of the rook being moved
                int const rfi = (delta < 0) ? fy * 8 : fy * 8 + 7;      // index to move rook from
                int const rti = (delta < 0) ? fy * 8 + 3 : fy * 8 + 5;  // index to move rook to
                // move the rook
                board[rti] = board[rfi];
                setMoved(rti, true);
                board[rfi] = Empty;
            }
        } else if (fromType == Pawn) {
            // see if this pawn has reached the other side and promote it to a queen if so
            if ((ty == 0 && fromSide == White) || (ty == 7 && fromSide == Black)) {
                setType(ti, Queen);
                setPromoted(ti);
            }
        }

        history.push_back(move);
    }

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
    void Board::advanceTurn() {
        turns++;
        turn = ((turn + 1) % 2);
        generateMoveLists();
    }

    MoveList Board::getMovesSorted(Piece const side) const {
        MoveList moves = getMoves(side, true);
        // sort in descending order by move value
        sort(moves.begin(), moves.end(),
             [](Move const &m1, Move const &m2) { return m1.getValue() > m2.getValue(); });
        return moves;
    }

    /**
     * Get a list of all legal moves currently available for the specified side
     *
     * @param side The side to get moves for
     * @param checkKing Flag indicating whether to check each move to see if it places
     *                  the king in check.
     *                  *NOTE* Setting this flag to true causes the method to call itself
     *                  recursively after making the move for each valid move available to
     *                  the opponent to see if the initial move placed the king in check.
     * @return A list of all valid moves currently available for the specified side
     *
     */
    MoveList Board::getMoves(Piece const side, bool checkKing) const {
        MoveList moves;
        MoveList pieceMoves;

        // We also update the pieces array for this side
        PieceList pieces;

        for (int ndx = 0; ndx < BOARD_SIZE; ndx++) {
            if ((getType(ndx) == Empty) || getSide(ndx) != side) continue;

            // add piece to list of pieces for this side
            Piece b = board[ndx];
            pieces.push_back(b);

            int const col = ndx % 8;
            int const row = ndx / 8;
            switch (getType(ndx)) {
                case Pawn:
                    pieceMoves = getPawnMoves(col, row);
                    moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
                    break;
                case Rook:
                    pieceMoves = getRookMoves(col, row);
                    moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
                    break;
                case Knight:
                    pieceMoves = getKnightMoves(col, row);
                    moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
                    break;
                case Bishop:
                    pieceMoves = getBishopMoves(col, row);
                    moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
                    break;
                case Queen:
                    pieceMoves = getQueenMoves(col, row);
                    moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
                    break;
                case King:
                    pieceMoves = getKingMoves(col, row);
                    moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
                    break;
            }
        }

        if (checkKing) {
            moves = cleanupMoves(moves, side);
        }

        return moves;
    }

    /**
     * Clean a list of moves by removing any moves that place the king in check
     *
     * @param moves The list of Moves to check
     * @param side The side to get moves for
     * @return The list of moves sans any illegal moves that would place the king
     *         in check
     */
    MoveList Board::cleanupMoves(MoveList &moves, Piece const side) const {
        MoveList valid;
        for (Move &move : moves) {
            Board current(*this);
            current.executeMove(move);
            if (!current.kingIsInCheck(side)) {
                valid.push_back(move);
            }
        }

        return valid;
    }

    /**
     * Check coordinates to see if they are valid or off of the board
     *
     * @param col The column to check
     * @param row The row to check
     * @return true if the coorinates are on the board
     */
    bool Board::isValidSpot(int const col, int const row) {
        return (col >= 0) && (col <= 7) && (row >= 0) && (row <= 7);
    }

    /**
     * Add a Move to a list if it is legal
     *
     * @param moves The list to append to if the move is legal
     * @param fromCol The column to move from
     * @param fromRow The row to move from
     * @param toCol The column to move to
     * @param fromRow The row to move to
     * @return nothing.  If the move is legal the specified list is updated to include the move
     */
    void Board::addMoveIfValid(MoveList &moves, int const fromCol, int const fromRow,
                               int const toCol, int const toRow) const {
        if (!isValidSpot(fromCol, fromRow)) {
            return;
        }
        if (!isValidSpot(toCol, toRow)) {
            return;
        }
        int const fi = fromCol + fromRow * 8;
        int const ti = toCol + toRow * 8;

        int value = 0;
        Piece pieceType = getType(fi);
        Piece pieceSide = getSide(fi);

        if (!isEmpty(ti)) {
            if (getSide(fi) == getSide(ti)) {
                return;
            }
            value = getValue(ti);
        }

        // extra checks if moving a pawn...
        if (pieceType == Pawn) {
            int const forward = (pieceSide == Black) ? 1 : -1;
            // if double push
            if (abs(fromRow - toRow) == 2) {
                // not allowed if the pawn has already moved
                if (hasMoved(fi) || !isEmpty(fromCol + (fromRow + forward) * 8)) {
                    return;
                }
            }
            // if advancing in same column
            if (fromCol == toCol) {
                // not allowed if a piece is in the way
                if (!isEmpty(ti)) {
                    return;
                }
            } else {
                // pawns cannot move diagonally unless capturing
                if (isEmpty(ti)) {
                    if (lastMove().getToRow() != (toRow - forward)
                        || lastMove().getToCol() != toCol) {
                        return;
                    }
                    // capturing en passant
                    value = getValue(Pawn);
                } else {
                    // capturing normal
                    value = getValue(ti);
                }
            }
        }

        moves.emplace_back(fromCol, fromRow, toCol, toRow, value);
    }

    /**
     * Get a list of all possible moves for a pawn at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new MoveList containing all possible moves a pawn could make from the given
     * spot
     */
    MoveList Board::getPawnMoves(int const col, int const row) const {
        int const ndx = col + row * 8;
        int const forward = getSide(ndx) == White ? -1 : 1;
        MoveList moves;

        addMoveIfValid(moves, col, row, col, row + forward);
        if (!hasMoved(col + row * 8)) {
            addMoveIfValid(moves, col, row, col, row + forward + forward);
        }

        if (isValidSpot(col - 1, row + forward) && !isEmpty((col - 1) + (row + forward) * 8)) {
            addMoveIfValid(moves, col, row, col - 1, row + forward);
        }

        if (isValidSpot(col + 1, row + forward) && !isEmpty((col + 1) + (row + forward) * 8)) {
            addMoveIfValid(moves, col, row, col + 1, row + forward);
        }

        // en-passant! on the left
        int ep1x = col - 1;
        if (isValidSpot(ep1x, row) && getSide(ep1x + row * 8) != getSide(ndx)) {
            if (lastMove().getToCol() == ep1x && lastMove().getToRow() == row) {
                if (abs(lastMove().getFromRow() - lastMove().getToRow()) > 1) {
                    if (getType(ep1x + row * 8) == Pawn) {
                        addMoveIfValid(moves, col, row, ep1x, row + forward);
                    }
                }
            }
        }

        // en-passant! on the right
        ep1x = col + 1;
        if (isValidSpot(ep1x, row) && getSide(ep1x + row * 8) != getSide(ndx)) {
            if (lastMove().getToCol() == ep1x && lastMove().getToRow() == row) {
                if (abs(lastMove().getFromRow() - lastMove().getToRow()) > 1) {
                    if (getType(ep1x + row * 8) == Pawn) {
                        addMoveIfValid(moves, col, row, ep1x, row + forward);
                    }
                }
            }
        }

        return moves;
    }

    /**
     * Utility method for pieces that can 'slide' one or more spots.  Called
     * by the move generation methods for rooks, bishops and queens.  Basically
     * it is the same as addMoveIfValid(...) but it includes adding end spots
     * occupied by an opponent piece that can be captured and returns true if
     * the check for more spots in this 'slide' should continue or false if the
     * slide has terminated.
     *
     * @param moves Reference to the list of moves to add to
     * @param col The column on the board to move from
     * @param row The row on the board to move from
     * @param x The column on the board to move to
     * @param y The row on the board to move to
     * @return true if the check for more 'slide' spots should
     *         continue for this piece or false if the 'slide' has
     *         completed by going off of the board or terminating
     *         at a non-empty spot.
     */
    bool Board::addSlider(MoveList &moves, int const col, int const row, int const x,
                          int const y) const {
        if (!isValidSpot(x, y)) {
            return false;
        }
        if (!isEmpty(x + y * 8)) {
            if (getSide(col + row * 8) == getSide(x + y * 8)) {
                return false;
            }
            addMoveIfValid(moves, col, row, x, y);
            return false;
        }
        addMoveIfValid(moves, col, row, x, y);
        return true;
    }

    /**
     * Get a list of all possible moves for a rook at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new MoveList containing all possible moves a rook could make from the given
     * spot
     */
    MoveList Board::getRookMoves(int const col, int const row) const {
        MoveList moves;

        for (int offset = 1; offset <= 7; offset++) {
            int const x = col - offset;
            int const y = row;
            if (!addSlider(moves, col, row, x, y)) break;
        }
        for (int offset = 1; offset <= 7; offset++) {
            int const x = col + offset;
            int const y = row;
            if (!addSlider(moves, col, row, x, y)) break;
        }
        for (int offset = 1; offset <= 7; offset++) {
            int const x = col;
            int const y = row - offset;
            if (!addSlider(moves, col, row, x, y)) break;
        }
        for (int offset = 1; offset <= 7; offset++) {
            int const x = col;
            int const y = row + offset;
            if (!addSlider(moves, col, row, x, y)) break;
        }
        return moves;
    }

    /**
     * Get a list of all possible moves for a knight at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new MoveList containing all possible moves a knight could make from the given
     * spot
     */
    MoveList Board::getKnightMoves(int const col, int const row) const {
        MoveList moves;

        addMoveIfValid(moves, col, row, col - 1, row - 2);
        addMoveIfValid(moves, col, row, col + 1, row - 2);
        addMoveIfValid(moves, col, row, col - 1, row + 2);
        addMoveIfValid(moves, col, row, col + 1, row + 2);
        addMoveIfValid(moves, col, row, col - 2, row - 1);
        addMoveIfValid(moves, col, row, col + 2, row - 1);
        addMoveIfValid(moves, col, row, col - 2, row + 1);
        addMoveIfValid(moves, col, row, col + 2, row + 1);

        return moves;
    }

    /**
     * Get a list of all possible moves for a bishop at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new MoveList containing all possible moves a bishop could make from the given
     * spot
     */
    MoveList Board::getBishopMoves(int const col, int const row) const {
        MoveList moves;

        for (int offset = 1; offset <= 7; offset++) {
            int const x = col - offset;
            int const y = row - offset;
            if (!addSlider(moves, col, row, x, y)) break;
        }
        for (int offset = 1; offset <= 7; offset++) {
            int const x = col + offset;
            int const y = row - offset;
            if (!addSlider(moves, col, row, x, y)) break;
        }
        for (int offset = 1; offset <= 7; offset++) {
            int const x = col - offset;
            int const y = row + offset;
            if (!addSlider(moves, col, row, x, y)) break;
        }
        for (int offset = 1; offset <= 7; offset++) {
            int const x = col + offset;
            int const y = row + offset;
            if (!addSlider(moves, col, row, x, y)) break;
        }

        return moves;
    }

    /**
     * Get a list of all possible moves for a queen at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new MoveList containing all possible moves a queen could make from the given
     * spot
     */
    MoveList Board::getQueenMoves(int const col, int const row) const {
        MoveList moves;
        moves = getRookMoves(col, row);
        MoveList bishopMoves = getBishopMoves(col, row);
        moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());
        return moves;
    }

    /**
     * Get a list of all possible moves for a king at the given location on the board.
     *
     * @param col The column on the board to get moves from
     * @param row The row on the board to get moves from
     * @return A new MoveList containing all possible moves a king could make from the given
     * spot
     */
    MoveList Board::getKingMoves(int const col, int const row) const {
        int const ndx = col + row * 8;
        MoveList moves;

        addMoveIfValid(moves, col, row, col - 1, row - 1);
        addMoveIfValid(moves, col, row, col + 1, row - 1);
        addMoveIfValid(moves, col, row, col - 1, row + 1);
        addMoveIfValid(moves, col, row, col + 1, row + 1);
        addMoveIfValid(moves, col, row, col, row - 1);
        addMoveIfValid(moves, col, row, col, row + 1);
        addMoveIfValid(moves, col, row, col - 1, row);
        addMoveIfValid(moves, col, row, col + 1, row);

        // check for king side castling:
        if (!hasMoved(ndx) && isEmpty((col + 1) + row * 8) && isEmpty((col + 2) + row * 8)
            && getType((col + 3) + row * 8) == Rook && !hasMoved((col + 3) + row * 8)) {
            addMoveIfValid(moves, col, row, col + 2, row);
        }

        // check for queen side castling:
        if (!hasMoved(ndx) && isEmpty((col - 1) + row * 8) && isEmpty((col - 2) + row * 8)
            && isEmpty((col - 3) + row * 8) && getType((col - 4) + row * 8) == Rook
            && !hasMoved((col - 4) + row * 8)) {
            addMoveIfValid(moves, col, row, col - 2, row);
        }

        return moves;
    }
}  // namespace chess
