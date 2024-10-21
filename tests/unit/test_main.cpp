#include <gtest/gtest.h>
#include <cxxopts.hpp>
#include <string>
#include <vector>

std::vector<std::string> parseArguments(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(std::string(argv[i]));
    }
    return args;
}

TEST(CommandLineArgumentParsingTest, DefaultValues) {
    int argc = 1;
    char* argv[] = { (char*)"program" };
    auto args = parseArguments(argc, argv);

    cxxopts::Options options(args[0], "Platformer Prototype");
    int windowWidth = 800;
    int windowHeight = 600;
    bool fullscreen = false;

    options.add_options()
        ("w,width", "Window width", cxxopts::value<int>(windowWidth)->default_value("800"))
        ("h,height", "Window height", cxxopts::value<int>(windowHeight)->default_value("600"))
        ("f,fullscreen", "Fullscreen mode", cxxopts::value<bool>(fullscreen)->default_value("false"))
        ("help", "Print help");

    auto result = options.parse(argc, argv);

    EXPECT_EQ(windowWidth, 800);
    EXPECT_EQ(windowHeight, 600);
    EXPECT_EQ(fullscreen, false);
}

TEST(CommandLineArgumentParsingTest, CustomValues) {
    int argc = 4;
    char* argv[] = { (char*)"program", (char*)"--width", (char*)"1024", (char*)"--height", (char*)"768" };
    auto args = parseArguments(argc, argv);

    cxxopts::Options options(args[0], "Platformer Prototype");
    int windowWidth = 800;
    int windowHeight = 600;
    bool fullscreen = false;

    options.add_options()
        ("w,width", "Window width", cxxopts::value<int>(windowWidth)->default_value("800"))
        ("h,height", "Window height", cxxopts::value<int>(windowHeight)->default_value("600"))
        ("f,fullscreen", "Fullscreen mode", cxxopts::value<bool>(fullscreen)->default_value("false"))
        ("help", "Print help");

    auto result = options.parse(argc, argv);

    EXPECT_EQ(windowWidth, 1024);
    EXPECT_EQ(windowHeight, 768);
    EXPECT_EQ(fullscreen, false);
}

TEST(CommandLineArgumentParsingTest, FullscreenFlag) {
    int argc = 2;
    char* argv[] = { (char*)"program", (char*)"--fullscreen" };
    auto args = parseArguments(argc, argv);

    cxxopts::Options options(args[0], "Platformer Prototype");
    int windowWidth = 800;
    int windowHeight = 600;
    bool fullscreen = false;

    options.add_options()
        ("w,width", "Window width", cxxopts::value<int>(windowWidth)->default_value("800"))
        ("h,height", "Window height", cxxopts::value<int>(windowHeight)->default_value("600"))
        ("f,fullscreen", "Fullscreen mode", cxxopts::value<bool>(fullscreen)->default_value("false"))
        ("help", "Print help");

    auto result = options.parse(argc, argv);

    EXPECT_EQ(windowWidth, 800);
    EXPECT_EQ(windowHeight, 600);
    EXPECT_EQ(fullscreen, true);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
