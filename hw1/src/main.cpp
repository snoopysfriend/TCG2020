/*
 *  \file main.cpp
 *  \brief game controller
 *  \author Maria Elsa (elsa)
 *  \course Theory of Computer Game (TCG)
 */

#include <iostream>
#include <fstream>
#include <getopt.h>
#include <string>

#include "sokoboru.hpp"
#include "getch.hpp"

#define CLEAR_SCREEN std::cout << "\033[H\033[J"
#define WRONG_ARG do{\
    std::cout << "Usage: sokoboru [-i file] [-o file] [-s start_stage]\n"; \
    std::cout << " -i filename: read the puzzles from \'file\'.\n"; \
    std::cout << " -o filename: if specified, write the solutions to \'file\'.\n"; \
    std::cout << " -s stage: if specified, start from Stage \'stage\'.\n"; \
    return (1); \
} while(0)

void print_legend() {
  std::cout << Color::BOLD << Color::FG_CYAN << "Legend:" << Color::CLEAR << "\n\n";
  std::cout << Item::WALL << "\tWall\n\n";
  std::cout << Item::PLAYER << "\tPlayer\n\n";
  std::cout << Item::BALL << "\tBall\n\n";
  std::cout << Item::BOX << "\tEmpty Box\n\n";
  std::cout << Item::BALL_BOX << "\tNon-Empty Box \n\n";
  std::cout << Item::FLOOR << "\tFloor\n\n";
  std::cout << "Use " << Color::BOLD << Color::FG_PINK << "wasd" << Color::CLEAR
            << " or " << Color::BOLD << Color::FG_PINK << "arrow keys"
            << Color::CLEAR << " to move.\n";
  std::cout << "Press " << Color::BOLD << Color::FG_PINK << "u" << Color::CLEAR
            << " to undo.\n\n";
}

Direction get_direction(char cmd) {
  switch (cmd) {
    case 'w': case 'A':
      return UP;
    case 's': case 'B':
      return DOWN;
    case 'd': case 'C':
      return RIGHT;
    case 'a': case 'D':
      return LEFT;
    default:
      return NONE;
  }
}

int main(int argc, char **argv) {
  Board b;
  std::string input_file, output_file;
  std::ifstream infile;
  std::ofstream outfile;
  int opt, start_stage = 1;

  while ((opt = getopt(argc, argv, "i:o:s:h")) != -1) {
    switch (opt) {
        case 'i':
            input_file = optarg;
            break;
        case 'o':
            output_file = optarg;
            break;
        case 's':
            start_stage = std::stoi(optarg);
            break;
        case 'h':
            WRONG_ARG;
            return 1;
        default:
            WRONG_ARG;
            return 1;
    }
  }

  if (input_file.empty()) {
    WRONG_ARG;
    return 1;
  }

  if (start_stage < 1) {
    std::cerr << "Error: start_stage must be greater than 1!\n";
    return 1;
  }

  infile.open(input_file);

  if (!infile.is_open()) {
    std::cerr << "Error: input file " << input_file << " not found!\n";
    return 1;
  }

  if (!output_file.empty()) {
    outfile.open(output_file);
    if (!outfile.is_open()) {
        std::cerr << "Error: output file " << output_file << " not found!\n";
        return 1;
    }
  }

  for (int stage = 1; b.init_board(infile); ++stage) {
    bool undo = false;
    if (stage < start_stage) continue;

    while (true) {
        undo = false;
        CLEAR_SCREEN;
        print_legend();
        std::cout << Color::BOLD << Color::FG_CYAN << "Stage #" << stage << Color::CLEAR << std::endl;
        std::cout << b.display_board();
        std::cout << "\nPenalty: " << b.get_penalty() << std::endl;
        std::cout << "\nMove Count: " << b.move_count() << std::endl;
        std::cout << "\nMoves: " << b.display_moves() << std::endl;

        if (b.is_terminal()) break;

        // read keystroke
        char cmd = getch();

        switch (cmd) {
            case 'u':
                undo = true;
                break;
            default:
                std::cout << "\a";
                break;
        }

        if (undo) {
            b.undo_move();
        } else {
            Direction dir = get_direction(cmd);
            if (dir != NONE) b.do_move(dir);
        }
    }
    std::cout << Color::BOLD << Color::FG_CYAN << "Stage Clear!\n" << Color::CLEAR;
    std::cout << Color::BOLD << Color::FG_CYAN << "Press ENTER to continue\n" << Color::CLEAR;
    getchar();

    if (outfile.is_open()) {
        outfile << b.move_count() << std::endl;
        outfile << b.display_moves() << std::endl;
    }
  }

  std::cout << Color::BOLD << Color::FG_CYAN << "Thanks for Playing!\n" << Color::CLEAR;

  return 0;
}
