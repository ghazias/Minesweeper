#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

#define REAL_GAME 1

// TODO write bomb_count_calculator function

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

std::vector<std::pair<int, int>> generate_bomb_locations(
    std::size_t width, std::size_t height, std::size_t total_bomb_count) {
  std::vector<std::pair<int, int>> bomb_coordinates{};

  for (std::size_t i = 0; i < total_bomb_count; ++i) {
    bomb_coordinates.push_back({rand() % height, rand() % width});
  }
  // TODO write helper function which checks and replaces vector's duplicates
  return bomb_coordinates;
}

class Board {
 public:
  Board(const std::size_t width, const std::size_t height,
        const std::size_t total_bomb_count)
      : tiles_{height, std::vector<Tile>{width}},
        total_bomb_count_{total_bomb_count},
        remaining_tiles_{width * height} {
    auto bomb_locations =
        generate_bomb_locations(width, height, total_bomb_count);

    for (const auto &[row, col] : bomb_locations) {
      std::cout << row << ", " << col << "\n";
      tiles_[row][col].set_is_bomb(true);
    }

    set_bomb_counts();
  }

  const std::vector<std::vector<Tile>> &tiles() const { return tiles_; };
  std::vector<std::vector<Tile>>
      tiles_;  // each vector in this vector is a row of tiles
               // with the entirety of the vector representing the board

  void set_bomb_counts() {}  // TODO

  bool is_bomb(std::size_t row, std::size_t col) const {
    return tiles()[row][col].is_bomb();
  }

  void flood(std::size_t row, std::size_t col) {
    if (row < 0 || col < 0 || row == tiles().size() ||
        col == tiles()[0].size()) {
      return;
    }

    Tile &current = tiles_[row][col];

    if (!current.is_bomb()) {
      current.set_is_revealed(true);
    }
    if (!current.is_blank()) {
      return;
    }

    for (std::size_t i = row - 1; i < row + 1; ++i) {
      for (std::size_t j = col - 1; j < col + 1; ++j) {
        if (i != row || j != col) {
          flood(i, j);
        }
      }
    }
  }

  int total_bomb_count() const { return total_bomb_count_; }
  std::size_t remaining_tiles() const { return remaining_tiles_; }

  void select(int row, int col, GameAction action) {
    Tile &my_tile = tiles_[row][col];

    switch (action) {
      case GameAction::FLAG:
        my_tile.set_is_flagged(true);
        break;
      case GameAction::UNFLAG:
        my_tile.set_is_flagged(false);
        break;
      case GameAction::REVEAL:
        flood(row, col);
        break;
      default:
        break;
    }
  }

  int width() const { return tiles()[0].size(); }
  int height() const { return tiles().size(); }

  bool is_clear() const { return !(remaining_tiles() - total_bomb_count()); }

 private:
  std::size_t total_bomb_count_{};
  std::size_t remaining_tiles_{};
};

Board create_board(const std::size_t width, const std::size_t height,
                   const std::size_t total_bomb_count) {
  return Board{width, height, total_bomb_count};
}

std::pair<int, int> get_user_choice(const Board &board) {
  std::pair<int, int> choice{};
  // add bounds checking through board reference
  std::cout << "Pick a coordinate to reveal.\n";
  std::cout << "Enter row: ";
  std::cin >> choice.first;

  std::cout << "Enter col: ";
  std::cin >> choice.second;

  return choice;
}

GameAction get_user_action() {
  char choice{};
  std::cout << "Enter action.\n";
  std::cout << "(R)eveal, (F)lag, (U)nflag: ";
  std::cin >> choice;

  if (choice == 'R') {
    return GameAction::REVEAL;
  } else if (choice == 'F') {
    return GameAction::FLAG;
  } else {
    return GameAction::UNFLAG;
  }
}

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
  srand(time(NULL));
  std::size_t width{}, height{}, total_bomb_count{};
  if (argv[1]) {
    switch (std::stoi(argv[1])) {
      // create constexpr variables for these magic values?
      case 1:
        width = 9;
        height = 9;
        total_bomb_count = 10;
        break;
      case 2:
        width = 16;
        height = 16;
        total_bomb_count = 40;
        break;
      case 3:
        width = 30;
        height = 16;
        total_bomb_count = 99;
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

  auto board = create_board(width, height, total_bomb_count);
  auto result = play_game(board);
  game_over(result);

  /*
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
  */
}
