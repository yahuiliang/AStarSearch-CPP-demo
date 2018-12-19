//
//  grid.hpp
//  Playground
//
//  Created by Yahui Liang on 5/15/18.
//  Copyright © 2018 Yahui Liang. All rights reserved.
//

#ifndef grid_hpp
#define grid_hpp

#include <stdio.h>
#include <string>

struct Grid
{
    const int row, col;
    double center_x, center_y;
    double g, h, f;
    const bool is_available;
    bool visited;
    int back_col, back_row;
    
    Grid(int _row, int _col, bool _is_available);
    
    void reset();
    double calc_g(Grid& prev);
    double calc_h(Grid& dest);
    std::string to_string();
};

double calc_dist(Grid& g1, Grid& g2);

#endif /* grid_hpp */
