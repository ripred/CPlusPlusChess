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
#include <tuple>

namespace chess {
    using std::map;
    using std::mutex;
    using std::string;
    using std::tuple;
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

        Entry(Move const& m, int const examined)
            : move(m), movesExamined(examined), numRetries(0), numBetter(0) {}

        Entry(Move const& m, int const examined, int const value) {
            move = m;
            move.setValue(value);
            movesExamined = examined;
            numRetries = 0;
            numBetter = 0;
        }

        [[nodiscard]] bool isValid() const { return move.isValid(); }
        [[nodiscard]] bool isValid(Board const& board) const { return move.isValid(board); }
        [[nodiscard]] int getValue() const { return move.getValue(); }
        void setValue(int const value) { move.setValue(value); }

        [[nodiscard]] double getRisk() const {
            return (numRetries == 0) ? 1.0 : double(numBetter) / double(numRetries);
        }

        void increaseMoveUsedCount() { numRetries++; }

        void increaseMoveImprovedCount() { numBetter++; }
    };

    class MoveCache {
    private:
        using EntryFindType = tuple<bool, Entry&>;
        [[nodiscard]] EntryFindType getEntry(Board const& board, Color side) {
            static Entry nonExistent;
            if (cache.find(side) != cache.end()) {
                string const key = createKey(board);
                if (cache[side].find(key) != cache[side].end()) {
                    return EntryFindType(true, cache[side][key]);
                }
            }
            return EntryFindType(false, nonExistent);
        }

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
        Entry lookup(Board const& board);

        double getRisk(Board const& board, Color side);
        void increaseMoveUsedCount(Board const& board, Color side);
        void increaseMoveImprovedCount(Board const& board, Color side);

        void showMetrics() const;
    };

}  // namespace chess
