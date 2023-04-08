# akinator

Simple guesser based on binary tree data structure.

## Table of contents
* [Prerequisites](#prerequisites)
* [Installation and setup](#installation-and-setup)
* [Usage](#usage)

## Prerequisites
* `git`
* `make`
* `g++`

Program was built and tested on Linux 6.1.21-1-MANJARO.

## Installation and setup
1. Download all source files into one folder:
```
$ git clone https://github.com/princess-oregano/akinator
```
2. Open the subfolder with source files and build with Make:
```
$ cd akinator/
$ make
```
The program will build and run.

3. You can run `akinatorka` again from the folder, where it was built:
```
$ ./akinatorka
```
4. To remove all object files:
```
$ make clean
```
5. To remove all files built by `make`:
```
make distclean
```

## Usage
After you start the program, it will prompt you to enter a save file of
previous game. If you don't have one, enter any filename you like, and program
will create new save file.

> [NOTE] In case of new game the program will prompt you to enter one quality and
two objects to start the game.

Akinatorka has 3 game modes:
* `угадать`. It tries to guess object. In case of failure it will ask, what did
you mean, and save your answer. You can answer to questions with `да` or `нет`.
* `сравнить`. It compares two objects.
* `описать`. It decribes one object.

To leave program, enter `я ухожу` in any time of execution. 

> [NOTE] Be careful, sometimes akinatorka will get too playful to end the game. :)

