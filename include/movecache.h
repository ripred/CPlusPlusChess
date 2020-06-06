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
  private:
    struct Entry {
      int hit, changed;
      Move move;

      explicit Entry(Move const &m = Move(), int h = 1, int c = 0) : hit(h), changed(c), move(m) {}
    };

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
    static Move lookup(const Board &board, unsigned int side);
    static void offer(const Board &board, Move &move, unsigned int side);
    static void showMetrics();
  };

}  // namespace chess
