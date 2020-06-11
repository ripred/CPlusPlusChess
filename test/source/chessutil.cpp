#include <doctest/doctest.h>

#if defined(_WIN32) || defined(WIN32)
// apparently this is required to compile in MSVC++
#    include <sstream>
#endif

#include <chessutil.h>

namespace chess {
    /**
     * unit tests for low level chessutil functions
     *
     */
    TEST_CASE("chess::chessutil") {
        // temp:
        // force ref to chess:pieceValues until I figure out why this
        // is being reported as optimized out.
        int unused = pieceValues[Pawn];
        int unused2 = unused;
        unused = unused2;

        Piece const whitePawn = Promoted | makeSpot(Pawn, White, false, false);
        Piece const blackPawn = makeSpot(Pawn, Black, false, false);
        Piece const movedWhitePawn = setCheck(setMoved(whitePawn, true), true);
        Piece const movedBlackPawn = setCheck(setMoved(blackPawn, true), true);

        CHECK(getType(whitePawn) == Pawn);
        CHECK(getType(blackPawn) == Pawn);
        CHECK(getType(Rook) == Rook);
        CHECK(getType(Knight) == Knight);
        CHECK(getType(Bishop) == Bishop);
        CHECK(getType(Queen) == Queen);
        CHECK(getType(King) == King);

        CHECK(isEmpty(0));
        CHECK(isEmpty(Empty));
        CHECK(!isEmpty(whitePawn));
        CHECK(!isEmpty(blackPawn));
        CHECK(!isEmpty(Rook));
        CHECK(!isEmpty(Knight));
        CHECK(!isEmpty(Bishop));
        CHECK(!isEmpty(Queen));
        CHECK(!isEmpty(King));

        CHECK(getValue(whitePawn) == pieceValues[Pawn]);
        CHECK(getValue(blackPawn) == pieceValues[Pawn]);
        CHECK(getValue(Rook) == pieceValues[Rook]);
        CHECK(getValue(Knight) == pieceValues[Knight]);
        CHECK(getValue(Bishop) == pieceValues[Bishop]);
        CHECK(getValue(Queen) == pieceValues[Queen]);
        CHECK(getValue(King) == pieceValues[King]);

        CHECK(getSide(whitePawn) == 1);
        CHECK(getSide(blackPawn) == 0);

        CHECK(!hasMoved(whitePawn));
        CHECK(!hasMoved(blackPawn));
        CHECK(hasMoved(movedWhitePawn));
        CHECK(hasMoved(movedBlackPawn));

        CHECK(!inCheck(whitePawn));
        CHECK(!inCheck(blackPawn));
        CHECK(inCheck(movedWhitePawn));
        CHECK(inCheck(movedBlackPawn));

        CHECK(setType(Empty, Pawn) == getType(whitePawn));
        CHECK(setType(Empty, Pawn) == getType(blackPawn));

        CHECK(getSide(setSide(setType(Empty, Pawn), White)) == White);
        CHECK(getSide(setSide(setType(Empty, Pawn), Black)) == Black);

        CHECK(setMoved(setSide(setType(Empty, Pawn), White), true) == (Moved | Pawn | Side));
        CHECK(setMoved(setSide(setType(Empty, Pawn), Black), true) == (Moved | Pawn));

        CHECK(getCoords(44) == string("4,5"));
        CHECK(getCoords(4, 5) == string("4,5"));
        CHECK(getNotate(4, 5) == string("e3"));
        CHECK(getNotate(44) == string("e3"));
        CHECK(getName(Pawn) == string("Pawn"));
        CHECK(getName(Knight) == string("Knight"));
        CHECK(getName(Bishop) == string("Bishop"));
        CHECK(getName(Rook) == string("Rook"));
        CHECK(getName(Queen) == string("Queen"));
        CHECK(getName(King) == string("King"));
        CHECK(getColor(whitePawn) == string("White"));
        CHECK(getColor(blackPawn) == string("Black"));

        CHECK(setPromoted(blackPawn, true) == (blackPawn | Promoted));
        CHECK(isPromoted(whitePawn));
    }
}  // namespace chess
