#include <gtest/gtest.h>
#include "mock_sdl.h"

class ErrorHandlingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up any necessary resources for the tests
    }

    void TearDown() override {
        // Clean up any resources used in the tests
    }
};

TEST_F(ErrorHandlingTest, SDLInitializationFailure) {
    // Simulate SDL_Init failure
    EXPECT_CALL(MockSDL, SDL_Init(SDL_INIT_VIDEO))
        .WillOnce(Return(-1));

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
    EXPECT_CALL(MockSDL, SDL_Init(SDL_INIT_VIDEO))
        .WillOnce(Return(0));
    EXPECT_CALL(MockSDL, SDL_CreateWindow(_, _, _, _, _, _))
        .WillOnce(Return(nullptr));

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
    EXPECT_CALL(MockSDL, SDL_Init(SDL_INIT_VIDEO))
        .WillOnce(Return(0));
    EXPECT_CALL(MockSDL, SDL_CreateWindow(_, _, _, _, _, _))
        .WillOnce(Return(reinterpret_cast<SDL_Window*>(1)));
    EXPECT_CALL(MockSDL, SDL_CreateRenderer(_, _, _))
        .WillOnce(Return(nullptr));

    // Capture the error message
    testing::internal::CaptureStderr();
    int result = main(0, nullptr);
    std::string output = testing::internal::GetCapturedStderr();

    // Verify the error message and return code
    EXPECT_EQ(result, 1);
    EXPECT_NE(output.find("Failed to create renderer"), std::string::npos);
}
