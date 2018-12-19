//
//  main.cpp
//  Playground
//
//  Created by Yahui Liang on 5/11/18.
//  Copyright © 2018 Yahui Liang. All rights reserved.
//

#include "grid.hpp"
#include <iostream>
#include <string>
#include <list>
#include <cfloat>
#include <cmath>
#include <set>
#include <OpenGL/gl.h>
#include <OpenGl/glu.h>
#include <GLUT/glut.h>

struct Line
{
    int start_x;
    int start_y;
    int end_x;
    int end_y;
};

static std::list<Line> lines;
static bool finished = false;

static const int WIDTH = 800;
static const int HEIGHT = 800;
static const int ROW = 8;
static const int COL = 8;
static const int GRID_HEIGHT = HEIGHT / ROW;
static const int GRID_WIDTH = WIDTH / COL;

static std::set<Grid*> choices;
static Grid* from;
static Grid* dest;
static Grid*** grid_objs;

bool is_valid_grid(int row, int col)
{
    return col >= 0 && col < COL && row >= 0 && row < ROW;
}

void add_grid_to_choices(Grid** grids[ROW], Grid& prev, Grid& dest, int row, int col, std::set<Grid*>& choices)
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

// Create Grids
void init_grids(int grids[ROW][COL])
{
    grid_objs = new Grid**[ROW];
    for (int i = 0; i < ROW; ++i)
    {
        grid_objs[i] = new Grid*[COL];
        for (int j = 0; j < COL; ++j)
        {
            grid_objs[i][j] = new Grid(i, j, grids[i][j]);
        }
    }
}

void reset_grids()
{
    for (int i = 0; i < ROW; ++i)
    {
        for (int j = 0; j < COL; ++j)
        {
            grid_objs[i][j]->reset();
        }
    }
}

// Delete Grids
void delete_grids()
{
    for (int i = 0; i < ROW; ++i)
    {
        for (int j = 0; j < COL; ++j)
        {
            delete grid_objs[i][j];
        }
        delete[] grid_objs[i];
    }
    delete[] grid_objs;
}

// Set the coordinate for the program
void coordinate_init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, WIDTH, HEIGHT, 0.0f, 0.0f, 1.0f);
}

void draw_grid(int x_index, int y_index, Grid* grid)
{
    int x = x_index * GRID_WIDTH;
    int y = y_index * GRID_HEIGHT;
    
    grid->center_x = x + GRID_WIDTH / 2;
    grid->center_y = y + GRID_HEIGHT / 2;
    
    if (grid->is_available)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glBegin(GL_POLYGON);
    glVertex2d(0 + x, 0 + y);
    glVertex2d(GRID_WIDTH + x, 0 + y);
    glVertex2d(GRID_WIDTH + x, GRID_HEIGHT + y);
    glVertex2d(0 + x, GRID_HEIGHT + y);
    glEnd();
}

void draw_circle(float cx, float cy, float r, int num_segments)
{
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < num_segments; i++)
    {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);//get the current angle
        
        float x = r * cosf(theta);//calculate the x component
        float y = r * sinf(theta);//calculate the y component
        
        glVertex2f(x + cx, y + cy);//output vertex
        
    }
    glEnd();
}

void draw_circle_by_index(int x_index, int y_index)
{
    int x = x_index * GRID_WIDTH + GRID_WIDTH / 2;
    int y = y_index * GRID_HEIGHT + GRID_HEIGHT / 2;
    draw_circle(x, y, 10, 100);
}

void display_function()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Draw the start point
    glColor3f(0.0, 1.0, 0.0);
    draw_circle_by_index(0, 0);
    // Draw the end point
    glColor3f(1.0, 0.0, 1.0);
    draw_circle_by_index(7, 7);
    
    glColor3f(1.0, 0.0, 0.0);
    // Create grids on the window
    glLineWidth(1);
    for(int row = 0; row < ROW; row++)
    {
        for(int col = 0; col < COL; col++)
        {
            draw_grid(col, row, grid_objs[row][col]);
        }
    }
    
    glLineWidth(3);
    for (std::list<Line>::iterator it = lines.begin(); it != lines.end(); ++it)
    {
        Line line = *it;
        glBegin(GL_LINES);
        glVertex2d(line.start_x, line.start_y);
        glVertex2d(line.end_x, line.end_y);
        glEnd();
    }

    glutSwapBuffers();
}

void search_init(int from_row, int from_col, int dest_row, int dest_col)
{
    from = grid_objs[from_row][from_col];
    dest = grid_objs[dest_row][dest_col];
    from->g = 0;
    from->h = calc_dist(*from, *dest);
    from->f = from->g + from->h;
    // Push the first grid into the array
    choices.clear();
    choices.insert(from);
}

void begin_search(int n)
{
    // Determine if it is
    if (finished)
    {
        lines.clear();
        finished = false;
    }
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
    
    // Draw the line
    if (cur->back_row > -1 && cur->back_col > -1)
    {
        Grid* prev = grid_objs[cur->back_row][cur->back_col];
        Line line;
        line.start_x = prev->center_x;
        line.start_y = prev->center_y;
        line.end_x = cur->center_x;
        line.end_y = cur->center_y;
        lines.push_back(line);
        glutPostRedisplay();
    }
    
    std::cout << cur->to_string() << std::endl;
    
    if (!(cur->row == dest->row && cur->col == dest->col))
    {
        int col;
        int row;
        
        col = cur->col;
        row = cur->row - 1;
        add_grid_to_choices(grid_objs, *cur, *dest, row, col, choices);
        
        col = cur->col;
        row = cur->row + 1;
        add_grid_to_choices(grid_objs, *cur, *dest, row, col, choices);
        
        col = cur->col - 1;
        row = cur->row;
        add_grid_to_choices(grid_objs, *cur, *dest, row, col, choices);
        
        col = cur->col + 1;
        row = cur->row;
        add_grid_to_choices(grid_objs, *cur, *dest, row, col, choices);
        
        col = cur->col - 1;
        row = cur->row - 1;
        add_grid_to_choices(grid_objs, *cur, *dest, row, col, choices);
        
        col = cur->col + 1;
        row = cur->row - 1;
        add_grid_to_choices(grid_objs, *cur, *dest, row, col, choices);
        
        col = cur->col - 1;
        row = cur->row + 1;
        add_grid_to_choices(grid_objs, *cur, *dest, row, col, choices);
        
        col = cur->col + 1;
        row = cur->row + 1;
        add_grid_to_choices(grid_objs, *cur, *dest, row, col, choices);
        
        // Keep executing
        glutTimerFunc(250, begin_search, 0);
    }
    else
    {
        std::cout << "*************************" << std::endl;
        finished = true;
        reset_grids();
        search_init(0, 0, 7, 7);
        glutTimerFunc(250, begin_search, 0);
    }
}

int main(int argc, char **argv)
{
    int grids[][COL] =
    {
        // Most likely case
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 0, 1, 1, 0, 1},
        {0, 0, 1, 0, 1, 1, 0, 1},
        {1, 1, 0, 0, 1, 1, 0 ,1},
        {1, 0, 1, 0, 1, 1, 1, 1},
        {1, 0, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 0, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1}
        // Best case
//        {1, 1, 1, 1, 1, 1, 1, 1},
//        {1, 1, 1, 1, 1, 1, 1, 1},
//        {1, 1, 1, 1, 1, 1, 1, 1},
//        {1, 1, 1, 1, 1, 1, 1, 1},
//        {1, 1, 1, 1, 1, 1, 1, 1},
//        {1, 1, 1, 1, 1, 1, 1, 1},
//        {1, 1, 1, 1, 1, 1, 1, 1},
//        {1, 1, 1, 1, 1, 1, 1, 1}
        // Worst case
//        {1, 1, 1, 1, 1, 1, 1, 1},
//        {1, 1, 1, 1, 1, 1, 0, 1},
//        {1, 1, 1, 1, 1, 1, 0, 1},
//        {1, 1, 1, 1, 1, 1, 0, 1},
//        {1, 1, 1, 1, 1, 1, 0, 1},
//        {1, 1, 1, 1, 1, 1, 0, 1},
//        {1, 1, 1, 1, 1, 1, 0, 1},
//        {1, 1, 1, 1, 1, 1, 0, 1}
    };

    init_grids(grids);
    search_init(0, 0, 7, 7);

    // init GLUT and create Window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Yahui A* Demo");

    // register callbacks
    coordinate_init();
    glutDisplayFunc(display_function);
    glutTimerFunc(250, begin_search, 0);

    // enter GLUT event processing cycle
    glutMainLoop();

    delete_grids();
    return 0;
}
