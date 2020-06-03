/**
 * The MoveCache class stores known board states and the moves
 * made from them in order to re-use previous computed moves
 * without the computational overhead
 *
 */

#pragma once

#include <chessutil.h>
#include <board.h>
#include <move.h>

#include <string>
#include <map>

namespace chess {
  using std::map;
  using std::string;

  class MoveCache {
  public:
    struct Entry {
      Move move;
      int hit, changed;
      explicit Entry(Move const& m = Move(), int h = 1, int c = 0) : move(m), hit(h), changed(c) {}
    };

  private:
    static char constexpr symbols[2][8]{{' ', 'P', 'R', 'N', 'B', 'Q', 'K', ' '},
                                        {' ', 'p', 'r', 'n', 'b', 'q', 'k', ' '}};
    static map<string, Entry> cache;
    static string createKey(const Board& board);

    static int num_offered;
    static int num_entries;
    static int num_lookups;
    static int num_changed;
    static int num_found;

  public:
    static Move lookup(const Board& board);
    static void offer(const Board& board, Move& move);

    static void showMetrics() {
      using std::cout, std::endl;
      cout << "Offered: " << num_offered << endl;
      cout << "Lookups: " << num_lookups << endl;
      cout << "Changed: " << num_changed << endl;
      cout << "Entries: " << num_entries << endl;
      cout << "Found  : " << num_found << endl;
    }
  };
}  // namespace chess
