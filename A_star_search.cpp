//
//  A_star_search.cpp
//  Playground
//
//  Created by Yahui Liang on 5/15/18.
//  Copyright © 2018 Yahui Liang. All rights reserved.
//

#include "grid.hpp"
#include <stdio.h>
#include <cstdlib>
#include <set>
#include <cfloat>
#include <iostream>
#include <list>
#include <chrono>

#define ROW 200
#define COL 200
#define SIM_TIME 100

static int cur_max_row = 0;
static int cur_max_col = 0;
static int is_available_mtx[ROW][COL] = {0};
static Grid* grids[ROW][COL] = {nullptr};

// Determine if the grid is out of bounds
bool is_valid_grid(int row, int col)
{
    return col >= 0 && col < cur_max_col && row >= 0 && row < cur_max_row;
}

/**
 * The method is for adding the grid to choices set if the grid is valid, not visited, and its f value is
 * smaller than the previous record (if itself is existed in the choices set before).
 * @param prev The previous grid of the current grid
 * @param dest The destination grid
 * @param row The row of the current grid
 * @param col The col of the current grid
 * @param choices The set which stores all possible choices
 */
void add_grid_to_choices(Grid& prev, Grid& dest, int row, int col, std::set<Grid*>& choices)
{
    if (is_valid_grid(row, col))
    {
        Grid* cur = grids[row][col];
        if (!cur->visited && cur->is_available)
        {
            double g = cur->calc_g(prev);
            double h = cur->calc_h(dest);
            double f = g + h;
            if (f < cur->f)
            {
                cur->g = g;
                cur->h = h;
                cur->f = f;
                cur->back_col = prev.col;
                cur->back_row = prev.row;
                std::set<Grid*>::iterator it = choices.find(cur);
                if (it != choices.end())
                {
                    choices.erase(it);
                }
                choices.insert(cur);
            }
        }
    }
}

// Create worst case grids for the simulation purpose
void init_worst_case_grids(int cus_mx_row, int cus_mx_col)
{
    // Return immediately if row or col indexes are invalid
    if (cus_mx_row > ROW || cus_mx_col > COL)
    {
        return;
    }
    // Start initiating grids
    for (int i = 0; i < cus_mx_row; ++i)
    {
        for (int j = 0; j < cus_mx_col; ++j)
        {
            if (j == cus_mx_col - 2 && i != 0)
            {
                grids[i][j] = new Grid(i, j, false); // Create barrier
            }
            else
            {
                grids[i][j] = new Grid(i, j, true); // Create available grid
            }
//            std::cout << grids[i][j]->is_available << " ";
        }
//        std::cout << std::endl;
    }
}

void init_most_likely_case_grids(int cus_mx_row, int cus_mx_col)
{
    for (int i = 0; i < cus_mx_row; ++i)
    {
        for (int j = 0; j < cus_mx_col; ++j)
        {
            if ((i == 0 && j == 0) || (i == cus_mx_row - 1 && j == cus_mx_col - 1))
            {
                grids[i][j] = new Grid(i, j, true);
            }
            else
            {
                grids[i][j] = new Grid(i, j, is_available_mtx[i][j]);
            }
//            std::cout << grids[i][j]->is_available << " ";
        }
//        std::cout << std::endl;
    }
}

void init_best_case_grids(int cus_mx_row, int cus_mx_col)
{
    for (int i = 0; i < cus_mx_row; ++i)
    {
        for (int j = 0; j < cus_mx_col; ++j)
        {
            grids[i][j] = new Grid(i, j, true);
//            std::cout << grids[i][j]->is_available << " ";
        }
//        std::cout << std::endl;
    }
}

// Delete grids
void delete_grids(int cus_mx_row, int cus_mx_col)
{
    // Return immediately if row or col indexes are invalid
    if (cus_mx_row > ROW || cus_mx_col > COL)
    {
        return;
    }
    // Start deleting grids
    for (int i = 0; i < cus_mx_row; ++i)
    {
        for (int j = 0; j < cus_mx_col; ++j)
        {
            delete grids[i][j];
        }
    }
}

bool a_star_search(int from_row, int from_col, int dest_row, int dest_col)
{
    std::set<Grid*> choices;
    Grid* from_grid = grids[from_row][from_col];
    Grid* dest_grid = grids[dest_row][dest_col];
    from_grid->g = 0;
    from_grid->h = calc_dist(*from_grid, *dest_grid);
    from_grid->f = from_grid->g + from_grid->h;
    // Push the first grid into the array
    choices.clear();
    choices.insert(from_grid);
    
    bool found = false;
    
    while (!choices.empty() && !found)
    {
        // Get the grid with the minimum f
        std::set<Grid*>::iterator min = choices.end();
        double min_f = FLT_MAX;
        for (std::set<Grid*>::iterator it = choices.begin(); it != choices.end(); ++it)
        {
            if ((*it)->f < min_f)
            {
                min_f = (*it)->f;
                min = it;
            }
        }
        
        Grid* cur = *min;
        cur->visited = true;
        choices.erase(min);
        
        if (!(cur->row == dest_grid->row && cur->col == dest_grid->col))
        {
            // Insert adjacent grids to possible choices
            int col;
            int row;
            
            col = cur->col;
            row = cur->row - 1;
            add_grid_to_choices(*cur, *dest_grid, row, col, choices);
            
            col = cur->col;
            row = cur->row + 1;
            add_grid_to_choices(*cur, *dest_grid, row, col, choices);
            
            col = cur->col - 1;
            row = cur->row;
            add_grid_to_choices(*cur, *dest_grid, row, col, choices);
            
            col = cur->col + 1;
            row = cur->row;
            add_grid_to_choices(*cur, *dest_grid, row, col, choices);
            
            col = cur->col - 1;
            row = cur->row - 1;
            add_grid_to_choices(*cur, *dest_grid, row, col, choices);
            
            col = cur->col + 1;
            row = cur->row - 1;
            add_grid_to_choices(*cur, *dest_grid, row, col, choices);
            
            col = cur->col - 1;
            row = cur->row + 1;
            add_grid_to_choices(*cur, *dest_grid, row, col, choices);
            
            col = cur->col + 1;
            row = cur->row + 1;
            add_grid_to_choices(*cur, *dest_grid, row, col, choices);
        }
        else
        {
            // Set the flag and terminate the loop after the grid has been found
            found = true;
        }
    }
    return found;
}

void print_path(int trace_back_start_row, int trace_back_start_col)
{
    std::list<Grid*> path;
    Grid* grid = grids[trace_back_start_row][trace_back_start_col];
    path.push_front(grid);
    while (grid->back_row != -1 && grid->back_col != -1)
    {
        grid = grids[grid->back_row][grid->back_col];
        path.push_front(grid);
    }
    std::cout << "{";
    for (std::list<Grid*>::iterator it = path.begin(); it != path.end(); ++it)
    {
        grid = *it;
        std::cout << grid->to_string() << " ";
    }
    std::cout << "}" << std::endl;
}

// Initialize availability matrix
void init_is_available_mtx()
{
    for (int i = 0; i < ROW; ++i)
    {
        for (int j = 0; j < COL; ++j)
        {
            is_available_mtx[i][j] = (int) (rand() % 2);
        }
    }
}

// Simulate the A* search
double sim(int cus_mx_row, int cus_mx_col)
{
    auto start = std::chrono::high_resolution_clock::now();
    a_star_search(0, 0, cus_mx_row - 1, cus_mx_col - 1);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = finish - start;
    return elapsed.count();
}

// Simulate the most likely case
void sim_most_likely_case()
{
    for (int i = 5; i <= ROW; i += 5)
    {
        double time = 0;
        for (int j = 0; j < SIM_TIME; ++j)
        {
            cur_max_row = i;
            cur_max_col = i;
            init_most_likely_case_grids(i, i);
            time += sim(i, i);
            delete_grids(i, i);
        }
        time /= SIM_TIME;
        std::cout << time << std::endl;
    }
}

// Simulate the worst case
void sim_worst_case()
{
    for (int i = 5; i <= ROW; i += 5)
    {
        double time = 0;
        for (int j = 0; j < SIM_TIME; ++j)
        {
            cur_max_row = i;
            cur_max_col = i;
            init_worst_case_grids(i, i);
            time += sim(i, i);
            delete_grids(i, i);
        }
        time /= SIM_TIME;
        std::cout << time << std::endl;
    }
}

// Simulate the best case
void sim_best_case()
{
    for (int i = 5; i <= ROW; i += 5)
    {
        double time = 0;
        for (int j = 0; j < SIM_TIME; ++j)
        {
            cur_max_row = i;
            cur_max_col = i;
            init_best_case_grids(i, i);
            time += sim(i, i);
            delete_grids(i, i);
        }
        time /= SIM_TIME;
        std::cout << time << std::endl;
    }
}

int main()
{
//    srand(static_cast<unsigned int>(time(NULL)));
//    init_is_available_mtx();
//    sim_worst_case();
    int a[2][2];
    a[0][0] = 1;
    a[0][1] = 2;
    a[1][0] = 3;
    a[1][1] = 4;
    return 0;
}
