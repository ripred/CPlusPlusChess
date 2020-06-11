/**
 * The chessutil namespace is used to interpret and/or set the attributes
 * of a chess piece using one int to represent the piece
 */

#pragma once

#include <array>
#include <iostream>
#include <limits>
#include <map>
#include <mutex>
#include <string>
#include <vector>

using std::array;
using std::cerr;
using std::cout;
using std::endl;
using std::map;
using std::mutex;
using std::numeric_limits;
using std::string;
using std::to_string;
using std::vector;

namespace chess {

    using Piece = unsigned int;
    using Color = Piece;
    using PieceList = vector<Piece>;
    using SidePieceMap = map<Piece, PieceList>;
    using PieceMap = map<Piece, SidePieceMap>;

    static int const BOARD_SIZE = 64;

    static Piece const White = 1u;
    static Piece const Black = 0u;

    static int const MAX_VALUE = numeric_limits<signed int>::max() / 2;
    static int const MIN_VALUE = 0 - MAX_VALUE;

    static const int pieceValues[7] = {
        0,         // empty spot value
        10'000,    // pawn value
        30'000,    // knight value
        30'000,    // bishop value
        50'000,    // rook value
        90'000,    // queen value
        MAX_VALUE  // king value
    };

    /// Piece Masks
    /// These bit masks define the usage of the bits in each board spot
    static Piece const Type = 0b00000000'00000000'00000000'00000111u;
    //  static Piece const Unused   = 0b00000000'00000000'00000000'00001000u;
    static Piece const Side = 0b00000000'00000000'00000000'00010000u;
    static Piece const Moved = 0b00000000'00000000'00000000'00100000u;
    static Piece const Check = 0b00000000'00000000'00000000'01000000u;
    static Piece const Promoted = 0b00000000'00000000'00000000'10000000u;

    /// The Piece Types
    static Piece const Empty = 0u;
    static Piece const Pawn = 1u;
    static Piece const Knight = 2u;
    static Piece const Bishop = 3u;
    static Piece const Rook = 4u;
    static Piece const Queen = 5u;
    static Piece const King = 6u;
    //  static Piece const Marker = 7u;

    Piece getType(Piece b);

    bool isEmpty(Piece b);

    int getValue(Piece b);

    Color getSide(Piece b);

    bool hasMoved(Piece b);

    bool inCheck(Piece b);

    bool isPromoted(Piece b);

    Piece setType(Piece b, Piece type);

    Piece setSide(Piece b, Piece side);

    Piece setMoved(Piece b, bool hasMoved);

    Piece setCheck(Piece b, bool inCheck);

    Piece setPromoted(Piece b, bool promoted);

    Piece makeSpot(Piece type, Piece side, bool moved = false, bool inCheck = false);

    // common strings
    string getCoords(int index);

    string getCoords(int file, int rank);

    string getNotate(int index);

    string getNotate(int file, int rank);

    string getName(Piece b);

    string getColor(Piece b);

    string addCommas(long int value);

#define UNUSED(a)  \
    do {           \
        (void)(a); \
    } while (0);

}  // namespace chess
