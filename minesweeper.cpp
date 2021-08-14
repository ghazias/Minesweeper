#include <cstdlib>
#include <iostream>
#include <set>
#include <utility>
#include <vector>

enum class GameResult { WIN, LOSE };
enum class GameAction { FLAG, UNFLAG, REVEAL };

class Tile {
 public:
  int bomb_count() const { return bomb_count_; }
  bool is_bomb() const { return is_bomb_; }
  bool is_blank() const { return bomb_count_ == 0 && !is_bomb_; }
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

std::pair<int, int> gen_coord(std::size_t width, std::size_t height) {
  return {rand() % height, rand() % width};
}

std::set<std::pair<int, int>> generate_bomb_locations(
    std::size_t width, std::size_t height, std::size_t total_bomb_count) {
  std::set<std::pair<int, int>> bombs{};
  while (bombs.size() < total_bomb_count) {
    bombs.insert(gen_coord(width, height));
  }

  return bombs;
}

class Board {
 public:
  Board(const std::size_t width, const std::size_t height,
        const std::size_t total_bomb_count)
      : total_bomb_count_{total_bomb_count},
        remaining_tiles_{width * height},
        tiles_{height, std::vector<Tile>{width}},
        bomb_tiles_(generate_bomb_locations(width, height, total_bomb_count)) {
    for (const auto &[row, col] : bomb_tiles_) {
      tiles_[row][col].set_is_bomb(true);
    }
    set_bomb_counts();
  }
  const std::vector<std::vector<Tile>> &tiles() const { return tiles_; };
  const std::set<std::pair<int, int>> &bomb_tiles() const {
    return bomb_tiles_;
  };

  void reveal_all_bombs() {
    for (auto &[row, col] : bomb_tiles_) {
      tiles_[row][col].set_is_revealed(true);
    }
  }

  void decrement_remaining() {
    --remaining_tiles_;
  }

  bool is_edge(std::size_t row, std::size_t col) {
    return (row > 0 && row < height()) && (col > 0 && col < width());
  }

  bool is_corner(std::size_t row, std::size_t col) {
    return (row == 0 || row == height()) && (col == 0 || col == width());
  }

  // TODO add member variables to each tile containing location coordinates and
  // refactor count_col as a tile member which only takes direction maybe also
  // add member bool labels to mark corners and edges?
  void set_bomb_counts() {
    for (std::size_t row = 0; row < height(); ++row) {
      for (std::size_t col = 0; col < width(); ++col) {
        auto count = count_surrounding_bombs(row, col);
        tiles_[row][col].set_bomb_count(count);
      }
    }
  }
  #if 0
    void set_bomb_counts() {
    for (std::size_t row = 0; row < height(); ++row) {
      int previous_count{};
      int current_count = count_bombs_in_column(row, 0);
      for (std::size_t col = 0; col < width(); ++col) {
        int next_count = count_bombs_in_column(row, col+1);
        auto total = previous_count + current_count + next_count;
        tiles_[row][col].set_bomb_count(total);
        previous_count = current_count;
        current_count = next_count;
      }
    }
  }
  
  int count_bombs_in_column(int row, int col) const {
      int count{};
      for (int i = row-1; i <= row + 1; ++i) {
          if (is_in_bounds(i, col) 
            && tiles_[i][col].is_bomb()) {
              ++count;
          }
      }
      return count;
  }
  #endif


  int count_surrounding_bombs(int row, int col) const {
      int count{};
      for (int i = row-1; i <= row + 1; ++i) {
          for (int j = col - 1; j <= col + 1; ++j) {
              if (is_in_bounds(i, j)
                && tiles_[i][j].is_bomb()) {
                  ++count;
              }
          }
      }
      return count;
  }
  
  bool is_in_bounds(int row, int col) const {
      return row >= 0 
                && col >= 0 
                && row < static_cast<int>(height()) 
                && col < static_cast<int>(width());
  }

  bool is_bomb(std::size_t row, std::size_t col) const {
    return tiles()[row][col].is_bomb();
  }
  int total_bomb_count() const { return total_bomb_count_; }
  std::size_t remaining_tiles() const { return remaining_tiles_; }

  void flood(std::size_t row, std::size_t col) {
    if (row < 0 || col < 0 || row == height() || col == width()) {
      return;
    }  // boundary checking

    Tile &current = tiles_[row][col];

    if (!current.is_bomb() && !current.is_revealed()) {
      current.set_is_revealed(true);
    } else {
      return;
    }  // check if valid space to reveal

    if (!current.is_blank()) {
      return;
    }

    for (auto i = row - 1; i < row + 1; ++i) {
      for (auto j = col - 1; j < col + 1; ++j) {
        if (i != row || j != col) {  // skip self
          flood(i, j);
        }  // recursive reveal
      }
    }
  }

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
        return;
    }
  }

  std::size_t width() const { return tiles()[0].size(); }
  std::size_t height() const { return tiles().size(); }
  bool is_clear() const { return !(remaining_tiles() - total_bomb_count()); }

 private:
  std::size_t total_bomb_count_{};
  std::size_t remaining_tiles_{};
  std::vector<std::vector<Tile>> tiles_{};
  std::set<std::pair<int, int>> bomb_tiles_{};
};

std::pair<int, int> get_user_choice(const Board &board) {
  std::pair<int, int> choice{};  // couldn't I take size_t's?
  do {
    std::cout << "Pick a coordinate to reveal.\n";
    std::cout << "Enter row: ";
    std::cin >> choice.first;

    std::cout << "Enter col: ";
    std::cin >> choice.second;
  } while (!(choice.first >= 0 &&
             static_cast<std::size_t>(choice.first) <= board.tiles().size()) &&
           !(choice.second >= 0 && static_cast<std::size_t>(choice.second) <=
                                       board.tiles()[0].size()));

  return choice;
}

GameAction get_user_action() {
  char choice{};
  std::cout << "Enter action.\n";
  std::cout << "(R)eveal, (F)lag, (U)nflag: ";
  std::cin >> choice;
  std::cout << "\n";
  if (choice == 'R') {
    return GameAction::REVEAL;
  } else if (choice == 'F') {
    return GameAction::FLAG;
  } else {
    return GameAction::UNFLAG;
  }
}

void print_header(std::size_t size) {
  for (std::size_t i = 0; i < size; ++i) {
    std::cout << " -";
  }
}

void display(const Board &board) {
  std::size_t row_label{};
  for (std::size_t col_label = 0; col_label < board.tiles()[0].size();
       ++col_label) {
    std::cout << " " << col_label;
  }
  
  std::cout << "\tRemaining tiles: " << board.remaining_tiles();
  std::cout << "\n";
  print_header(board.tiles().size());
  std::cout << "\n";

  for (const auto &row : board.tiles()) {
    std::cout << "|";
    for (const auto &tile : row) {
      if (tile.is_flagged()) {
        std::cout << "F";  // Flagged space
      } else if (!tile.is_revealed()) {
        std::cout << "O";  // Unrevealed space
      } else if (tile.is_revealed() && tile.is_bomb()) {
        std::cout << "X";  // Bomb space
      } else if (tile.is_blank()) {
        std::cout << " ";  // Empty space with no adjacent bombs
      } else {
        std::cout << tile.bomb_count();  // Empty space with adjacent bombs
      }
      std::cout << "|";
    }
    std::cout << " " << row_label++ << "\n";
    print_header(board.tiles().size());
    std::cout << "\n";
  }
}

GameResult play_game(Board &board) {
  while (true) {
    display(board);
    auto [row, column] = get_user_choice(board);
    auto action = get_user_action();
    if (action == GameAction::REVEAL && board.is_bomb(row, column)) {
      board.reveal_all_bombs();
      return GameResult::LOSE;
    }  // lose state
    board.select(row, column, action);
    board.decrement_remaining();
    if (board.is_clear()) {
      return GameResult::WIN;
    }  // win state
  }
}

void game_over(GameResult result, Board &board) {
  if (result == GameResult::LOSE) {
    board.reveal_all_bombs();
    display(board);
    std::cout << "You blew it... Game over.\n";
  } else {
    std::cout << "Congrats! You won!";
  }
}

int main(int, char *argv[]) {
  srand(time(NULL));
  std::size_t width{}, height{}, total_bomb_count{};

  if (argv[1]) {
    switch (std::stoi(argv[1])) {
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
    std::cout << "Include game difficulty argument: ";
    std::cout << "1 for easy, 2 for medium, 3 for hard\n";
    return 1;
  }

  auto board = Board{width, height, total_bomb_count};
  auto result = play_game(board);
  game_over(result, board);
}
