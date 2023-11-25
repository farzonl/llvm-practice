#include <cmath>
#include <numeric>

#include <matplot/matplot.h>
#include <cmdParser/cmdparser.hpp>
#include <helpers/JsonHelpers.h>


int main(int argc, char** argv) {
	cli::Parser parser(argc, argv);
    using namespace matplot;
    parser.set_required<std::string>("f", "filename",
                                   "input file to visualize.");

    parser.run_and_exit_if_error(); 
    std::string filename = parser.get<std::string>("f");
    auto json = Helpers::JsonHelpers::readJsonFile(filename);
    std::vector<std::string> x;
    std::vector<int> y;
    for (auto& entry : json.items()) {
        if(entry.value().contains("functions")) {
            auto functions_list = entry.value()["functions"];
            for (auto& func : functions_list.items()) {
                x.push_back(func.key());
                y.push_back(func.value());
            }
        } else {
            x.push_back(entry.key());
            y.push_back(entry.value());
        }
    }

    int sum = std::accumulate(y.begin(), y.end(), 0);
    double avg = sum / static_cast<double>(x.size());
    
    auto b = bar(y);
    b->face_color("blue");
    hold(on);
    
    std::vector<double> ticks(x.size());
    std::iota(ticks.begin(), ticks.begin() + x.size(), 1.0);
    auto ax = gca();
    ax->x_axis().tick_values(ticks);
    ax->x_axis().ticklabels(x);
    xtickangle(45);

    std::vector<double> line_x = {0.0, static_cast<double>(x.size())}; // Adjust x-values for the line
    std::vector<double> line_y = {avg, avg};   // Set y-value for the line
    plot(line_x, line_y, "r--");
    
   show();
   return 0;
}