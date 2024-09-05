#include <SDL.h>
#include <iostream>
#include <vector>

// Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int BLOCK_SIZE = 20;
const int FPS = 10;

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Block {
    int x, y;
};

class Snake {
public:
    Snake(int startX, int startY) {
        blocks.push_back({startX, startY});
        direction = RIGHT;
    }

    void handleInput(SDL_Event& event) {
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    if (direction != DOWN) direction = UP;
                    break;
                case SDLK_DOWN:
                    if (direction != UP) direction = DOWN;
                    break;
                case SDLK_LEFT:
                    if (direction != RIGHT) direction = LEFT;
                    break;
                case SDLK_RIGHT:
                    if (direction != LEFT) direction = RIGHT;
                    break;
            }
        }
    }

    void move() {
        // Create a new head based on the current direction
        Block newHead = blocks[0];
        switch (direction) {
            case UP:
                newHead.y -= BLOCK_SIZE;
                break;
            case DOWN:
                newHead.y += BLOCK_SIZE;
                break;
            case LEFT:
                newHead.x -= BLOCK_SIZE;
                break;
            case RIGHT:
                newHead.x += BLOCK_SIZE;
                break;
        }

        // Add the new head to the front
        blocks.insert(blocks.begin(), newHead);

        // Remove the last block (the tail)
        blocks.pop_back();
    }

    void grow() {
        // Add a new block at the tail
        blocks.push_back(blocks.back());
    }

    bool checkCollision() {
        // Check if the snake runs into the wall
        if (blocks[0].x < 0 || blocks[0].x >= SCREEN_WIDTH || blocks[0].y < 0 || blocks[0].y >= SCREEN_HEIGHT) {
            return true;
        }

        // Check if the snake runs into itself
        for (size_t i = 1; i < blocks.size(); ++i) {
            if (blocks[0].x == blocks[i].x && blocks[0].y == blocks[i].y) {
                return true;
            }
        }

        return false;
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        for (const Block& block : blocks) {
            SDL_Rect rect = {block.x, block.y, BLOCK_SIZE, BLOCK_SIZE};
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    Block getHead() const {
        return blocks[0];
    }

private:
    std::vector<Block> blocks;
    Direction direction;
};

class Food {
public:
    Food(int x, int y) : position({x, y}) {}

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect rect = {position.x, position.y, BLOCK_SIZE, BLOCK_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }

    Block getPosition() const {
        return position;
    }

private:
    Block position;
};

int main(int argc, char* args[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialize snake and food
    Snake snake(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    Food food(100, 100);

    // Main game loop
    bool quit = false;
    SDL_Event e;
    Uint32 frameStart;
    int frameDelay = 1000 / FPS;

    while (!quit) {
        frameStart = SDL_GetTicks();

        // Handle events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            snake.handleInput(e);
        }

        // Move the snake
        snake.move();

        // Check for collisions
        if (snake.checkCollision()) {
            quit = true;
        }

        // Check if snake eats food
        if (snake.getHead().x == food.getPosition().x && snake.getHead().y == food.getPosition().y) {
            snake.grow();
            food = Food(rand() % (SCREEN_WIDTH / BLOCK_SIZE) * BLOCK_SIZE, rand() % (SCREEN_HEIGHT / BLOCK_SIZE) * BLOCK_SIZE);
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        snake.render(renderer);
        food.render(renderer);

        SDL_RenderPresent(renderer);

        // Cap the frame rate
        int frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
