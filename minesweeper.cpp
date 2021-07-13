#include <iostream>
#include <utility>
#include <vector>

#define REAL_GAME 0

enum class GameResult { WIN, LOSE };
enum class GameAction { FLAG, UNFLAG, REVEAL };

class Tile {
 public:
  int bomb_count() const { return bomb_count_; }
  bool is_bomb() const { return is_bomb_; }
  bool is_blank() const { return bomb_count_ == 0; }
  bool is_revealed() const { return is_revealed_; }
  bool is_flagged() const { return is_flagged_; }

  void set_bomb_count(int bomb_count) { bomb_count_ = bomb_count; }
  void set_is_bomb(bool is_bomb) { is_bomb_ = is_bomb; }
  void set_is_revealed(bool is_revealed) { is_revealed_ = is_revealed; }
  void set_is_flagged(bool is_flagged) { is_flagged_ = is_flagged; }

 private:
  bool is_bomb_{};
  bool is_revealed_{};
  int bomb_count_{};
  bool is_flagged_{};
};

class Board {
 public:
  Board(int width, int height, int total_bomb_count)
      : total_bomb_count_{total_bomb_count}, remaining_tiles_{width * height} {
    // auto bomb_locations = generate_bomb_locations(width, height,
    // total_bomb_count);
  }

  const std::vector<std::vector<Tile>> &tiles() const { return tiles_; };
  std::vector<std::vector<Tile>>
      tiles_;  // each vector in this vector is a row of tiles
               // with the entirety of the vector representing the board

  bool is_bomb(int row, int col) const {
    return tiles()[row - 1][col - 1].is_bomb();
  }

  void flood(int row, int col) {
    Tile &current = tiles_[row - 1][col - 1];
    if (current.bomb_count()) {
      current.set_is_revealed(true);
      return;
    }

    current.set_is_revealed(true);

    for (std::size_t i = row - 1; i < row + 1; ++i) {
      for (std::size_t j = col - 1; j < col + 1; ++j) {
        if (i == row && j == col) {  // skip self
          continue;
        }
        flood(i, j);
      }
    }
  }

  int total_bomb_count() const { return total_bomb_count_; }
  int remaining_tiles() const { return remaining_tiles_; }

  void select(int row, int col, GameAction action) {
    Tile &my_tile = tiles_[row - 1][col - 1];

    switch (action) {
      case GameAction::FLAG:
        my_tile.set_is_flagged(true);
        break;
      case GameAction::UNFLAG:
        my_tile.set_is_flagged(false);
        break;
      case GameAction::REVEAL:
        my_tile.set_is_revealed(true);
        // my_tile.flood(my_tile, row, col);
        break;
      default:
        break;
    }
  }

  bool is_clear() const { return !(remaining_tiles() - total_bomb_count()); }

 private:
  int total_bomb_count_{};  // TODO make const?
  int remaining_tiles_{};
};

Tile create_bomb() {
  Tile bomb_tile{};
  bomb_tile.set_is_bomb(true);

  return bomb_tile;
}

Board create_board(int width, int height, int total_bomb_count) {
  return Board{width, height, total_bomb_count};
}

std::pair<int, int> get_user_choice(const Board &board) {
  std::pair<int, int> choice{};

  std::cout << "Pick a coordinate to reveal.";
  std::cout << "Enter row: ";
  std::cin >> choice.first;
  std::cout << "Enter col: ";
  std::cin >> choice.second;

  return choice;
}

GameAction get_user_action() { return GameAction::REVEAL; }  // TODO

std::string display_helper(const std::vector<Tile> &row) {}  // TODO

void display(const Board &board) {
  // TODO add timer and number of bomb counters as headers to board
  for (const auto &row : board.tiles()) {
    for (const auto &tile : row) {
      if (tile.is_flagged()) {
        std::cout << "F";  // Flagged space
      } else if (!tile.is_revealed()) {
        std::cout << "O";  // Unrevealed space
      } else if (tile.is_revealed() && tile.is_bomb()) {
        std::cout << "X";  // Bomb space
      } else if (tile.is_blank()) {
        std::cout << "_";  // Empty space with no adjacent bombs
      } else {
        std::cout << tile.bomb_count();  // Empty space with adjacent bombs
      }
    }
    std::cout << "\n";
  }
}

GameResult play_game(Board &board) {
  while (true) {
    display(board);
    auto [row, column] = get_user_choice(board);
    auto action = get_user_action();
    if (action == GameAction::REVEAL && board.is_bomb(row, column)) {
      return GameResult::LOSE;
    }  // lose state
    board.select(row, column, action);
    if (board.is_clear()) {
      return GameResult::WIN;
    }  // win state
  }
}

void game_over(GameResult result) {
  if (result == GameResult::LOSE) {
    std::cout << "You blew it... Game over.";
    // TODO add helper function to reveal all bomb spaces and display final
    // board
  } else {
    std::cout << "Congrats! You won!";
  }
  // TODO maybe add cout for time taken?
}

int main(int, char *argv[]) {
#if REAL_GAME
  if (argv[1]) {
    switch (std::stoi(argv[1])) {
      case 1:
        create_board(9, 9, 10);
        break;
      case 2:
        create_board(16, 16, 40);
        break;
      case 3:
        create_board(30, 16, 99);
        break;
      default:
        std::cout << "Invalid choice\n";
        return 1;
    }
  } else {
    std::cout << "Include game difficulty argument, ";
    std::cout << "1 for easy, 2 for medium, 3 for hard\n";
    return 1;
  }
#else
  auto board = create_board(0, 0, 0);
  board.tiles_.push_back({});
  auto &row = board.tiles_.back();
  row.push_back({});
  row.back().set_is_bomb(true);
  row.back().set_is_revealed(true);
  row.push_back({});  // adds revealed bomb tile
  row.back().set_is_flagged(true);
  row.push_back({});  // adds flagged tile
  row.back().set_bomb_count(3);
  row.back().set_is_revealed(true);
  row.push_back({});  // adds revealed bomb tile with 3 adjacent bombs
  row.back().set_is_revealed(true);
  row.push_back({});  // adds empty tile

  board.tiles_.push_back(row);  // adds a duplicate row to the board

  std::cout << "-----------------\n";
  display(board);
  std::cout << "-----------------\n";
#endif

#if REAL_GAME
  auto board = create_board(10, 15, 7);
  auto result = play_game(board);
  game_over(result);
#endif
}
