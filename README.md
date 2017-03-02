# Splines

Generating 3d shapes by drawing splines and using them for translational or rotational sweeping

## Roadmap

### Requierements

* ~~User decides whether to have a translational “T” or rotational sweep “R” (through console)~~

* ~~The user then marks control points in the window with the mouse (assume XZ plane, Y=0). The X and Z coordinates of each point should then relate to the window coordinates where the user has clicked.~~

* ~~User can then press “Enter” which computes and draws the spline.~~
    
    * ~~If the user selected translational sweep, then repeat steps 2-3 for the second set of control points resulting in two separate splines (this time assume XY plane, Z=0). The X and Y coordinates of each point should then relate to the window coordinates where the user has clicked.~~

* ~~Write the points of the resulting spline/s to an output file. The output file should have the same description format as Assignment #1.~~

* ~~Load the output file in Assignment #1 and generate smooth surfaces based on the input splines.~~

### Sub-requierements

* ~~Create a GLFW window of size 800x800 with double buffering support~~

* ~~The application should handle window resize events~~

* ~~Use an orthographic projection for simplicity~~

* Use the subdivision technique presented in class for drawing the spline

* Generate indices to draw lines (l)

## Usage

An example command:

    ./run.sh

### Controls

    left mouse click    marks control points in the window

    enter               computes and draws the spline based on the control points
    backspace           resets the application (removes all control points, interpolated points etc)

    rotate              arrows
    
    w                   move forward
    s                   move backward
    a                   move left
    d                   move right
    
    l                   display lines
    t                   display triangles
    p                   display points

## Setup

It was tested on Arch Linux.

### Arch Linux

Install these :

    sudo pacman -S glew glm glfw-x11 mesa-demos

Compile:

    make arch

### Linux (general)

Install the dependencies and compile:

    make linux

## Authors

Vsevolod (Seva) Ivanov - seva@tumahn.net
