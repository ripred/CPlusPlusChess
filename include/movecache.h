/**
 * The MoveCache class stores known board states and the moves made from them in order to
 * re-use previous computed moves without the computational overhead
 *
 */

#pragma once

#include <board.h>
#include <chessutil.h>
#include <move.h>

#include <map>
#include <memory>
#include <mutex>
#include <string>

namespace chess {
  using std::map;
  using std::mutex;
  using std::string;
  using std::unique_ptr;

  /// Entry contains the data kept for every cached move
  struct Entry {
    Move move;
    int movesExamined{};
    int numRetries{};
    int numBetter{};

    Entry() = default;
    Entry(const Entry& ref) = default;
    Entry& operator=(const Entry& ref) = default;

    Entry(Move const& m, int examined)
        : move(m), movesExamined(examined), numRetries(0), numBetter(0) {}

    Entry(Move const& m, int examined, int value) : Entry(m, examined) { move.setValue(value); }

    [[nodiscard]] bool isValid() const { return move.isValid(); }
    [[nodiscard]] int getValue() const { return move.getValue(); }
    void setValue(int const value) { move.setValue(value); }
  };

  class MoveCache {
    using SideMapType = map<string, Entry>;
    using MoveCacheType = map<Piece, SideMapType>;

  public:
    int num_offered;
    int num_entries;
    int num_lookups;
    int num_changed;
    int num_found;
    unique_ptr<mutex> pCacheMutex;
    MoveCacheType cache;

    MoveCache();
    ~MoveCache() = default;

    static string createKey(const Board& board);

    void offer(Board const& board, Move const& move, Color side, int value, int movesExamined);
    Entry lookup(Board const& board, Color side);

    double getRisk(Board const& board, Color side);
    void increaseMoveUsedCount(Board const& board, Color side);
    void increaseMoveImprovedCount(Board const& board, Color side);

    void showMetrics() const;
  };

}  // namespace chess
