//
// Board.h
// properties and methods for managing a chess board
//

#pragma once

#include "chessutil.h"

namespace chess {
  class Board {
  private:
  public:
    int board[BOARD_SIZE];

  public:
    Board();
    Board(Board const& ref);
  };

}  // namespace chess
