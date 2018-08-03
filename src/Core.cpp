#include "Core.h"

#include <chrono>
#include <iostream>
#include <queue>
#include <random>
#include <thread>

#include <ncurses.h>

// FIXME
// 1. It should not to change direction to the direction of snake's neck.
//    Currently it is possible to instantly die by reversing direction.
// 2. It should be possible to chase your own tail without dying even
//    if it is right in front of snake's head.
// 3. There should be a way to speed up advancing in current direction.

/*
class Model
{
public:
  Model();

private:
  
};

class IOutputDevice
{
public:
  virtual ~IOutputDevice() = 0;

  virtual void set(int x, int y, char c) = 0;
  virtual void render() = 0;
};

class ModelRenderer
{
public:
  ModelRenderer(Model& model, IOutputDevice& outputDevice)
    : mModel(model)
    , mOutputDevice(outputDevice)
  {
  }

  void render()
  {
    // TODO read mModel and do mOutputDevice.set(x, y, c)

    mOutputDevice.render();
  }

private:
  Model& mModel;
  IOutputDevice& mOutputDevice;
};
*/
namespace {
  char c;
  int x;
  int y;

  int ch;

  void Input() {
    ch = ERR;
    int k = ERR;
    while ((k = getch()) != ERR) {
      ch = k;
    }
  }

  static constexpr int w = 40;
  static constexpr int h = 25;

  int pos_x = w / 2;
  int pos_y = h / 2;

  double progress = 0.0;
  double progress_per_update = 0.1;

  bool game_over_flag = false;

  int iteration = 0;

  static int direction = KEY_UP;

  enum class CellType {
    NOTHING = 0,
    FOOD,
    SNAKE,
  };

  int food_x;
  int food_y;

  struct Point {
    int x;
    int y;
  };

  std::deque<Point> snake;

  void PutFoodSomewhere() {
    // TODO check target location first if there is already something
    food_x = rand() % w;
    food_y = rand() % h;
  }

  void Initialize() {
    // set snake's location
    pos_x = w / 2;
    pos_y = h / 2;
    Point point;
    point.x = pos_x;
    point.y = pos_y;
    snake.push_back(point);
    // put food
    PutFoodSomewhere();
  }

  CellType GetCellType(int x, int y) {
    // figure out what is at specified location
    if (x == food_x && y == food_y) {
      return CellType::FOOD;
    }

    for (int i = 0; i < snake.size(); ++i) {
      if (snake[i].x == x && snake[i].y == y) {
        return CellType::SNAKE;
      }
    }

    return CellType::NOTHING;
  }

  void Advance() { 
    // Has to advance in the direction of current direction.
    int target_x = pos_x;
    int target_y = pos_y;
    
    switch (direction) {
      case KEY_UP:
        target_y = (pos_y - 1 + h) % h;
        break;
      case KEY_DOWN:
        target_y = (pos_y + 1) % h;
        break;
      case KEY_LEFT:
        target_x = (pos_x - 1 + w) % w;
        break;
      case KEY_RIGHT:
        target_x = (pos_x + 1) % w;
        break;
      case ERR:
      default:
        game_over_flag = true;
        break;
    }

    pos_x = target_x;
    pos_y = target_y;

    // 1. nothing in front of snake, simply move head there and update tail location.
    // 2. food in front of snake: eat food, keep tail's location but move head
    // 3. own body is in front: game over
    switch (GetCellType(target_x, target_y)) {
      case CellType::FOOD:
        Point food_point;
        food_point.x = target_x;
        food_point.y = target_y;
        snake.push_back(food_point);
        PutFoodSomewhere();
        break;
      case CellType::SNAKE:
        game_over_flag = true;
        break;
      case CellType::NOTHING:
      default:
        Point p = snake.front();
        p.x = target_x;
        p.y = target_y;
        snake.pop_front();
        snake.push_back(p);
        break;
    }
  }

  void Update() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 9);
    static std::uniform_int_distribution<> disx(0, w - 1);
    static std::uniform_int_distribution<> disy(0, h - 1);

    if (game_over_flag) {
      return;
    }

    c = '0' + dis(gen);
    y = disy(gen);
    x = disx(gen);

    switch (ch) {
      case KEY_UP:
        //pos_y = (pos_y - 1 + h) % h;
        //break;
      case KEY_DOWN:
        //pos_y = (pos_y + 1) % h;
        //break;
      case KEY_LEFT:
        //pos_x = (pos_x - 1 + w) % w;
        //break;
      case KEY_RIGHT:
        //pos_x = (pos_x + 1) % w;
        // TODO check if not neck
        direction = ch;
        break;
      case ERR:
      default:
        break;
    }

    ++iteration;
    progress += progress_per_update;
    progress_per_update += 0.0001;

    while (progress >= 1.0) {
      Advance();
      progress -= 1.0;
    }
  }

  void Clear() {
    static constexpr char kEmptyCell = ' ';

    for (int y = 0; y < h; ++y) {
      for (int x = 0; x < w; ++x) {
        move(y + 1, x * 2 + 1);
        addch(kEmptyCell);
        addch(kEmptyCell);
      }
    }
  }

  void Border() {
    move(0, 0);
    addch('+');
    move(h + 1, 0);
    addch('+');
    move(0, w * 2 + 1);
    addch('+');
    move(h + 1, w * 2 + 1);
    addch('+');

    for (x = 0; x < w; ++x) {
      move(0, x * 2 + 1);
      addch('-');
      addch('-');
      move(h + 1, x * 2 + 1);
      addch('-');
      addch('-');
    }

    for (y = 0; y < h; ++y) {
      move(y + 1, 0);
      addch('|');
      move(y + 1, w * 2 + 1);
      addch('|');
    }
  }

  void Labels() {
    if (game_over_flag) {
      static std::string label = " <<< Game over >>> ";
      move(h / 2, (w * 2) / 2 - label.size() / 2);
      printw("%s", label.c_str());
    }

    mvprintw(h + 2, 0, "%d %3.1f", iteration, progress);
  }

  void Redraw() {
    static constexpr char kSnakeHeadChar = '@';
    static constexpr char kSnakeBodyChar = '8';
    static constexpr char kFoodChar = '*';

    Clear();
    Border();
    Labels();

    // Draw food
    move(food_y + 1, food_x * 2 + 1);
    addch(kFoodChar);
    addch(kFoodChar);
  
    // Draw snake.
    for (int i = 0; i < snake.size(); ++i) {
      move(snake[i].y + 1, snake[i].x * 2 + 1);
      addch((i == (snake.size() - 1)) ? kSnakeHeadChar : kSnakeBodyChar);
      addch((i == (snake.size() - 1)) ? kSnakeHeadChar : kSnakeBodyChar);
    }

    refresh();
  }

  void Sleep() {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  //Model model;
  //ModelRenderer modelRenderer(model);
} // namespace

namespace snake {

Core::Core(int argc, char* argv[]) {
  initscr();
  refresh();
  noecho();
  cbreak();
  curs_set(0);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);

  //std::cout << "LINES: " << LINES << '\n'
  //          << "COLS: " << COLS << '\n';
}

int Core::Run() {
  Initialize();
  while (true) {
    Input();
    Update();
    Redraw();
    Sleep();
  }

  return 0;
}

} // namespace snake
