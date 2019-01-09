#include "EngineVersion17.h"
#include "EngineVersion18.h"
#include "EngineVersion19.h"
#include "EngineVersion20.h"
#include "EngineVersion21.h"
#include "EngineVersion22.h"
#include "EngineVersion23.h"
#include "EngineVersion24.h"
#include "tool.h"
#include "CommonNonClassData.h"
#include <iostream>
#include <vector>
#include <time.h>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <random>
#include <stdexcept>
#include <algorithm>

using namespace std;

using namespace std::chrono;

struct RunningData
{
    int num_EngineOne_wins;

    int num_EngineTwo_wins;

    int num_draws;

    duration<double> total_time_EngineOne_has_thought;

    duration<double> total_time_EngineTwo_has_thought;

    int num_instances_EngineOne_thought; // Stores the number of times EngineOne thought for the normal 0.30s.

    int num_instances_EngineTwo_thought; // Stores the number of times EngineTwo thought for the normal 0.30s.

    int num_level_trials; // stores the number of trials where the score was 1-1.

    int num_trials_favouring_EngineOne; // stores the number of trials where EngineOne scored 2-0 or 1.5-0.5

    int num_trials_favouring_EngineTwo; // stores the number of trials where EngineTwo scored 2-0 or 1.5-0.5

    duration<double> time_EngineOne_spent_in_find_best_move;

    duration<double> time_EngineTwo_spent_in_find_best_move;

    int num_instances_EngineOne_used_find_best_move;

    int num_instances_EngineTwo_used_find_best_move;

    double average_number_moves_in_games_EngineOne_won;

    double average_number_moves_in_games_EngineTwo_won;

    double average_number_moves_in_games_drawn;

    duration<double> max_time_EngineOne_spent_at_once_in_find_best_move;

    duration<double> max_time_EngineTwo_spent_at_once_in_find_best_move;
};

struct Engine_numbers_for_a_match
{
    int first_Engine_version;

    int second_Engine_version;
};

const int max_row_index = 5;
const int max_col_index = 6;

bool compare_sets_of_starting_moves(const vector<coordinate>& first_set, const vector<coordinate>& second_set)
{
    if (first_set.size() < second_set.size())
    {
        return true;
    }

    else if (first_set.size() > second_set.size())
    {
        return false;
    }

    for (int i = 0; i < first_set.size(); i++)
    {
        if (first_set[i].row < second_set[i].row)
        {
            return true;
        }

        else if (first_set[i].row > second_set[i].row)
        {
            return false;
        }

        else if (first_set[i].col < second_set[i].col)
        {
            return true;
        }

        else if (first_set[i].col > second_set[i].col)
        {
            return false;
        }
    }

    return false;
}

void remove_duplicate_sets(vector<vector<coordinate>>& sets)
{
    vector<vector<coordinate>> replacement;

    for (int i = 0; i < sets.size(); i++)
    {
        if (i == 0 || sets[i] != sets[i-1])
        {
            replacement.push_back(sets[i]);
        }
    }

    sets = replacement;
}

vector<vector<coordinate>> record_all_sets_of_possible_starting_moves(int max_num_moves_to_make)
{
    vector<vector<coordinate>> return_vec;

    vector<coordinate> moves;

    for (int c = 0; c <= max_col_index; c++)
    {
        coordinate temp;
        temp.row = max_row_index;
        temp.col = c;
        moves.push_back(temp);
    }

    for (int i = 1; i <= 1000000; i++)
    {
        vector<coordinate> copy_moves = moves;

        vector<coordinate> current_set; // Will be added to return_vec

        int num_pieces = rand() % max_num_moves_to_make + 1;

        for (int x = 1; x <= num_pieces; x++)
        {
            int rand_index = rand() % copy_moves.size();

            current_set.push_back(copy_moves[rand_index]);

            copy_moves[rand_index].row--;

            if (copy_moves[rand_index].row == -1)
            {
                copy_moves.erase(copy_moves.begin() + rand_index);
            }
        }

        return_vec.push_back(current_set);
    }

    cout << return_vec.size() << "\n";

    sort(return_vec.begin(), return_vec.end(), compare_sets_of_starting_moves);

    remove_duplicate_sets(return_vec);

    random_shuffle(return_vec.begin(), return_vec.end());

    return return_vec;
}

void make_it_fair_for_smaller_sets(vector<vector<coordinate>>& sets, int largest_size_of_a_set)
{
    // There are far fewer smaller sets than larger sets. Make it so all smaller sets get presented
    // in equal number as all other sets, until that smaller set is completely represented (nothing remaining).
    // All sets range in size from 1 - 8 elements.

    vector<vector<vector<coordinate>>> sets_separated_by_their_sizes;

    for (int i = 1; i <= largest_size_of_a_set; i++)
    {
        vector<vector<coordinate>> temp;
        sets_separated_by_their_sizes.push_back(temp);
    }

    for (const vector<coordinate>& current_set: sets)
    {
        sets_separated_by_their_sizes[current_set.size()-1].push_back(current_set);
    }

    // Randomize the order of each group of sets in sets_separated_by_their_sizes:

    for (vector<vector<coordinate>>& current_group: sets_separated_by_their_sizes)
    {
        random_shuffle(current_group.begin(), current_group.end());
    }

    vector<vector<coordinate>> replacement; // sets parameter will be set equal to it.

    int current_index = 0;

    while (sets_separated_by_their_sizes.size() > 0)
    {
        replacement.push_back(sets_separated_by_their_sizes[current_index].back());

        sets_separated_by_their_sizes[current_index].pop_back();

        if (sets_separated_by_their_sizes[current_index].size() == 0)
        {
            sets_separated_by_their_sizes.erase(sets_separated_by_their_sizes.begin() + current_index);
        }

        current_index ++;

        if (current_index >= sets_separated_by_their_sizes.size())
        {
            current_index = 0;
        }
    }

    sets = replacement;
}

void initialize_empty_board(vector<vector<char>>& board)
{
    vector<char> row;

    for (int i = 0; i <= max_col_index; i++)
    {
        row.push_back(' ');
    }

    for (int i = 0; i <= max_row_index; i++)
    {
        board.push_back(row);
    }
}

void display_board(const vector<vector<char>>& board)
{
    cout << "\n    A   B   C   D   E   F   G\n\n";

    for (int row = 0; row <= 5; row++)
    {
        cout << (6 - row) << " | "; // Since I want row numbers to be displayed increasing from bottom up, not top down.

        for (int col = 0; col <= 6; col++)
        {
            cout << board[row][col] << " | ";
        }

        cout << "\n" << "  |---|---|---|---|---|---|---|\n";
    }
}

void play_starting_moves_on_to_board(vector<vector<char>>& board, const vector<coordinate>& starting_moves)
{
    char current_char = 'C';

    for (const coordinate& current_move: starting_moves)
    {
        board[current_move.row][current_move.col] = current_char;

        if (current_char == 'C')
        {
            current_char = 'U';
        }

        else
        {
            current_char = 'C';
        }
    }
}

void print_board_with_starting_moves(const vector<coordinate>& starting_moves) // Function is for testing.
{
    vector<vector<char>> board;

    initialize_empty_board(board);

    play_starting_moves_on_to_board(board, starting_moves);

    display_board(board);
}

void point_to_correct_derived_class(unique_ptr<tool>& pt, int version_number_of_Engine)
{
    double old_thinking_time = 0.30; // just to be safe, this variable doesn't need to even be given a value yet.

    if (version_number_of_Engine == 17)
    {
        old_thinking_time = EngineVersion17::position::thinking_time;

        EngineVersion17::position::thinking_time = 0;

        pt = EngineVersion17::position::think_on_game_position(true, true);
    }

    else if (version_number_of_Engine == 18)
    {
        old_thinking_time = EngineVersion18::position::thinking_time;

        EngineVersion18::position::thinking_time = 0;

        pt = EngineVersion18::position::think_on_game_position(true, true);
    }

    else if (version_number_of_Engine == 19)
    {
        old_thinking_time = EngineVersion19::position::thinking_time;

        EngineVersion19::position::thinking_time = 0;

        pt = EngineVersion19::position::think_on_game_position(true, true);
    }

    else if (version_number_of_Engine == 20)
    {
        old_thinking_time = EngineVersion20::position::thinking_time;

        EngineVersion20::position::thinking_time = 0;

        pt = EngineVersion20::position::think_on_game_position(true, true);
    }

    else if (version_number_of_Engine == 21)
    {
        old_thinking_time = EngineVersion21::position::thinking_time;

        EngineVersion21::position::thinking_time = 0;

        pt = EngineVersion21::position::think_on_game_position(true, true);
    }

    else if (version_number_of_Engine == 22)
    {
        old_thinking_time = EngineVersion22::position::thinking_time;

        EngineVersion22::position::thinking_time = 0;

        pt = EngineVersion22::position::think_on_game_position(true, true);
    }

    else if (version_number_of_Engine == 23)
    {
        old_thinking_time = EngineVersion23::position::thinking_time;

        EngineVersion23::position::thinking_time = 0;

        pt = EngineVersion23::position::think_on_game_position(true, true);
    }

    // Continue writing else if statements here for future Versions added.......

    else
    {
        throw runtime_error("Version doesn't exist in this Simulation!\n");
    }

    pt->set_static_thinking_time(old_thinking_time);
}

void get_Engine_primed_to_play(const vector<coordinate>& starting_moves, unique_ptr<tool>& pt,
                                bool is_engine_moving_first_in_destination_position, int version_number_of_Engine)
{
    // Function gets pt (passed by reference) to the position reached after starting_moves.

    // On the way to this position, pt will not be given time to think deeply (will only be able to think to depth 1).
    // pt will only think deeply (at normal thinking_time of 0.30) when it reaches the destination position.

    point_to_correct_derived_class(pt, version_number_of_Engine);
    // Gets base class pointer pt to point to the right position class by calling a think_ function with 0 passed in
    // for temporary thinking_time.

    double old_thinking_time = pt->get_static_thinking_time();

    pt->set_static_thinking_time(0);

    vector<vector<char>> assisting_board; // the moves will be played on this board and sent to pt's position class.

    initialize_empty_board(assisting_board);

    char current_char_to_be_placed = 'U';

    if ((starting_moves.size() % 2 == 0 && is_engine_moving_first_in_destination_position) ||
        (starting_moves.size() % 2 != 0 && !is_engine_moving_first_in_destination_position))
    {
        current_char_to_be_placed = 'C';
    }

    pt = pt->call_static_think_on_game_position(current_char_to_be_placed == 'C', true);

    for (int i = 0; i < starting_moves.size(); i++)
    {
        assisting_board[starting_moves[i].row][starting_moves[i].col] = current_char_to_be_placed;

        if (current_char_to_be_placed == 'C')
        {
            current_char_to_be_placed = 'U';
        }

        else
        {
            current_char_to_be_placed = 'C';
        }

        if (i == starting_moves.size() - 1)
        {
            // Final move - think at normal duration:

            pt->set_static_thinking_time(old_thinking_time);
        }

        pt = pt->call_static_think_on_game_position(assisting_board, current_char_to_be_placed == 'C',
                                                    starting_moves[i], pt->get_squares_amplifying_comp_2(),
                                                    pt->get_squares_amplifying_comp_3(), pt->get_squares_amplifying_user_2(),
                                                    pt->get_squares_amplifying_user_3(), true);
    }

    if (pt->get_static_thinking_time() != old_thinking_time)
    {
        throw runtime_error("EngineOne's thinking_time was not set back to normal.\n");
    }
}

bool fair(const vector<coordinate>& starting_moves, int version_EngineOne, int version_EngineTwo)
{
    // Returns true if both Engines agree that the passed in set of starting moves leads to a fair position.
    // I.e., > -20 and < 20

    unique_ptr<tool> pt_EngineOne;
    get_Engine_primed_to_play(starting_moves, pt_EngineOne, true, version_EngineOne);

    unique_ptr<tool> pt_EngineTwo;
    get_Engine_primed_to_play(starting_moves, pt_EngineTwo, true, version_EngineTwo);

    if (pt_EngineOne->get_board() != pt_EngineTwo->get_board())
    {
        throw runtime_error("boards not equal in fair()\n");
    }

    if (pt_EngineOne->get_evaluation() > -20 && pt_EngineOne->get_evaluation() < 20 &&
        pt_EngineTwo->get_evaluation() > -20 && pt_EngineTwo->get_evaluation() < 20)
    {
        display_board(pt_EngineOne->get_board());

        cout << "EngineOne's evaluation: " << pt_EngineOne->get_evaluation() << "\n";

        cout << "EngineTwo's evaluation: " << pt_EngineTwo->get_evaluation() << "\n";

        return true;
    }

    return false;
}

void flip_board(vector<vector<char>>& board)
{
    for (int row = 0; row <= max_row_index; row++)
    {
        for (int col = 0; col <= max_col_index; col++)
        {
            if (board[row][col] == 'C')
            {
                board[row][col] = 'U';
            }

            else if (board[row][col] == 'U')
            {
                board[row][col] = 'C';
            }
        }
    }
}

void playout_game(RunningData& simulation_stats, const vector<coordinate>& starting_moves, bool EngineOne_moves_first,
                  int version_EngineOne, int version_EngineTwo)
{
    unique_ptr<tool> pt_EngineOne;
    get_Engine_primed_to_play(starting_moves, pt_EngineOne, EngineOne_moves_first, version_EngineOne);

    unique_ptr<tool> pt_EngineTwo;
    get_Engine_primed_to_play(starting_moves, pt_EngineTwo, !EngineOne_moves_first, version_EngineTwo);

    vector<vector<char>> assisting_board;

    if (EngineOne_moves_first) // Then EngineOne's board is ready for EngineOne to place a 'C' on it on the next turn:
    {
        assisting_board = pt_EngineOne->get_board();
    }

    else
    {
        assisting_board = pt_EngineTwo->get_board();
    }

    const double old_thinking_time_EngineOne = pt_EngineOne->get_static_thinking_time();

    const double old_thinking_time_EngineTwo = pt_EngineTwo->get_static_thinking_time();

    if (old_thinking_time_EngineOne != 0.30 || old_thinking_time_EngineTwo != 0.30)
    {
        throw runtime_error("Standard thinking time isn't 0.30 for one of the Engines.\n");
    }

    bool EngineOne_turn = EngineOne_moves_first;

    int num_moves_played = 0; // Will store the number of moves played in the game.

    while (!pt_EngineOne->did_computer_win() && !pt_EngineOne->did_opponent_win() && !pt_EngineOne->is_game_drawn() &&
           !pt_EngineTwo->did_computer_win() && !pt_EngineTwo->did_opponent_win() && !pt_EngineTwo->is_game_drawn())
    {
        if (EngineOne_turn)
        {
            steady_clock::time_point start_time = steady_clock::now();

            coordinate chosen_move = pt_EngineOne->find_best_move_for_comp();

            duration<double> time_just_spent = duration_cast<duration<double>>(steady_clock::now() - start_time);

            simulation_stats.time_EngineOne_spent_in_find_best_move += time_just_spent;

            if (time_just_spent.count() > simulation_stats.max_time_EngineOne_spent_at_once_in_find_best_move.count())
            {
                simulation_stats.max_time_EngineOne_spent_at_once_in_find_best_move = time_just_spent;
            }

            simulation_stats.num_instances_EngineOne_used_find_best_move ++;

            assisting_board[chosen_move.row][chosen_move.col] = 'C';

            num_moves_played ++;

            // Now to get BOTH EngineOne and EngineTwo to think on this new position. Since EngineOne has made its
            // move and its turn is over, I'll make its thinking_time = 0 temporarily. This will allow it to
            // update its amplifying vectors, while only being able to think at depth 1 (which doesn't boost its TT's strength).

            pt_EngineOne->set_static_thinking_time(0);

            pt_EngineOne = pt_EngineOne->call_static_think_on_game_position(assisting_board, false, chosen_move,
                                                                            pt_EngineOne->get_squares_amplifying_comp_2(),
                                                                            pt_EngineOne->get_squares_amplifying_comp_3(),
                                                                            pt_EngineOne->get_squares_amplifying_user_2(),
                                                                            pt_EngineOne->get_squares_amplifying_user_3(), false);

            pt_EngineOne->set_static_thinking_time(old_thinking_time_EngineOne);

            flip_board(assisting_board);

            start_time = steady_clock::now();

            pt_EngineTwo = pt_EngineTwo->call_static_think_on_game_position(assisting_board, true, chosen_move,
                                                                            pt_EngineTwo->get_squares_amplifying_comp_2(),
                                                                            pt_EngineTwo->get_squares_amplifying_comp_3(),
                                                                            pt_EngineTwo->get_squares_amplifying_user_2(),
                                                                            pt_EngineTwo->get_squares_amplifying_user_3(), false);

            simulation_stats.total_time_EngineTwo_has_thought += duration_cast<duration<double>>(steady_clock::now() - start_time);

            simulation_stats.num_instances_EngineTwo_thought ++;
        }

        else // EngineTwo's turn:
        {
            steady_clock::time_point start_time = steady_clock::now();

            coordinate chosen_move = pt_EngineTwo->find_best_move_for_comp();

            duration<double> time_just_spent = duration_cast<duration<double>>(steady_clock::now() - start_time);

            simulation_stats.time_EngineTwo_spent_in_find_best_move += time_just_spent;

            if (time_just_spent.count() > simulation_stats.max_time_EngineTwo_spent_at_once_in_find_best_move.count())
            {
                simulation_stats.max_time_EngineTwo_spent_at_once_in_find_best_move = time_just_spent;
            }

            simulation_stats.num_instances_EngineTwo_used_find_best_move ++;

            assisting_board[chosen_move.row][chosen_move.col] = 'C';

            num_moves_played ++;

            // Now to get BOTH EngineOne and EngineTwo to think on this new position. Since EngineTwo has made its
            // move and its turn is over, I'll make its thinking_time = 0 temporarily. This will allow it to
            // update its amplifying vectors, while only being able to search at depth 1 (not boosting its TT's strength at all).

            pt_EngineTwo->set_static_thinking_time(0);

            pt_EngineTwo = pt_EngineTwo->call_static_think_on_game_position(assisting_board, false, chosen_move,
                                                                            pt_EngineTwo->get_squares_amplifying_comp_2(),
                                                                            pt_EngineTwo->get_squares_amplifying_comp_3(),
                                                                            pt_EngineTwo->get_squares_amplifying_user_2(),
                                                                            pt_EngineTwo->get_squares_amplifying_user_3(), false);

            pt_EngineTwo->set_static_thinking_time(old_thinking_time_EngineTwo);

            flip_board(assisting_board);

            start_time = steady_clock::now();

            pt_EngineOne = pt_EngineOne->call_static_think_on_game_position(assisting_board, true, chosen_move,
                                                                            pt_EngineOne->get_squares_amplifying_comp_2(),
                                                                            pt_EngineOne->get_squares_amplifying_comp_3(),
                                                                            pt_EngineOne->get_squares_amplifying_user_2(),
                                                                            pt_EngineOne->get_squares_amplifying_user_3(), false);

            simulation_stats.total_time_EngineOne_has_thought += duration_cast<duration<double>>(steady_clock::now() - start_time);

            simulation_stats.num_instances_EngineOne_thought ++;
        }

        EngineOne_turn = !EngineOne_turn;
    }

    // Now see what result to record, and make sure the Engines agree on this result.

    if (pt_EngineOne->did_computer_win() && pt_EngineTwo->did_opponent_win())
    {
        simulation_stats.num_EngineOne_wins ++;

        simulation_stats.average_number_moves_in_games_EngineOne_won += num_moves_played;

        cout << "EngineOne won.\n";
    }

    else if (pt_EngineOne->did_opponent_win() && pt_EngineTwo->did_computer_win())
    {
        simulation_stats.num_EngineTwo_wins ++;

        simulation_stats.average_number_moves_in_games_EngineTwo_won += num_moves_played;

        cout << "EngineTwo won.\n";
    }

    else if (pt_EngineOne->is_game_drawn() && pt_EngineTwo->is_game_drawn())
    {
        simulation_stats.num_draws ++;

        simulation_stats.average_number_moves_in_games_drawn += num_moves_played;

        cout << "The game is drawn.\n";
    }

    else
    {
        throw runtime_error("Engines couldn't agree on a result for the game they played.\n");
    }
}

int main()
{
    // Ready to use tool pointers, with all methods in position classes ready for accessing static methods too!

    srand(time(NULL));

    ofstream fout("Simulation_Data.txt");

    if (fout.fail())
    {
        throw runtime_error("fout failed to make a file.\n");
    }

    vector<vector<coordinate>> all_starting_sets_of_moves = record_all_sets_of_possible_starting_moves(8);

    cout << all_starting_sets_of_moves.size() << " different starting sets of moves.\n";

    make_it_fair_for_smaller_sets(all_starting_sets_of_moves, 8);

    int num_trials = 0;

    cout << "Enter the number of trials/starting positions to play out in each \"mini-match\": ";

    cin >> num_trials;

    vector<Engine_numbers_for_a_match> matchups; // Each element stores the version numbers (as ints) for the two engines to
                                                 // play in a mini-match.

    for (int i = 1; ; i++)
    {
        Engine_numbers_for_a_match temp;

        cout << "Enter the first Engine's version for mini-match #" << i << ", or enter a negative number to stop: ";

        cin >> temp.first_Engine_version;

        if (temp.first_Engine_version < 0)
        {
            break;
        }

        cout << "Enter the second Engine's version for mini-match #" << i << ": ";

        cin >> temp.second_Engine_version;

        matchups.push_back(temp);
    }

    for (const Engine_numbers_for_a_match& current_match_players: matchups)
    {
        RunningData simulation_stats;
        simulation_stats.num_draws = 0;
        simulation_stats.num_EngineOne_wins = 0;
        simulation_stats.num_EngineTwo_wins = 0;
        simulation_stats.total_time_EngineOne_has_thought = duration_cast<duration<double>>(steady_clock::now() - steady_clock::now());
        simulation_stats.total_time_EngineTwo_has_thought = duration_cast<duration<double>>(steady_clock::now() - steady_clock::now());
        simulation_stats.num_instances_EngineOne_thought = 0;
        simulation_stats.num_instances_EngineTwo_thought = 0;
        simulation_stats.num_level_trials = 0;
        simulation_stats.num_trials_favouring_EngineOne = 0;
        simulation_stats.num_trials_favouring_EngineTwo = 0;
        simulation_stats.time_EngineOne_spent_in_find_best_move = duration_cast<duration<double>>(steady_clock::now() - steady_clock::now());
        simulation_stats.time_EngineTwo_spent_in_find_best_move = duration_cast<duration<double>>(steady_clock::now() - steady_clock::now());
        simulation_stats.num_instances_EngineOne_used_find_best_move = 0;
        simulation_stats.num_instances_EngineTwo_used_find_best_move = 0;
        simulation_stats.average_number_moves_in_games_EngineOne_won = 0.0;
        simulation_stats.average_number_moves_in_games_EngineTwo_won = 0.0;
        simulation_stats.average_number_moves_in_games_drawn = 0.0;
        simulation_stats.max_time_EngineOne_spent_at_once_in_find_best_move = duration_cast<duration<double>>(steady_clock::now() - steady_clock::now());
        simulation_stats.max_time_EngineTwo_spent_at_once_in_find_best_move = duration_cast<duration<double>>(steady_clock::now() - steady_clock::now());

        int current_index_in_sets = 0; // Stores the index of the current set being examined in all_starting_sets_of_moves.

        for (int i = 1; i <= num_trials; i++)
        {
            while (!fair(all_starting_sets_of_moves[current_index_in_sets], current_match_players.first_Engine_version,
                         current_match_players.second_Engine_version))
            {
                current_index_in_sets ++;

                if (current_index_in_sets >= all_starting_sets_of_moves.size())
                {
                    throw runtime_error("Ran out of starting positions!\n");
                }
            }

            // Get the Engines to play out starting_sets_of_moves[current_index_in_sets],
            // as both sides (2 games).

            RunningData old_stats = simulation_stats;

            playout_game(simulation_stats, all_starting_sets_of_moves[current_index_in_sets], i % 2 == 0,
                         current_match_players.first_Engine_version, current_match_players.second_Engine_version);

            playout_game(simulation_stats, all_starting_sets_of_moves[current_index_in_sets], i % 2 != 0,
                         current_match_players.first_Engine_version, current_match_players.second_Engine_version);

            cout << "Up to this point, Version " << current_match_players.first_Engine_version <<" has ";
            cout << simulation_stats.num_EngineOne_wins << " wins, ";
            cout << "Version " << current_match_players.second_Engine_version << " has ";
            cout << simulation_stats.num_EngineTwo_wins << " wins, and ";
            cout << "there have been " << simulation_stats.num_draws << " draws.\n\n";

            double thinking_rate_EngineOne = simulation_stats.total_time_EngineOne_has_thought.count();

            thinking_rate_EngineOne /= static_cast<double>(simulation_stats.num_instances_EngineOne_thought);

            cout << "Version " << current_match_players.first_Engine_version << " is thinking at an average rate of ";
            cout << thinking_rate_EngineOne << " seconds/move.\n";

            double thinking_rate_EngineTwo = simulation_stats.total_time_EngineTwo_has_thought.count();

            thinking_rate_EngineTwo /= static_cast<double>(simulation_stats.num_instances_EngineTwo_thought);

            cout << "Version " << current_match_players.second_Engine_version << " is thinking at an average rate of ";
            cout << thinking_rate_EngineTwo << " seconds/move.\n";

            double best_move_function_EngineOne_thinking_rate = simulation_stats.time_EngineOne_spent_in_find_best_move.count();

            best_move_function_EngineOne_thinking_rate /= static_cast<double>(simulation_stats.num_instances_EngineOne_used_find_best_move);

            cout << "In the find_best_move() function, ";
            cout << "Version " << current_match_players.first_Engine_version << " is thinking at an average rate of ";
            cout << best_move_function_EngineOne_thinking_rate << " seconds/move.\n";
            cout << "The total time, up until now, spent thinking in find_best_move() is ";
            cout << simulation_stats.time_EngineOne_spent_in_find_best_move.count() << " seconds.\n";
            cout << "The max amount of time spent thinking in ONE call of find_best_move() is ";
            cout << simulation_stats.max_time_EngineOne_spent_at_once_in_find_best_move.count() << "\n\n";

            double best_move_function_EngineTwo_thinking_rate = simulation_stats.time_EngineTwo_spent_in_find_best_move.count();

            best_move_function_EngineTwo_thinking_rate /= static_cast<double>(simulation_stats.num_instances_EngineTwo_used_find_best_move);

            cout << "In the find_best_move() function, ";
            cout << "Version " << current_match_players.second_Engine_version << " is thinking at an average rate of ";
            cout << best_move_function_EngineTwo_thinking_rate << " seconds/move.\n";
            cout << "The total time, up until now, spent thinking in find_best_move() is ";
            cout << simulation_stats.time_EngineTwo_spent_in_find_best_move.count() << " seconds.\n";
            cout << "The max amount of time spent thinking in ONE call of find_best_move() is ";
            cout << simulation_stats.max_time_EngineTwo_spent_at_once_in_find_best_move.count() << "\n";

            int num_EngineOne_wins_from_trial = simulation_stats.num_EngineOne_wins - old_stats.num_EngineOne_wins;
            int num_EngineTwo_wins_from_trial = simulation_stats.num_EngineTwo_wins - old_stats.num_EngineTwo_wins;
            int indicator = num_EngineOne_wins_from_trial - num_EngineTwo_wins_from_trial;

            if (indicator > 0)
            {
                simulation_stats.num_trials_favouring_EngineOne ++;
            }

            else if (indicator < 0)
            {
                simulation_stats.num_trials_favouring_EngineTwo ++;
            }

            else
            {
                simulation_stats.num_level_trials ++;
            }

            current_index_in_sets ++;
        }

        simulation_stats.average_number_moves_in_games_EngineOne_won /= static_cast<double>(simulation_stats.num_EngineOne_wins);

        simulation_stats.average_number_moves_in_games_EngineTwo_won /= static_cast<double>(simulation_stats.num_EngineTwo_wins);

        simulation_stats.average_number_moves_in_games_drawn /= static_cast<double>(simulation_stats.num_draws);

        fout << "\n\n\n\nResults of the match between Version " << current_match_players.first_Engine_version;
        fout << " and Version " << current_match_players.second_Engine_version << ":\n\n";

        fout << "Version " << current_match_players.first_Engine_version << " won ";
        fout << simulation_stats.num_EngineOne_wins << " games.\n";

        fout << "Version " << current_match_players.second_Engine_version << " won ";
        fout << simulation_stats.num_EngineTwo_wins << " games.\n";

        fout << "There were " << simulation_stats.num_draws << " draws.\n";

        fout << "There were " << simulation_stats.num_trials_favouring_EngineOne << " trials where Version ";
        fout << current_match_players.first_Engine_version << " scored 2-0 or 1.5-0.5\n";

        fout << "There were " << simulation_stats.num_trials_favouring_EngineTwo << " trials where Version ";
        fout << current_match_players.second_Engine_version << " scored 2-0 or 1.5-0.5\n";

        fout << "There were " << simulation_stats.num_level_trials << " trials where the score was equal (1-1)\n";

        double thinking_rate_EngineOne = simulation_stats.total_time_EngineOne_has_thought.count();

        thinking_rate_EngineOne /= static_cast<double>(simulation_stats.num_instances_EngineOne_thought);

        fout << "Version " << current_match_players.first_Engine_version << " spent ";
        fout << simulation_stats.total_time_EngineOne_has_thought.count() << " seconds thinking overall, and ";
        fout << "spent " << thinking_rate_EngineOne << " seconds per instance, on average.\n";
        fout << "Version " << current_match_players.first_Engine_version << " had ";
        fout << simulation_stats.num_instances_EngineOne_thought << " such instances.\n";

        double thinking_rate_EngineTwo = simulation_stats.total_time_EngineTwo_has_thought.count();

        thinking_rate_EngineTwo /= static_cast<double>(simulation_stats.num_instances_EngineTwo_thought);

        fout << "Version " << current_match_players.second_Engine_version << " spent ";
        fout << simulation_stats.total_time_EngineTwo_has_thought.count() << " seconds thinking overall, and ";
        fout << "spent " << thinking_rate_EngineTwo << " seconds per instance, on average.\n";
        fout << "Version " << current_match_players.second_Engine_version << " had ";
        fout << simulation_stats.num_instances_EngineTwo_thought << " such instances.\n";

        fout << "The average number of moves played (by both sides) in games where Version ";
        fout << current_match_players.first_Engine_version << " won was ";
        fout << simulation_stats.average_number_moves_in_games_EngineOne_won << "\n";

        fout << "The average number of moves played (by both sides) in games where Version ";
        fout << current_match_players.second_Engine_version << " won was ";
        fout << simulation_stats.average_number_moves_in_games_EngineTwo_won << "\n";

        fout << "The average number of moves played (by both sides) in games drawn ";
        fout << "was " << simulation_stats.average_number_moves_in_games_drawn << "\n";

        fout << "The max amount of time Version " << current_match_players.first_Engine_version << " spent in ONE call ";
        fout << "to find_best_move() was " << simulation_stats.max_time_EngineOne_spent_at_once_in_find_best_move.count();
        fout << " seconds.\n";

        fout << "The max amount of time Version " << current_match_players.second_Engine_version << " spent in ONE call ";
        fout << "to find_best_move() was " << simulation_stats.max_time_EngineTwo_spent_at_once_in_find_best_move.count();
        fout << " seconds.\n";
    }
}

