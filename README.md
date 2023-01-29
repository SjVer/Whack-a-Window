# Whack 'a Window

This is my intake assignment for Creative Media & Game Technologies at Breda University.

## Gameplay

In *Whack 'a Window*, the goal is to collect as many stars as possible within 60 seconds by hitting them with a bouncing ball. The twist, however, is that the player has no actual control over the ball! Instead, the game window can be dragged around used to bounce the ball. The ball is confined by the game window's borders, but the stars are not. To find and gather them the player must drag the game window all over the monitor and bounce the ball in the right direction!

## Controls

Control the bouncing ball by dragging the window around. The ball will bounce off the window's borders. Any other controls (such as the exit key) are clearly explained in-game.

## Additional information

The files `templace.cpp`, `template.h`, `surface.cpp` and `surface.h` are adapted from the given template and are left mostly untouched. The `template` files have been slightly edited for both additional game functionality and Linux compatibility (as Linux was used during development). The file `surface.cpp` has as only a few extra lines in the function `Sprite::DrawScaled`. For explanation see the implementation of said function.

## References/Credits

The use of pre-existing code (apart from the changes mentioned above) is always denoted by a comment and a reference to its source. The assets have their licences or readme's included.

## Building

The game should build fine on Windows using Visual Studio. The resulting executable should be located in the `bin/` directory.

On Linux, a library or two might need to be installed first. Then, simply run `make whack-a-window`. The resulting binary should be located in the `bin/` directory.

If any text is rendered incorrectly, if the game complains about missing files or if a segfault occurs try executing `assets/converted_font/convert.py` from the base directory. The game itself should also be run from the base directory.
