#pragma once

#include "CommonNonClassData.h"
#include <vector>

using namespace std;

class tool // Abstract base class allowing all position types in the different EngineX namespaces to be grouped under the same type.
{
public:
    // Methods listed will only be the ones explicitly called from main.cpp in the position classes:

    virtual ~tool() {} // Necessary to avoid a memory leak.

    virtual vector<treasure_spot> get_squares_amplifying_comp_2() const = 0;

    virtual vector<treasure_spot> get_squares_amplifying_comp_3() const = 0;

    virtual vector<treasure_spot> get_squares_amplifying_user_2() const = 0;

    virtual vector<treasure_spot> get_squares_amplifying_user_3() const = 0;

    virtual vector<vector<char>> get_board() const = 0;

    virtual int get_evaluation() const = 0;

    virtual bool did_computer_win() const = 0;

    virtual bool did_opponent_win() const = 0;

    virtual bool is_game_drawn() const = 0;

    virtual coordinate find_best_move_for_comp() = 0;

    virtual double get_static_thinking_time() const = 0;

    virtual void set_static_thinking_time(double val) = 0;

    virtual unique_ptr<tool> call_static_think_on_game_position(const vector <vector<char>>& boardP, bool is_comp_turnP,
                                                                coordinate last_moveP, const vector<treasure_spot>& squares_amplifying_comp_2P,
                                                                const vector<treasure_spot>& squares_amplifying_comp_3P,
                                                                const vector<treasure_spot>& squares_amplifying_user_2P,
                                                                const vector<treasure_spot>& squares_amplifying_user_3P, bool starting_new_game) = 0;

    virtual unique_ptr<tool> call_static_think_on_game_position(bool is_comp_turnP, bool starting_new_game) = 0;


};

