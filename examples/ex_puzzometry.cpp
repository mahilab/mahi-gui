// MIT License
//
// Copyright (c) 2020 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include <Mahi/Util/Range.hpp>

using namespace mahi::gui;
using namespace mahi::util;

#include <deque>
#include <set>
#include <fstream>
#include <utility>
#include <algorithm>
#include <random>

using std::size_t;
using std::vector;
using std::pair;

//==============================================================================
// MATRIX OPERATIONS
//==============================================================================

typedef int         Num;
typedef vector<Num> Row;
typedef vector<Row> Matrix;

struct Coord {
    int r;
    int c;
};

inline size_t size(const Matrix& mat, int dim) {
    if (dim == 0)
        return mat.size();
    else if (dim == 1)
        return mat[0].size();
    return 0;
} 

inline Matrix zeros(size_t r, size_t c) {
    return Matrix(r, Row(c, 0));
}

inline void flipud(Matrix &mat) {
    std::reverse(mat.begin(), mat.end());
}

inline void fliplr(Matrix& mat) {
    for (auto &row : mat)
        std::reverse(row.begin(), row.end());
}

inline void rot90(Matrix& mat) {
    auto temp = mat;
    mat.resize(size(temp,1));
    for (auto j : range(size(temp,1))) {
        mat[j].resize(size(temp,0));
        for (auto i : range(size(temp,0)))
            mat[j][i] = temp[i][j];
    }
    fliplr(mat);
}

inline void permute(Matrix& mat, size_t permutation) {
    switch(permutation) {
        case 0 : { break; }
        case 1 : { rot90(mat); break; }
        case 2 : { rot90(mat); rot90(mat); break;}
        case 3 : { rot90(mat); rot90(mat); rot90(mat); break;}
        case 4 : { fliplr(mat); break;}
        case 5 : { fliplr(mat); rot90(mat); break;}
        case 6 : { fliplr(mat); rot90(mat); rot90(mat); break;}
        case 7 : { fliplr(mat); rot90(mat); rot90(mat); rot90(mat); break;}
    }
}

inline size_t uniquePermutations(const Matrix& mat, vector<size_t>& uniquePerms, vector<Matrix>& uniqueMats)  {
    size_t numUnique = 0;
    uniquePerms.clear(); uniquePerms.reserve(8);
    uniqueMats.clear();  uniqueMats.reserve(8);
    for (size_t perm = 0; perm < 8; ++perm) {
        auto temp = mat;
        permute(temp, perm);
        if (std::find(uniqueMats.begin(), uniqueMats.end(), temp) == uniqueMats.end()) {
            uniquePerms.push_back(perm);  
            uniqueMats.push_back(temp);
            numUnique++;  
        }    
    }
    return numUnique;
}

inline void sparseToDense(const Matrix& sparse, Matrix& dense) {
    auto numRows = size(sparse,0);
    auto numCols = size(sparse,1);
    dense.assign(numRows,{});
    for (size_t r = 0; r < numRows; ++r) {
        for (size_t c = 0; c < numCols; ++c) {
            if (sparse[r][c])
                dense[r].push_back((Num)c);
        }
    }
}

std::string matToStr(const Matrix& mat) {
    std::stringstream ss;
    for (auto& row : mat) {
        for (auto& val : row) {
            if (val == 0)
                ss << ". ";
            else
                ss << val << " ";
        }
        ss << std::endl;
    }
    return ss.str();
}

inline void printMat(const Matrix& mat) {
    for (auto& row : mat) {
        for (auto& val : row) {
            if (val == 0)
                std::cout << ".  ";
            else
                std::cout << val << "  ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

//==============================================================================
// GLOBAL DEFINITIONS
//==============================================================================

const float g_sideLength = 25.0f;
const float g_gridSize   = (2.0f + sqrt(2.0f)) * 0.5f * g_sideLength * 0.999f;

const Matrix g_board {
    {0, 0, 0, 0, 0, 0, 0, 0, 4, 8, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 4, 8, 4, 8, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 4, 8, 4, 8, 4, 8, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 8, 4, 8, 4, 8, 4, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 4, 8, 4, 8, 4, 8, 4, 8, 0, 0},
    {0, 0, 0, 0, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 0},
    {0, 0, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8},
    {0, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 0},
    {0, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 0, 0},
    {8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 0, 0, 0},
    {0, 8, 4, 8, 4, 8, 4, 8, 4, 8, 4, 8, 0, 0, 0, 0},
    {0, 0, 0, 4, 8, 4, 8, 4, 8, 4, 8, 0, 0, 0, 0, 0},
    {0, 0, 0, 8, 4, 8, 4, 8, 4, 8, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 8, 4, 8, 4, 8, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

const vector<Matrix> g_matrices
{
    {{8, 4, 8, 4}, {0, 8, 4, 8}},                                          
    {{0, 4, 8}, {4, 8, 4}, {8, 4, 8}},                                     
    {{8, 4, 0}, {4, 8, 4}, {8, 4, 0}},                                     
    {{0, 4, 0}, {4, 8, 4}, {8, 4, 8}, {4, 8, 4}, {8, 4, 0}},               
    {{4, 8, 0, 0}, {8, 4, 8, 0}, {0, 8, 4, 8}, {0, 4, 8, 4}, {0, 8, 4, 0}},
    {{4, 8, 0}, {8, 4, 8}, {0, 8, 4}},                                     
    {{4, 0, 0, 0}, {8, 4, 0, 0}, {4, 8, 4, 0}, {0, 4, 8, 4}},              
    {{0, 0, 0, 8, 4}, {0, 0, 8, 4, 8}, {0, 8, 4, 8, 0}, {8, 4, 0, 0, 0}},  
    {{0, 0, 0, 4, 0}, {0, 8, 4, 8, 4}, {8, 4, 8, 4, 8}, {0, 8, 4, 8, 4}},  
    {{0, 8, 4, 8}, {8, 4, 8, 0}, {4, 8, 4, 0}, {8, 0, 0, 0}},              
    {{0, 4, 8}, {0, 8, 4}, {8, 4, 0}, {0, 8, 0}},                          
    {{8, 4, 8}, {4, 8, 0}, {8, 0, 0}},                                     
    {{4, 8, 0}, {8, 4, 8}, {4, 8, 0}, {8, 0, 0}},                          
    {{4, 8, 0}, {8, 4, 8}, {4, 8, 0}},                                     
};

const vector<Coord> g_solutions {
    {9, 0}, 
    {6, 1}, 
    {5, 4}, 
    {1, 6}, 
    {0, 8}, 
    {11, 3},
    {8, 4}, 
    {5, 5}, 
    {7, 6}, 
    {4, 10},
    {12, 5},
    {11, 8},
    {7, 11},
    {5, 13},
};

const vector<Color> g_colors {
    Reds::FireBrick,
    Oranges::OrangeRed,
    Reds::LightCoral,
    Yellows::Yellow,
    Greens::YellowGreen,
    Greens::Chartreuse,
    Greens::SpringGreen,
    Cyans::LightSeaGreen,
    Blues::DeepSkyBlue,
    Blues::DodgerBlue,
    Blues::SteelBlue,
    Purples::BlueViolet,
    Purples::Magenta,
    Pinks::DeepPink
};

//==============================================================================
// DLX EXACT COVER SOLVER
//==============================================================================

/// Formulates an exact cover problem from a board and number of pieces
struct ExactCover {

    struct Info {
        size_t piece, perm, r, c;
    };

    ExactCover(const Matrix& board, const vector<Matrix>& pieces) :
        board(board), pieces(pieces), numPieces(pieces.size()), numSlots(0),
        boardRows(size(board,0)), boardCols(size(board,1))
    {
        buildSlots();
        buildSparseMatrix();
        sparseToDense(sparse,dense);
    }

    void buildSlots() {
        slots = zeros(size(board, 0), size(board, 1));
        for (auto r : range(size(slots,0))) {
            for (auto c : range(size(slots,1))) {
                if (board[r][c] != 0)
                    slots[r][c] = (Num)numSlots++;
            }
        }
    }

    void buildSparseMatrix() {
        Row row;
        sparse.reserve(2500);
        dense.reserve(2500);
        info.reserve(2500);
        for (auto piece : range(numPieces)) {
            vector<size_t> uniquePerms;
            vector<Matrix>      uniqueMats;
            auto numUnique = uniquePermutations(pieces[piece], uniquePerms, uniqueMats);
            for (auto perm : range(numUnique)) {
                for (auto r : range(boardRows)) {
                    for (auto c : range(boardCols)) {
                        row.assign(numPieces + numSlots, 0);
                        row[piece] = 1;
                        if (place(uniqueMats[perm], r, c, row)) {
                            sparse.push_back(row);  
                            info.push_back({piece, perm, r, c});
                        }                     
                    }
                }
            }
        }
        numRows = size(sparse,0);
        numCols = size(sparse,1);
    }

   
    bool place(const Matrix& permMat, size_t r, size_t c, Row& rowOut) {
        auto height = size(permMat,0);
        auto width  = size(permMat,1);
        if ((r + height) > boardRows || (c + width) > boardCols) 
            return false;
        else {
            for (size_t rr = 0; rr < height; ++rr) {
                for (size_t cc = 0; cc < width; ++cc) {
                    if (permMat[rr][cc] == 0)
                        continue;
                    auto diff = permMat[rr][cc] - board[r + rr][c + cc];
                    if  (diff != 0)
                        return false;
                    rowOut[numPieces + slots[r + rr][c + cc]] = 1;
                }
            }
        }
        return true;   
    }

    void writeToFiles() {
        std::ofstream file1, file2, file3;
        file1.open("exact_cover_sparse.txt");
        file2.open("exact_cover_dense.txt");
        file3.open("exact_cover_info.txt");
        for (auto r : range(numRows)) {
            file1 << sparse[r] << std::endl;
            file2 << dense[r]  << std::endl;
            file3 << fmt::format("{},{},{},{}",info[r].piece, info[r].perm, info[r].r, info[r].c) << std::endl;
        }
        file1.close();
        file2.close();
        file3.close();
    }    

    Matrix         board;
    vector<Matrix> pieces;
    size_t         boardRows;
    size_t         boardCols;
    size_t         numPieces;
    size_t         numSlots;
    Matrix         slots;
    Matrix         sparse;
    Matrix         dense;
    size_t         numRows;
    size_t         numCols;
    vector<Info>   info;
};

/// Implementation of Donald Knuth's DLX algorithm
struct DLX {
    /// Vector of row indices into exact cover matrix
    typedef vector<size_t> Solution;
    /// Node within toroidal linked list
    struct Cell {
        Cell(int row) : row(row) {}
        int row;              // row of cell
        Cell* R    = nullptr; // pointer to right cell
        Cell* L    = nullptr; // pointer to left cell
        Cell* U    = nullptr; // pointer to up cell
        Cell* D    = nullptr; // pointer to down cell
        Cell* C    = nullptr; // pointer to header cell
        size_t* S  = nullptr; // pointer to cell's column size
    };  
    /// Constructor, taking exact cover matrix in dense representation
    DLX(const Matrix& dense, size_t numCols) {
        size_t numCells = 1 + numCols;
        for (auto& row : dense)
            numCells += row.size();
        cells.reserve(numCells);
        sizes.assign(numCols,0);
        headers.resize(numCols);
        makeHeaderRow(numCols);
        for (size_t r = 0; r < dense.size(); ++r)
            makeRow(r, dense[r]);
        assert(numCells == cells.size());
    }
    /// Makes root and header cells
    void makeHeaderRow(size_t numCols) {
        // root
        root = &cells.emplace_back(-1);
        root->L = root;
        root->R = root;
        // headers
        for (size_t c = 0; c < numCols; ++c) {
            auto h = &cells.emplace_back(-1);
            // up/down linkage
            h->U = h;
            h->D = h;
            // left/right linkage
            h->L = root->L;
            h->L->R = h;
            root->L = h;
            h->R = root;
            // linkage to header (self)
            h->C = h;
            /// linage to size block
            h->S = &sizes[c];
            // insert into header list
            headers[c] = h;
        }
    }
    /// Takes a dense row representation and adds it to toroidal linked list
    void makeRow(size_t r, const Row& row) {
        Cell* leftMost = nullptr;
        for (auto& c : row) {
            auto cell = &cells.emplace_back(r);
            // up/down linkage
            cell->U = headers[c]->U;
            cell->U->D = cell;
            headers[c]->U = cell;
            cell->D = headers[c];
            // left/right linkage
            if (!leftMost) {
                cell->L = cell;
                cell->R = cell;    
                leftMost = cell;        
            }
            else {
                cell->L     = leftMost->L;
                cell->L->R  = cell;
                leftMost->L = cell;
                cell->R     = leftMost;
            }
            // linkage to header
            cell->C = headers[c];
            // linkage to size block
            cell->S = &sizes[c];
            ++sizes[c];
        }
    }
    /// Chooses column with fewest cells remaining in it
    inline Cell* chooseColumn() {
        auto C = root->R->C;
        size_t s = -1;        
        for (Cell* j = root->R; j != root; j = j->R) {
            if (*j->S < s) {
                s = *j->S;
                C = j->C;
            }
        }
        return C;
    }
    /// Covers a column cell C
    inline void cover(Cell* C) {
        C->R->L = C->L;
        C->L->R = C->R;
        for (Cell* i = C->D; i != C; i = i->D) {            
            for (Cell* j = i->R; j != i; j = j->R) {
                j->D->U = j->U;
                j->U->D = j->D;
                --*j->S;
            }
        }
    }
    /// Uncoverse a column cell C
    inline void uncover(Cell* C) {        
        for (Cell* i = C->U; i != C; i = i->U) {            
            for (Cell* j = i->L; j != i; j = j->L) {
                ++*j->S;
                j->D->U = j;
                j->U->D = j;
            }
        }
        C->R->L = C;
        C->L->R = C;
    }
    /// Recursive search algorithm
    bool search() {
        if (root->R == root) {
            solutions.push_back(workingSolution);
            return true;
        }
        auto C = chooseColumn();
        cover(C);        
        for (Cell* r = C->D; r != C; r = r->D) {
            workingSolution.push_back(r->row);  
            operations.push_back({(size_t)r->row,true});         
            for (Cell* j = r->R; j != r; j = j->R) 
                cover(j->C);                
            if (search() && !findAll)
                return true;
            workingSolution.pop_back();
            operations.push_back({(size_t)r->row,false});
            C = r->C;           
            for (Cell* j = r->L; j != r; j = j->L)
                uncover(j->C);                
        }
        uncover(C);
        return false;
    }    

    bool                      findAll = false; ///< find all solutions?
    vector<Cell>              cells;           ///< all root, header, and normal cells
    Cell*                     root;            ///< pointer to root cell
    vector<Cell*>             headers;         ///< pointers to headers indexed by column
    vector<size_t>            sizes;           ///< column sizes indexed by columns
    Solution                  workingSolution; ///< the working solution of search()
    vector<Solution>          solutions;       ///< all final saved solutions of search()
    vector<pair<size_t,bool>> operations;      ///< row insertions (true) and deletions (false)
};

//==============================================================================
// HELPERS
//==============================================================================

Vec2 coordPosition(const Coord& coord) {
    return Vec2(coord.c * g_gridSize, coord.r * g_gridSize);
}

Vec2 coordPosition(int i, int j) {
    return coordPosition({i,j});
}

Shape makeShape(const Matrix& mat) {
        // create square and octagon primitives
        auto sqr = make_rectangle_shape(g_sideLength,g_sideLength);
        auto oct = make_polygon_shape(8, g_sideLength, PolyParam::SideLength);
        oct.rotate(360.0f / 16.0f);
        // create que of shapes to merge
        std::deque<Shape> shapes;
        for (auto & r : range(size(mat,0))) {
            for (auto & c : range(size(mat,1))) {
                if (mat[r][c] == 4)
                    shapes.push_back(sqr);
                else if (mat[r][c] == 8)
                    shapes.push_back(oct);
                oct.move(g_gridSize, 0);  
                sqr.move(g_gridSize, 0);
            }
            oct.move(-g_gridSize * size(mat,1), g_gridSize);
            sqr.move(-g_gridSize * size(mat,1), g_gridSize);
        }
        // marge shapes
        Shape shape;
        while (!shapes.empty()) {
            auto toMerge = shapes.front();
            shapes.pop_front();
            auto merged = clip_shapes(shape, toMerge, ClipType::Union);
            if (merged.size() == 1)
                shape = merged[0];
            else
                shapes.push_back(toMerge);
        }
        return shape;
};

//==============================================================================
// BOARD
//==============================================================================

class Board
{
  public:
  
    Board() : matrix(g_board), color(Grays::Gray50) {
        shape.set_point_count(4);
        shape.set_point(0, coordPosition(9,-2));
        shape.set_point(1, coordPosition(-2,9));
        shape.set_point(2, coordPosition(6,17));
        shape.set_point(3, coordPosition(17,6));
        shape.set_radii(g_gridSize);
        auto hole = makeShape(matrix);
        hole = offset_shape(hole, 2.0f);
        shape.push_back_hole(hole);
    }

    void draw(NVGcontext* vg) {
        Vec2 tl = shape.bounds().position();
        Vec2 br = tl + shape.bounds().size();
        nvgBeginPath(vg);
        nvgShapeWithHoles(vg, shape);
        auto paint = nvgLinearGradient(vg, tl.x, tl.y, br.x, br.y, {0.5f,0.5f,0.5f,0.25f}, {0.5f,0.5f,0.5f,0.5f});
        nvgFillPaint(vg, paint);
        nvgFill(vg);
        nvgStrokeColor(vg, Grays::Gray80);
        nvgStrokeWidth(vg, 1);
        nvgStroke(vg);
    }

    Shape shape;
    Matrix matrix;
    Color color;
};

//==============================================================================
// PIECE
//==============================================================================

class Piece
{
  public:

    Piece(const Matrix &mat, const Color &col, Application* app) : m_app(app), matrix(mat), color(col)
    {
        shape = makeShape(matrix);
        shape = offset_shape(shape, -2.0f);
        perm = 0;
        pos = Vec2(0,0);
        origin = Vec2(0,0);
        scale = Vec2(1,1);
        rotation = 0;
    }

    void updateMatrix(const Matrix &mat) {
        matrix = mat;
        shape = makeShape(matrix);
        shape = offset_shape(shape, -2.0f);
        perm = 0;
    }

    Vec2 getScale(int perm) {
        return perm < 4 ? Vec2(1,1) : Vec2(-1,1);
    }

    Vec2 getOrigin(int perm) {
        return g_gridSize * Vec2((perm == 2 || perm == 3 || perm == 4 || perm == 5) ? (float)size(matrix,1) - 1 : 0.0f, 
                                 (perm == 1 || perm == 2 || perm == 5 || perm == 6) ? (float)size(matrix,0) - 1 : 0.0f);
    }

    float getRotation(int perm) {
        return 90.0f * (float)(perm % 4);
    }

    void place(const Coord& new_coord, int new_perm) {
        pos = coordPosition(new_coord); 
        origin = getOrigin(new_perm);
        scale  = getScale(new_perm);
        rotation = getRotation(new_perm);
        coord = new_coord;
        perm = new_perm;
    }


    Enumerator transition(Coord to_coord, int perm, float duration) {
        transitioning = true;
        // start state
        auto startPosition = pos;
        auto startRotation = rotation;
        auto startScale    = scale;
        auto startOrigin   = origin;
        // end state
        auto endPosition   = coordPosition(to_coord);
        auto endRotation   = getRotation(perm);
        auto endScale      = getScale(perm);
        auto endOrigin     = getOrigin(perm);
        // animation loop
        float elapsedTime = 0.0f;
        while (elapsedTime < duration) {
            float t  = elapsedTime / duration;
            pos = Tween::Smootherstep(startPosition, endPosition, t);
            rotation = Tween::Smootherstep(startRotation, endRotation, t);
            scale = Tween::Smootherstep(startScale, endScale, t);
            origin = Tween::Smootherstep(startOrigin, endOrigin, t);
            elapsedTime += (float)m_app->delta_time().as_seconds();
            co_yield nullptr;
        }        
        pos = endPosition;
        rotation = endRotation;
        scale = endScale;
        origin = endOrigin;
        place(to_coord, perm);
        transitioning = false;        
    }

    void draw(NVGcontext* vg) {
        float angle  = -rotation * 3.141592654f / 180.f;
        float c = static_cast<float>(std::cos(angle));
        float s   = static_cast<float>(std::sin(angle));
        float sxc    = scale.x * c;
        float syc    = scale.y * c;
        float sxs    = scale.x * s;
        float sys    = scale.y * s;
        float tx     = -origin.x * sxc - origin.y * sys + pos.x;
        float ty     =  origin.x * sxs - origin.y * syc + pos.y;
        nvgTransform(vg, sxc, -sxs, sys, syc, tx, ty);
        Vec2 tl = shape.bounds().position();
        Vec2 br = tl + shape.bounds().size();
        nvgBeginPath(vg);
        nvgShape(vg, shape);
        auto paint = nvgLinearGradient(vg, tl.x, tl.y, br.x, br.y, with_alpha(color,0.25f), with_alpha(color,0.5f));   
        nvgFillPaint(vg, paint);
        nvgFill(vg);        
        nvgStrokeColor(vg, Tween::Linear(Whites::White, color, 0.5f));
        nvgStrokeWidth(vg,1);
        nvgStroke(vg);
    }

    Enumerator test() {
        while (true) {
            print("{},{}",pos.x,pos.y);
            co_yield nullptr;
        }
    }

    Application* m_app;
    Shape shape;
    bool transitioning = false;
    bool selected = false;
    int perm;
    Coord coord;
    Matrix matrix;
    Color color;
    Vec2 origin;
    Vec2 pos;
    Vec2 scale;
    float rotation;
    
};

//==============================================================================
// PUZZOMETRY (ROOT OBJECT)
//==============================================================================

class Puzzometry : public Application {
public:
    Puzzometry() : Application(800,800,"Puzzometry",false,0), problem(g_board, g_matrices), solver(problem.dense, problem.numCols) { 

        pieces.reserve(g_matrices.size());
        for (size_t i = 0; i < g_matrices.size(); ++i) {
            auto p = Piece(g_matrices[i], g_colors[i], this);
            p.place(g_solutions[i], 0);
            pieces.emplace_back(std::move(p));
        }
        // solve
        set_vsync(false);  
        createCheckerBoard();   
    }

    void createCheckerBoard() {
        checker = nvgluCreateFramebuffer(m_vg, 16, 16, NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY);     
        nvgluBindFramebuffer(checker);
        glViewport(0, 0, 16, 16);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        nvgBeginFrame(m_vg, 16, 16, 1);
        nvgBeginPath(m_vg);
        nvgRect(m_vg, 0, 0, 8, 8);
        nvgFillColor(m_vg, Grays::Gray10);
        nvgFill(m_vg);
            nvgBeginPath(m_vg);
        nvgRect(m_vg, 8, 8, 16, 16);
        nvgFillColor(m_vg, Grays::Gray10);
        nvgFill(m_vg);
        nvgEndFrame(m_vg);
        nvgluBindFramebuffer(nullptr);
    }

    ~Puzzometry() {
        nvgluDeleteFramebuffer(checker);
    }

    Enumerator shuffle() {
        stop_coroutines();    
        animating = false;  
        percent = 0;
        solver.solutions.clear();
        std::random_device rng;
        std::mt19937 gen(rng()); 
        std::shuffle(pieces.begin(), pieces.end(), gen);
        std::vector<Matrix> matrices;
        for (auto& p : pieces) {
            int perm = random_range(0,7);
            auto mat = p.matrix;
            permute(mat, perm);
            matrices.push_back(mat);
            co_yield start_coroutine(p.transition({0,0},perm,0.1f));
            p.updateMatrix(mat);
            p.place({0,0},0);
        }
        problem = ExactCover(g_board, matrices);
    }


    void solve() {
        Clock clk;
        solver = DLX(problem.dense, problem.numCols); 
        solver.search();
        ms_solve = clk.get_elapsed_time().as_milliseconds();
    }

    void update() {
        ImGui::Begin("Puzzometry");
        if (ImGui::Button("Shuffle")) 
            start_coroutine(shuffle());      
        ImGui::SameLine();  
        ImGui::BeginDisabled(animating || solver.solutions.size() > 0);
        if (ImGui::Button("Solve")) 
            solve();        
        ImGui::EndDisabled();
        if (solver.solutions.size()) {
            ImGui::SameLine();
            ImGui::Text("Solve Time: %d ms | Operations: %d", ms_solve, solver.operations.size(), solver.solutions.size());
        }

        ImGui::BeginDisabled(solver.solutions.size() == 0);
        if (ImGui::Button("Animate"))
            start_coroutine(animateSolution());
        ImGui::SameLine();
        ImGui::ProgressBar(percent);        
        static float scale = 1;
        if (ImGui::DragFloat("Animation Speed", &scale, 0.1f, 0, 100, "%.1fx"))
            set_time_scale(scale);
        ImGui::EndDisabled();
        ImGui::Separator();
        for (int i = 0; i < pieces.size(); ++i) {
            ImGui::PushID(i);
            ImGui::PushItemWidth(100);
            ImGui::ColorButton("##Color",pieces[i].color); ImGui::SameLine();
            int r = pieces[i].coord.r;
            int c = pieces[i].coord.c;
            int p = pieces[i].perm;
            if (ImGui::SliderInt("Row",&r,0,15))
            { 
                pieces[i].coord.r = r;
                pieces[i].place(pieces[i].coord, pieces[i].perm);
            }
            ImGui::SameLine();
            if (ImGui::SliderInt("Col",&c,0,15))
            { 
                pieces[i].coord.c = c;
                pieces[i].place(pieces[i].coord, pieces[i].perm);
            }
            ImGui::SameLine();
            if (ImGui::SliderInt("Perm",&p,0,7))
            { 
                pieces[i].perm = p;
                pieces[i].place(pieces[i].coord, pieces[i].perm);
            }
            ImGui::PopItemWidth();
            ImGui::PopID();
        }
        ImGui::End();
    }

    void draw(NVGcontext* vg) override {
        NVGpaint img = nvgImagePattern(vg, 0, 0, 16, 16, 0, checker->image, 1.0f);
        nvgBeginPath(vg);
        nvgRect(vg,0,0,800,800);
        nvgFillPaint(vg, img);
        nvgFill(vg);

        nvgTranslate(vg, 400 - g_gridSize*7.5f,400-g_gridSize*7.5f);

        // grid
        for (int i = 0; i < 16; ++i) {
            auto p1 = coordPosition(-1, i);
            auto p2 = coordPosition(16, i);
            auto p3 = coordPosition(i, -1);
            auto p4 = coordPosition(i, 16);
            nvgBeginPath(vg);
            nvgMoveTo(vg, p1.x, p1.y);
            nvgLineTo(vg, p2.x, p2.y);
            nvgStrokeColor(vg, Grays::Gray30);
            nvgStrokeWidth(vg, 1);
            nvgStroke(vg);
            nvgBeginPath(vg);
            nvgMoveTo(vg, p3.x, p3.y);
            nvgLineTo(vg, p4.x, p4.y);
            nvgStrokeColor(vg, Grays::Gray30);
            nvgStrokeWidth(vg, 1);
            nvgStroke(vg);
        }

        board.draw(vg);
        for (auto& p : pieces) {
            nvgSave(vg);
            p.draw(vg);
            nvgRestore(vg);
        }
    }

    Enumerator animateSolution() {
        stop_coroutines();
        animating = true;
        for (auto& p : pieces)
            co_yield start_coroutine(p.transition({0, 0}, 0, 0.1f));
        // co_yield yield_time_scaled(500_ms);
        for (auto i : range(solver.operations.size())) {
            percent = (float)i / (float)solver.operations.size();
            auto op    = solver.operations[i];
            auto info  = problem.info[op.first];
            auto& piece = pieces[info.piece];
            if (op.second) {
                // insetion
                co_yield start_coroutine(
                    piece.transition({(int)info.r, (int)info.c}, info.perm, 0.25f));
                co_yield yield_time_scaled(150_ms);
            } else {
                // removal
                auto nextOp   = solver.operations[(i + 1) % solver.operations.size()];
                auto nextInfo = problem.info[nextOp.first];
                if (info.piece != nextInfo.piece)
                    co_yield start_coroutine(piece.transition({0,0}, piece.perm, 0.1f));
            }
        }
        animating = false;
    }

    ExactCover problem;
    DLX solver;
    int ms_solve = -1;
    Board board;
    vector<Piece> pieces;
    bool spinning = false;
    bool animating = false;
    bool toggled = true;
    float percent = 0;
    NVGLUframebuffer* checker = NULL;
};


int main(int argc, char const *argv[])
{
    Puzzometry puzz;
    puzz.run();
    return 0;
}
