#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include <thread>
#include <mutex>

namespace mahi {
namespace gui {

class PlotHelper {

public:
    PlotHelper() :
    m_clock()
    {
    }

    std::unordered_map<std::string, std::vector<std::pair<float,double>>> m_all_data;
    std::unordered_map<std::string, double> data_line;

    void add_data(std::string name,double data_point){
        data_line[name] = data_point;
    }

    // return a copy of m_all_data
    std::unordered_map<std::string, std::vector<std::pair<float,double>>> get_data(){
        std::lock_guard<std::mutex> lg(m_mutex);
        auto all_data_copy = m_all_data;
        for (auto& data_single : m_all_data){
            data_single.second.clear();
        }
        return all_data_copy;
    }

    void write_data(){
        float curr_time = static_cast<float>(m_clock.get_elapsed_time().as_seconds());

        {
            std::lock_guard<std::mutex> lg(m_mutex);

            for (const auto& data_single : data_line){
                // if the string corresponding to data point is not in the map, add it
                if (m_all_data.find(data_single.first) == m_all_data.end()) {
                    m_all_data[data_single.first] = std::vector<std::pair<float,double>>();
                }
                // push back the data point to the all vector
                m_all_data[data_single.first].push_back(std::make_pair(curr_time, data_single.second));
            }
        }
    }

    mahi::util::Clock m_clock;
    std::mutex m_mutex;
};

/// Basic Plotter
class Plotter : public mahi::gui::Application {
public:
    struct ScrollingData {
        int MaxSize = 10000;
        int Offset  = 0;
        ImVector<ImVec2> Data;
        ScrollingData() { Data.reserve(MaxSize); }
        ScrollingData(int MaxSize_): MaxSize(MaxSize_) { Data.reserve(MaxSize); }
        void AddPoint(float x, float y) {
            if (Data.size() < MaxSize)
                Data.push_back(ImVec2(x,y));
            else {
                Data[Offset] = ImVec2(x,y);
                Offset =  (Offset + 1) % MaxSize;
            }
        }
    };

    /// Constructor
    Plotter(PlotHelper& plot_helper);

    void window_resize_handler(int width, int height);

    /// GUI code
    void update() override;

    void add_close(mahi::util::ctrl_bool& stop){};

    bool m_open = true;
    mahi::util::ctrl_bool m_stop;
    PlotHelper& m_plot_helper;
    bool m_paused = false;

    std::unordered_map<std::string, ScrollingData> m_scrolling_data;
    std::unordered_map<std::string, ScrollingData> m_paused_data;
};



void run_plotter(PlotHelper& plot_helper);

} // namespace gui
} // namespace mahi