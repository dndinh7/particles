# particles

## Implements particle demos (GIFs at the end of the page are better quality)

https://user-images.githubusercontent.com/72237791/230537535-48745729-8680-4223-8499-d2bcdd9d7255.mp4


## How to build

*Windows*

Open git bash to the directory containing this repository.

```
particles $ mkdir build
particles $ cd build
particles/build $ cmake ..
particles/build $ start particles.sln
```

Your solution file should contain four projects.
To run from the git bash command shell, 

```
particles/build $ ../bin/Debug/billboard-axis.exe
particles/build $ ../bin/Debug/explosion.exe
particles/build $ ../bin/Debug/sparkle-trail.exe
particles/build $ ../bin/Debug/sparkles.exe
```

*macOS*

Open terminal to the directory containing this repository.

```
particles $ mkdir build
particles $ cd build
particles/build $ cmake ..
particles/build $ make
```

To run each program from build, you would type

```
particles/build $ ../bin/billboard-axis
particles/build $ ../bin/sparkles
particles/build $ ../bin/sparkle-trail
particles/build $ ../bin/explosion
```

## Demo of basic features

### Star Trail

![star-trail](https://user-images.githubusercontent.com/72237791/229005716-3acdfe63-bd31-4100-ac3f-6365e650cf5d.gif)



### Explosion

![explosions](https://user-images.githubusercontent.com/72237791/229006284-f07c0072-e8a2-4f70-9899-6b7d02e8039d.gif)



### Billboard One-Axis Rotation

![billboard](https://user-images.githubusercontent.com/72237791/229004846-0c700c1c-7cc5-453a-ba1a-eab054a7a7d3.gif)



## Unique features 

The demo shows particles traversing a flow field that was created using Perlin noise. The color of the particles is based
on their position on the grid, creating a gradient. The user can also move the player/camera using WASD and look around
holding CTRL and using the mouse.

As well, the user can press 'X', 'Y', or 'Z' to make the flow field 'wave' by offsetting noise parameters.

![elongated](https://user-images.githubusercontent.com/72237791/230537623-a146549a-876c-4327-8ab3-0fa76f181b02.gif)


![normal](https://user-images.githubusercontent.com/72237791/230537808-12decc09-0053-4b9b-b226-f61d1366efb7.gif)



