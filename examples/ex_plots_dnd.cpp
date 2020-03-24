#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

class PlotDragNDropDemo : public Application {
public:
    PlotDragNDropDemo() : Application(800, 500, "Plot Drag N Drop Demo", false) {
        ImGui::DisableViewports();
        plot.x_axis.maximum  = 10;
        plot.x_axis.lock_min = true;
        plot.x_axis.lock_max = true;
    }

    void update() override {
        float t = (float)time().as_seconds();

        ImGui::BeginFixed("Data List", {0, 0}, {800, 500}, ImGuiWindowFlags_NoTitleBar);
        ImGui::BeginGroup();
        if (ImGui::Button("Clear Plot", {100, 0}))
            items.clear();
        for (int i = 0; i < 10; ++i) {
            std::string label = fmt::format("data_{}", i);
            ImGui::Selectable(label.c_str(), false, 0, {100, 0});
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                payload = i;
                ImGui::SetDragDropPayload("DND_PLOT", &payload, sizeof(int));
                ImGui::Text(label.c_str());
                ImGui::EndDragDropSource();
            }
        }
        ImGui::EndGroup();

        for (auto& i : items)
            ImGui::PlotItemBufferPoint(
                i, t, i.data.back().y + random_range(0.0049f, 0.0051f) * random_range(-1.0, 1.0), 1000);

        ImGui::SameLine();
        ImGui::Plot("My Rolling Plot", plot, items);
        ImGui::PlotAxisScroll(plot.x_axis, t, 10);

        if (ImGui::BeginDragDropTarget()) {
            if (ImGui::AcceptDragDropPayload("DND_PLOT")) {
                items.emplace_back();
                items.back().label = fmt::format("data_{}", payload);
                items.back().data.push_back({t, (float)random_range(0.25, 0.75)});
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::End();
    }

    int                          payload;
    ImGui::PlotInterface         plot;
    std::vector<ImGui::PlotItem> items;
};

int main(int argc, char const* argv[]) {
    PlotDragNDropDemo demo;
    demo.run();
    return 0;
}