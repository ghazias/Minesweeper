#include <vector>
#include <utility>

enum class GameResult{ WIN, LOSE };

class Tile {
public:
	bool is_bomb() const { return is_bomb_; }
	bool is_blank() const { return bomb_count_ == 0; }
	private:
		bool is_bomb_{};
		bool is_revealed_{};
		int bomb_count_{};
};

class Board {
public:
	Board(int width, int height, int bomb_count)
{
// auto bomb_locations = generate_bomb_locations(width, height, bomb_count);
}
	const std::vector<std::vector<Tile> >& tiles() const { return tiles_; };
	std::vector<std::vector<Tile> > tiles_;

    bool is_bomb(int row, int col) const {
        // TODO implement this
        return false;
    }

    void select(int row, int col) {
        // TODO
    }

    bool is_clear() const {
        // TODO
        return false;
    }
};
Board create_board(int width, int height, int bomb_count) {
	return Board{width, height, bomb_count};
}

std::pair<int, int> get_user_choice(const Board& board) {
    // TODO
    return {0, 0};
}

void display(const Board& board) {
       for (const auto& row : board.tiles()) {
           for (const auto& tile : row) {
               
           }
       }
}

GameResult play_game(Board& board) {
	while (true) {
		display(board);
		auto [row, column] = get_user_choice(board);
		if (board.is_bomb(row, column)) {
			return GameResult::LOSE;
		}
		board.select(row, column);
		if (board.is_clear()) {
			return GameResult::WIN;
		}
	}
}

 void game_over(GameResult result) {
     // TODO
}

int main() {
	auto board = create_board(10, 15, 7);
	auto result = play_game(board);
	game_over(result);
}

