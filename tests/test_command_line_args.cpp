#include <gtest/gtest.h>
#include <cxxopts.hpp>

TEST(CommandLineArgsTest, DefaultValues) {
    int argc = 1;
    const char* argv[] = {"program"};

    cxxopts::Options options(argv[0], "Platformer Prototype");
    options.add_options()
        ("w,width", "Window width", cxxopts::value<int>()->default_value("800"))
        ("h,height", "Window height", cxxopts::value<int>()->default_value("600"))
        ("f,fullscreen", "Fullscreen mode", cxxopts::value<bool>()->default_value("false"))
        ("help", "Print help");

    auto result = options.parse(argc, const_cast<char**>(argv));

    EXPECT_EQ(result["width"].as<int>(), 800);
    EXPECT_EQ(result["height"].as<int>(), 600);
    EXPECT_EQ(result["fullscreen"].as<bool>(), false);
}

TEST(CommandLineArgsTest, CustomValues) {
    int argc = 5;
    const char* argv[] = {"program", "--width", "1024", "--height", "768", "--fullscreen"};

    cxxopts::Options options(argv[0], "Platformer Prototype");
    options.add_options()
        ("w,width", "Window width", cxxopts::value<int>()->default_value("800"))
        ("h,height", "Window height", cxxopts::value<int>()->default_value("600"))
        ("f,fullscreen", "Fullscreen mode", cxxopts::value<bool>()->default_value("false"))
        ("help", "Print help");

    auto result = options.parse(argc, const_cast<char**>(argv));

    EXPECT_EQ(result["width"].as<int>(), 1024);
    EXPECT_EQ(result["height"].as<int>(), 768);
    EXPECT_EQ(result["fullscreen"].as<bool>(), true);
}

TEST(CommandLineArgsTest, HelpMessage) {
    int argc = 2;
    const char* argv[] = {"program", "--help"};

    cxxopts::Options options(argv[0], "Platformer Prototype");
    options.add_options()
        ("w,width", "Window width", cxxopts::value<int>()->default_value("800"))
        ("h,height", "Window height", cxxopts::value<int>()->default_value("600"))
        ("f,fullscreen", "Fullscreen mode", cxxopts::value<bool>()->default_value("false"))
        ("help", "Print help");

    auto result = options.parse(argc, const_cast<char**>(argv));

    EXPECT_TRUE(result.count("help"));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
