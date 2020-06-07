/**
 * The MoveCache class stores known board states and the moves
 * made from them in order to re-use previous computed moves
 * without the computational overhead
 *
 */

#pragma once

#include <board.h>
#include <chessutil.h>
#include <move.h>

#include <map>
#include <mutex>
#include <string>

namespace chess {
  using std::map;
  using std::mutex;
  using std::string;

  class MoveCache {
  public:
    struct Entry {
      int hit, changed, movesExamined;
      Move move;

      Entry() = default;
      Entry(Entry const& ref) = default;

      explicit Entry(Move const &m, int examined, int h = 1, int c = 0) :
        hit(h), changed(c), movesExamined(examined), move(m) { }
    };

  private:
    using SideMapType = map<string, Entry>;
    using MoveCacheType = map<unsigned int, SideMapType>;

    static MoveCacheType cache;
    static mutex cacheMutex;

    static string createKey(const Board &board);

    static int num_offered;
    static int num_entries;
    static int num_lookups;
    static int num_changed;
    static int num_found;

  public:
    static Entry lookup(const Board &board, unsigned int side);
    static void offer(const Board &board, Move &move, unsigned int side, int movesExamined);
    static void showMetrics();
  };

}  // namespace chess
