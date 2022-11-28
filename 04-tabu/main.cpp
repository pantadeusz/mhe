#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <random>
#include <set>
#include <vector>

struct puzzle_t {
  int width;
  int height;
  std::vector<int> board;

  int &get(const int x, const int y) { return board[y * width + x]; }

  /**
   * @brief returns value at given coordinates.
   *
   * -1 inside of a bag
   * 0 - not set
   * >0 inside bag and represents the number of bag points starting here and
   * going up to the border of the bag
   *
   * @param x
   * @param y
   * @return int
   */
  int get(const int x, const int y) const {
    if ((x < 0) || (x >= width) || (y < 0) || (y >= height))
      throw std::invalid_argument("out of board");
    return board.at(y * width + x);
  }

  bool is_in_bag(const int x, const int y) const {
    if ((x < 0) || (x >= width) || (y < 0) || (y >= height)) return false;
    auto v = board.at(y * width + x);
    if ((v == -1) || (v > 0))
      return true;
    else
      return false;
  }
  /**
   * generates new random solution based on this problem definition
   * @return new random solution that is based on the problem definition
   */
  puzzle_t generate_random_solution() const;

  std::vector<puzzle_t> generate_neighbors() const;

  bool next_solution();

  int count_inside_bag_from_point(int x, int y) const;

  int count_bad_summary_size() const;
  int count_inconsistent(bool show_debug = false) const;
};

/**
 * -1 values are in "bag"
 */
std::ostream &operator<<(std::ostream &o, const puzzle_t &puzzle) {
  using namespace std;
  for (int y = 0; y < puzzle.height; y++) {
    o << "|";
    for (int x = 0; x < puzzle.width; x++) {
      auto &val = puzzle.board[y * puzzle.width + x];
      if (val > 0)
        o << setw(4) << val;
      else if (val == 0)
        o << "    ";
      else
        o << " .. ";
      o << "|";
    }
    o << endl;
  }
  return o;
}

int puzzle_t::count_inside_bag_from_point(int x, int y) const {
  const puzzle_t &puzzle = *this;
  using namespace std;
  vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  int sum = 1;
  for (auto [dx, dy] : directions) {
    int cx = x + dx, cy = y + dy;
    try {
      while ((puzzle.get(cx, cy)) != 0) {
        sum++;
        cx += dx;
        cy += dy;
      }
    } catch (...) {
    }
  }
  return sum;
}

int puzzle_t::count_bad_summary_size() const {
  const puzzle_t &puzzle = *this;
  int error = 0;
  for (int x = 0; x < puzzle.width; x++)
    for (int y = 0; y < puzzle.height; y++) {
      if (puzzle.get(x, y) > 0) {
        int count = puzzle.count_inside_bag_from_point(x, y);
        error += std::abs(count - puzzle.get(x, y));
      }
    }
  return error;
}

int puzzle_t::count_inconsistent(bool show_debug) const {
  const puzzle_t &puzzle = *this;
  int error = 0;
  auto puzzle_colors = puzzle;
  for (auto &e : puzzle_colors.board) e = 0;
  // changes every occurrence of one color into another
  auto change_from_to_color = [](auto &board, int a, int b) {
    for (auto &c : board) {
      if (c == a) c = b;
    }
  };
  int row_color = 1;
  bool prev_in_bag = false;
  for (int y = 0; y < puzzle.height; y++) {
    row_color++;
    for (int x = 0; x < puzzle.width; x++) {
      if (puzzle.is_in_bag(x, y)) {
        puzzle_colors.get(x, y) = row_color;
        if (puzzle.is_in_bag(x, y - 1) &&
            (puzzle_colors.get(x, y - 1) != row_color))
          change_from_to_color(puzzle_colors.board, puzzle_colors.get(x, y - 1),
                               row_color);
        prev_in_bag = true;
      } else {
        if (prev_in_bag) row_color++;
        prev_in_bag = false;
      }
    }
  }
  std::set<int> distinct_bags;
  for (int x = 0; x < puzzle.width; x++) {
    for (int y = 0; y < puzzle.height; y++) {
      int c = puzzle_colors.get(x, y);
      if (c) distinct_bags.insert(c);
    }
  }
  if (show_debug) {
    std::cout << puzzle_colors << std::endl;
    std::cout << distinct_bags.size() << std::endl;
    for (auto c : distinct_bags) std::cout << c << " ";
    std::cout << std::endl;
    std::cout << puzzle << std::endl;
  }
  return (distinct_bags.size() - 1) * puzzle.width * puzzle.height * 10;
}

double evaluate(const puzzle_t &puzzle) {
  return puzzle.count_bad_summary_size() + puzzle.count_inconsistent();
}

bool puzzle_t::next_solution() {
  puzzle_t &puzzle = *this;
  int i = 0;
  for (; i < puzzle.board.size(); i++) {
    if (puzzle.board[i] == 0) {
      puzzle.board[i] = -1;
      break;
    } else if (puzzle.board[i] == -1) {
      puzzle.board[i] = 0;
    }
  }
  return (i != puzzle.board.size());
}

std::vector<puzzle_t> puzzle_t::generate_neighbors() const {
  const puzzle_t &p = *this;
  std::vector<puzzle_t> neighbors;
  for (int i = 0; i < p.board.size(); i++) {
    if (p.board[i] <= 0) {
      auto new_board = p;
      new_board.board[i] = -1 - new_board.board[i];
      neighbors.push_back(new_board);
    }
  }
  return neighbors;
}

puzzle_t puzzle_t::generate_random_solution() const {
  const puzzle_t &p = *this;
  using namespace std;
  static random_device rd;
  static mt19937 mt(rd());
  uniform_int_distribution<int> distr(-1, 0);
  puzzle_t rand_sol = p;
  for (int i = 0; i < p.board.size(); i++) {
    if (p.board[i] <= 0) {
      auto new_board = p;
      rand_sol.board[i] = distr(mt);
    }
  }
  return rand_sol;
}

puzzle_t hill_climb_det(const puzzle_t &puzzle, int iterations,
                        bool show_progress = false) {
  using namespace std;
  auto best_so_far = puzzle.generate_random_solution();
  for (int n = 0; n < iterations; n++) {
    if (show_progress) cout << n << " " << evaluate(best_so_far) << endl;
    auto neighbors = best_so_far.generate_neighbors();
    neighbors.push_back(best_so_far);
    best_so_far = *std::min_element(
        neighbors.begin(), neighbors.end(),
        [](auto l, auto r) { return evaluate(l) < evaluate(r); });
  }
  return best_so_far;
}

bool operator==(puzzle_t l, puzzle_t r) {
  if (l.width != r.width) return false;
  if (l.height != r.height) return false;
  for (unsigned i = 0; i < r.board.size(); i++) {
    if (l.board.at(i) != r.board.at(i)) return false;
  }
  return true;
}

puzzle_t tabu_search(puzzle_t puzzle, int iterations,
                     bool show_progress = false) {
  using namespace std;
  const int tabu_size = 1000;
  list<puzzle_t> tabu_list;
  tabu_list.push_back(puzzle.generate_random_solution());
  auto best_so_far = tabu_list.back();
  for (int n = 0; n < iterations; n++) {
    if (show_progress)
      cout << n << " " << evaluate(tabu_list.back()) << " "
           << evaluate(best_so_far) << endl;
    vector<puzzle_t> neighbors;
    for (auto e : tabu_list.back().generate_neighbors()) {
      bool found =
          (std::find(tabu_list.begin(), tabu_list.end(), e) != tabu_list.end());
      if (!found) neighbors.push_back(e);
    }
    if (neighbors.size() == 0) {
      cerr << "we ate our tail :/" << endl;
      break;
    }
    tabu_list.push_back(*std::min_element(
        neighbors.begin(), neighbors.end(),
        [](auto l, auto r) { return evaluate(l) < evaluate(r); }));
    if (evaluate(tabu_list.back()) <= evaluate(best_so_far)) {
      best_so_far = tabu_list.back();
    }
    if (tabu_list.size() > tabu_size) tabu_list.pop_front();
  }
  return best_so_far;
}

puzzle_t brute_force(puzzle_t puzzle, int iterations,
                     bool show_progress = false) {
  using namespace std;
  int n = 0;
  auto best_so_far = puzzle;
  while (puzzle.next_solution()) {
    if (show_progress) cout << n << " " << evaluate(best_so_far) << endl;
    if (n >= iterations) {
      cerr << "broken brute-force " << endl;
      break;
    }
    if (evaluate(puzzle) == 0) {
      best_so_far = puzzle;
      cout << "Found at iteration " << n << endl << puzzle << endl;
      break;
    } else if (evaluate(puzzle) < evaluate(best_so_far)) {
      best_so_far = puzzle;
    }
    n++;
  }
  return best_so_far;
}

puzzle_t random_probe(puzzle_t puzzle, int iterations,
                      bool show_progress = false) {
  using namespace std;
  auto best_so_far = puzzle;
  for (int n = 0; n < iterations; n++) {
    if (show_progress) cout << n << " " << evaluate(best_so_far) << endl;
    puzzle = puzzle.generate_random_solution();
    if (evaluate(puzzle) <= evaluate(best_so_far)) {
      best_so_far = puzzle;
    }
  }
  return best_so_far;
}

int main(int argc, char **argv) {
  using namespace std;
  puzzle_t puzzle0 = {4, 4, {0, 0, 0, 3, 0, 4, -1, 0, 5, 0, 6, 0, 0, 0, 0, 2}};
  puzzle_t puzzle1 = {7, 7, {0, 0, 0,  0, 0, 0, 3, 2, 0, 3, 0, 0, 0, 0, 0, 0, 0,
                             0, 7, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 2, 0, 0, 0,
                             0, 0, 0,  0, 0, 3, 0, 6, 4, 0, 0, 0, 0, 0, 0}};
  auto puzzle = puzzle1;

  //    cout << puzzle << endl;
  // string method = "brute_force";
  // string method = "tabu_search";
  // string method = "random_probe";
  // string method = "hill_climb_det";
  string method = argv[1];
  int iterations = stoi(argv[2]);
  bool do_chart = std::string(argv[3]) == "true";
  bool print_input = std::string(argv[4]) == "true";
  bool print_result = std::string(argv[5]) == "true";
  bool print_result_eval = std::string(argv[6]) == "true";
  map<string, function<puzzle_t(puzzle_t, int, bool)>> methods = {
      {"brute_force", brute_force},
      {"random_probe", random_probe},
      {"hill_climb_det", hill_climb_det},
      {"tabu_search", tabu_search}};

  if (print_input) cout << puzzle << endl;
  puzzle_t result = methods.at(method)(puzzle, iterations, do_chart);
  if (print_result) cout << result << endl;
  if (print_result_eval) cout << evaluate(result) << endl;

  return 0;
}
