#include "boost/program_options.hpp"
#include "Core/Application.h"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    sb::ApplicationSpec spec;
    
    po::options_description desc("Allowed options");
    desc.add_options()("pix", "pix Enable");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("pix")) spec._pix = true;

    sb::Application App(spec);
    App.Run();

    return 0;
}