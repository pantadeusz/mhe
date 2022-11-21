#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <iomanip>
#include <algorithm>
#include <map>
#include <list>
#include <functional>

struct puzzle_t {
    int width;
    int height;
    std::vector<int> board;

    int &get(const int x, const int y) {
        return board[y * width + x];
    }

    int get(const int x, const int y) const {
        if ((x < 0) || (x >= width) || (y < 0) || (y >= height)) throw std::invalid_argument("out of board");
        return board.at(y * width + x);
    }

    /**
     * generates new random solution based on this problem definition
     * @return new random solution that is based on the problem definition
     */
    puzzle_t generate_random_solution() const;

    std::vector<puzzle_t> generate_neighbours() const;

    bool next_solution();
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
            if (val > 0) o << setw(4) << val;
            else if (val == 0) o << "    ";
            else o << " .. ";
            o << "|";
        }
        o << endl;
    }
    return o;
}

int count_inside_bag_from_point(int x, int y, const puzzle_t &puzzle) {
    using namespace std;
    vector<pair<int, int>> directions = {{-1, 0},
                                         {1,  0},
                                         {0,  -1},
                                         {0,  1}};
    int sum = 1;
    for (auto [dx, dy]: directions) {
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

int count_inconsistent(const puzzle_t &puzzle) {
    int error = 0;
    for (int x = 0; x < puzzle.width; x++)
        for (int y = 0; y < puzzle.height; y++) {
            if (puzzle.get(x, y) > 0) {
                int count = count_inside_bag_from_point(x, y, puzzle);
                error += std::abs(count - puzzle.get(x, y));
            }
        }
    return error;
}

double evaluate(const puzzle_t &puzzle) {
    return count_inconsistent(puzzle);
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

std::vector<puzzle_t> puzzle_t::generate_neighbours() const {
    const puzzle_t &p = *this;
    std::vector<puzzle_t> neighbours;
    for (int i = 0; i < p.board.size(); i++) {
        if (p.board[i] <= 0) {
            auto new_board = p;
            new_board.board[i] = -1 - new_board.board[i];
            neighbours.push_back(new_board);
        }
    }
    return neighbours;
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

puzzle_t hill_climb_det(puzzle_t puzzle, int iterations, bool show_progress = false) {
    using namespace std;
    auto best_so_far = puzzle.generate_random_solution();
    for (int n = 0; n < iterations; n++) {
        if (show_progress) cout << n << " " << evaluate(best_so_far) << endl;
        auto neighbours = puzzle.generate_neighbours();
        neighbours.push_back(best_so_far);
        best_so_far = *std::min_element(neighbours.begin(), neighbours.end(),
                    [](auto l, auto r) { return evaluate(l) < evaluate(r); });
    }
    return best_so_far;
}

bool operator==(puzzle_t l, puzzle_t r) {
    if (l.width != r.width) return false;
    if (l.height!= r.height) return false;
    for (unsigned i = 0; i < r.board.size(); i++) {
        if (l.board.at(i) != r.board.at(i)) return false;
    }
    return true;
}

puzzle_t tabu_search(puzzle_t puzzle, int iterations, bool show_progress = false) {
    using namespace std;
    const int tabu_size = 1000;
    list<puzzle_t> tabu_list;
    tabu_list.push_back(puzzle.generate_random_solution());
    auto best_so_far = tabu_list.back();
    for (int n = 0; n < iterations; n++) {
        if (show_progress) cout << n << " " << evaluate(tabu_list.back()) << " " << evaluate(best_so_far) << endl;
        vector<puzzle_t> neighbours;
        for (auto e : tabu_list.back().generate_neighbours()) {
            bool found = (std::find(tabu_list.begin(), tabu_list.end(), e) != tabu_list.end());
            if (!found) neighbours.push_back(e);
        }
        if (neighbours.size() == 0) {
            cerr << "we ate our tail :/" << endl;
            break;
        }
        tabu_list.push_back(*std::min_element(neighbours.begin(), neighbours.end(),
                    [](auto l, auto r) { return evaluate(l) < evaluate(r); }));
        if (evaluate(tabu_list.back()) <= evaluate(best_so_far)) {
            best_so_far = tabu_list.back();
        }
        if (tabu_list.size() > tabu_size) tabu_list.pop_front();
    }
    return best_so_far;
}

puzzle_t brute_force(puzzle_t puzzle, int iterations, bool show_progress = false) {
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

puzzle_t random_probe(puzzle_t puzzle, int iterations, bool show_progress = false) {
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
    puzzle_t puzzle0 = {
            4,
            4,
            {
                    0, 0, 0, 3,
                    0, 4, -1, 0,
                    5, 0, 6, 0,
                    0, 0, 0, 2}
    };
    puzzle_t puzzle1 = {
            7,
            7,
            {
                    0, 0, 0, 0, 0, 0, 3,
                    2, 0, 3, 0, 0, 0, 0,
                    0, 0, 0, 0, 7, 13, 0,
                    0, 0, 0, 0, 0, 0, 0,
                    0, 6, 2, 0, 0, 0, 0,
                    0, 0, 0, 0, 3, 0, 6,
                    4, 0, 0, 0, 0, 0, 0}
    };
    auto puzzle = puzzle1;
//    cout << puzzle << endl;
    //string method = "brute_force";
    //string method = "tabu_search";
    //string method = "random_probe";
    //string method = "hill_climb_det";
    string method = argv[1];
    int iterations = stoi(argv[2]);
    bool do_chart = std::string(argv[3]) == "true"; 
    bool print_input = std::string(argv[4]) == "true";
    bool print_result = std::string(argv[5]) == "true";
    bool print_result_eval = std::string(argv[6]) == "true";
    map<string,function<puzzle_t(puzzle_t,int,bool)>> methods = {
            {"brute_force", brute_force},
            {"random_probe", random_probe},
            {"hill_climb_det", hill_climb_det},
            {"tabu_search", tabu_search}
    };

    
    if (print_input) cout << puzzle << endl;
    puzzle_t result = methods.at(method)(puzzle,iterations, do_chart);
    if (print_result) cout << result << endl;
    if (print_result_eval) cout << evaluate(result) << endl;
    return 0;
}
