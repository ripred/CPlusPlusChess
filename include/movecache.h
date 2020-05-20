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
#include <string>

namespace chess {
  using std::map;
  using std::string;
  class MoveCache {
  public:
    struct Entry {
      Move move;
      int hit, changed;
      Entry() : hit(1), changed(0) {}
    };

  private:
    static char constexpr symbols[2][8]{{' ', 'P', 'R', 'N', 'B', 'Q', 'K', ' '},
                                        {' ', 'p', 'r', 'n', 'b', 'q', 'k', ' '}};
    static map<string, Entry> cache;
    static float minRatio;
    static int minHits;

    static string createKey(const Board& board) {
      char key[BOARD_SIZE]{};
      for (auto i = 0; i < BOARD_SIZE; ++i) {
        unsigned int p = board.board[i];
        key[i] = symbols[getSide(p)][getType(p)];
      }
      return string(key);
    }

  public:
    Move lookup(const Board& board) {
      string key = createKey(board);
      bool exists = false;
      Entry hit;

      exists = cache.find(key) != cache.end();
      if (!exists) {
        return Move();
      }
      hit = cache[key];
      float ratio = 1.0;
      if (hit.changed > 0) {
        ratio = float(hit.hit) / float(hit.changed);
      }
      if (hit.hit >= minHits && ratio >= minRatio) {
        return hit.move;
      }
      return Move();
    }

    void offer(const Board& board, Move& move) {
      if (!move.isValid(board)) return;
      string key = createKey(board);
      bool exists = cache.find(key) != cache.end();
      bool add = false;
      Entry hit;
      if (exists) {
        hit = cache[key];
        if (move.getValue() > hit.move.getValue()) {
          hit.move = move;
          hit.changed++;
          add = true;
        }
      } else {
        add = true;
      }
      if (add) {
        hit.hit++;
        cache[key] = hit;
      }
    }
  };
}  // namespace chess
