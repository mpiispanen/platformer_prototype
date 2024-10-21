#include <gtest/gtest.h>
#include "mock_sdl.h"

class ErrorHandlingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up any necessary resources for the tests
        mock_sdl = new MockSDL();
    }

    void TearDown() override {
        // Clean up any resources used in the tests
        delete mock_sdl;
        mock_sdl = nullptr;
    }
};

TEST_F(ErrorHandlingTest, SDLInitializationFailure) {
    // Simulate SDL_Init failure
    EXPECT_CALL(*mock_sdl, SDL_Init(SDL_INIT_VIDEO))
        .WillOnce(testing::Return(-1));

    // Capture the error message
    testing::internal::CaptureStderr();
    int result = main(0, nullptr);
    std::string output = testing::internal::GetCapturedStderr();

    // Verify the error message and return code
    EXPECT_EQ(result, 1);
    EXPECT_NE(output.find("Failed to initialize SDL2"), std::string::npos);
}

TEST_F(ErrorHandlingTest, WindowCreationFailure) {
    // Simulate SDL_CreateWindow failure
    EXPECT_CALL(*mock_sdl, SDL_Init(SDL_INIT_VIDEO))
        .WillOnce(testing::Return(0));
    EXPECT_CALL(*mock_sdl, SDL_CreateWindow(testing::_, testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(nullptr));

    // Capture the error message
    testing::internal::CaptureStderr();
    int result = main(0, nullptr);
    std::string output = testing::internal::GetCapturedStderr();

    // Verify the error message and return code
    EXPECT_EQ(result, 1);
    EXPECT_NE(output.find("Failed to create window"), std::string::npos);
}

TEST_F(ErrorHandlingTest, RendererCreationFailure) {
    // Simulate SDL_CreateRenderer failure
    EXPECT_CALL(*mock_sdl, SDL_Init(SDL_INIT_VIDEO))
        .WillOnce(testing::Return(0));
    EXPECT_CALL(*mock_sdl, SDL_CreateWindow(testing::_, testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(reinterpret_cast<SDL_Window*>(1)));
    EXPECT_CALL(*mock_sdl, SDL_CreateRenderer(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(nullptr));

    // Capture the error message
    testing::internal::CaptureStderr();
    int result = main(0, nullptr);
    std::string output = testing::internal::GetCapturedStderr();

    // Verify the error message and return code
    EXPECT_EQ(result, 1);
    EXPECT_NE(output.find("Failed to create renderer"), std::string::npos);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
