//
//  grid.cpp
//  Playground
//
//  Created by Yahui Liang on 5/15/18.
//  Copyright © 2018 Yahui Liang. All rights reserved.
//

#include "grid.hpp"
#include <cfloat>
#include <cmath>

Grid::Grid(int _row, int _col, bool _is_available): row(_row), col(_col), is_available(_is_available)
{
    visited = false;
    g = FLT_MAX;
    f = FLT_MAX;
    h = FLT_MAX;
    back_col = -1;
    back_row = -1;
}

void Grid::reset()
{
    visited = false;
    g = FLT_MAX;
    f = FLT_MAX;
    h = FLT_MAX;
    back_col = -1;
    back_row = -1;
}

double Grid::calc_g(Grid& prev)
{
    return prev.g + calc_dist(prev, *this);
}

double Grid::calc_h(Grid& dest)
{
    return calc_dist(*this, dest);
}

std::string Grid::to_string()
{
    return "(" + std::to_string(row)  + "," + std::to_string(col) + ")";
}

double calc_dist(Grid& g1, Grid& g2)
{
    int row_diff = abs(g1.row - g2.row);
    int col_diff = abs(g1.col - g2.col);
    return sqrt(pow(row_diff, 2) + pow(col_diff, 2));
}
