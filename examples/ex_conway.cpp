#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

// Conway's Game of Life:
//
// Any live cell with fewer than two live neighbors dies, as if by underpopulation.
// Any live cell with more than three live neighbors dies, as if by overpopulation.
// Any live cell with two or three live neighbors lives on to the next generation.
// Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.

class Cell {
public:

    Cell(float size) : size(size) { }

    void update() {
        auto n = livingNeighbors();
        if (alive) {
            if (n < 2 || n > 3)
                willLive = false;
            else
                willLive = true;
        } else {
            if (n == 3)
                willLive = true;
        }
    }

    void post_update() {
        if (willLive) {
            alive = true;
            age++;
        } else {
            alive = false;
            age   = 0;
        }
    }

    void draw(NVGcontext* vg, float t) {
        if (alive) {            
            nvgBeginPath(vg);
            nvgRect(vg, pos.x, pos.y, size, size);
            nvgFillColor(vg, Tween::Linear(Colors::Black, Colors::White, t));
            nvgFill(vg);
        }
    }

    std::size_t livingNeighbors() {
        std::size_t i = 0;
        i += top->left->alive ? 1 : 0;
        i += top->alive ? 1 : 0;
        i += top->right->alive ? 1 :0;
        i += left->alive ? 1 : 0;
        i += right->alive ? 1 : 0;
        i += bottom->left->alive ? 1 : 0;
        i += bottom->alive ? 1 : 0;
        i += bottom->right->alive ? 1 : 0;
        return i;
    }

    Vec2 pos;
    float size;
    int age = 0;
    bool alive    = false;
    bool willLive = false;
    Cell* left; 
    Cell* right;
    Cell* top;
    Cell* bottom;
};

class Conway : public Application {
public:
    Conway(Config conf, float cell_size) : Application(conf), cell_size(cell_size)
    {
        auto [w,h] = get_window_size();
        R = (int)(h / cell_size);
        C = (int)(w / cell_size);
        cells.resize(R*C, Cell(cell_size));
        load();
    }

    void load() {
        for (int r = 0; r < R; ++r) {
            for (int c = 0; c < C; ++c) {
                auto cell = &cells[r * C + c];
                cell->pos = Vec2(c * cell_size, r * cell_size);
                if (r != 0) {
                    cell->top  = &cells[(r-1) * C + c];
                    cell->top->bottom = cell;
                    if (r == R-1) {
                        cell->bottom = &cells[c];
                        cell->bottom->top = cell;
                    }
                }
                if (c != 0) {
                    cell->left = &cells[r * C + c - 1];
                    cell->left->right = cell;
                    if (c == C-1) {
                        cell->right = &cells[r * C];
                        cell->right->left = cell;
                    }
                }
            }
        }
        spawn();
    }

    void gliderBR(std::size_t r, std::size_t c) {
        auto& cell = cells[r * C + c];
        cell.top->left->alive     = false;
        cell.top->alive           = true;
        cell.top->right->alive    = false;
        cell.left->alive          = false;
        cell.alive                = false;
        cell.right->alive         = true;
        cell.bottom->left->alive  = true;
        cell.bottom->alive        = true;
        cell.bottom->right->alive = true;
    }

    void gliderBL(std::size_t r, std::size_t c) {
        auto& cell = cells[r * C + c];
        cell.top->left->alive     = false;
        cell.top->alive           = true;
        cell.top->right->alive    = false;
        cell.left->alive          = true;
        cell.alive                = false;
        cell.right->alive         = false;
        cell.bottom->left->alive  = true;
        cell.bottom->alive        = true;
        cell.bottom->right->alive = true;
    }

    void spawn() {
        for (int i = 0; i < 1000; ++i) {
            gliderBR(random_range(0,R-1),random_range(0,C-1));
            gliderBL(random_range(0,R-1),random_range(0,C-1));
        }
    }

    void update() override {
        for (auto& c : cells)
            c.update();
        for (auto& c : cells)
            c.post_update();

        ImGui::Begin("Conway");
        ImGui::Text("%.3f FPS", ImGui::GetIO().Framerate);
        ImGui::Separator();
        if (ImGui::Button("Spawn Cells") || ImGui::IsKeyPressed(GLFW_KEY_SPACE))
            spawn();
        ImGui::End();
    }

    void draw(NVGcontext* vg) override {
        float t = 0.5 + 0.5 * std::sin((float)time().as_seconds() * 0.1f);
        set_background(Tween::Linear(Colors::White, Colors::Black, t));
        for (auto& c : cells)
            c.draw(vg,t);        
    }

public:
    bool loaded = false;
    std::vector<Cell> cells;
    int R, C;
    float cell_size;
};

int main(int argc, char const *argv[])
{
    Application::Config conf;
    conf.fullscreen = true;
    conf.msaa = false;
    conf.nvg_aa = false;
    conf.monitor = 1;
    conf.title = "Conway's Game of Life";
    conf.background = Colors::White;
    Conway conway(conf, 2.0f);
    conway.run();
    return 0;
}
