# renpy-vita
A Native Ren'Py Visual Novel Port for the PSVita

## Based on [renpy-switch](https://github.com/uyjulian/renpy-switch/) by uyjulian (great work)

### Yeah. It's finally here
About a year into the making (totally not because I can get a bit lazy), Ren'Py Vita is finally out.<br>
A ton of frustration, troubleshooting, and long nights went into getting this to work. From cuevavirus making the base<br>
of the Python 2.7 port, to GrapheneCt making PVR_PSP2, to isage working on SDL additions. And we can finally release<br>
this gigantic project to you guys. Hope you like it, literally all I wanted to do was play the REAL DDLC on the Vita<br>
        -SonicMastr
        
## Known Issues
- Load times can be pretty long and some image loading my cause hitches. This is sadly not something I can fix in<br>
as it is a Vita hardware limitation.

- Video playback will not work

- Quitting will crash (But I kinda left that in. Doesn't do anything bad. I promise)

- Some games will run into memory issues. I will continue to tweak memory usage as necessary, but the Vita is pretty<br>
sparse in memory, so I may not be able to pull a rabbit out of my hat when it comes to full compatibility.



### Build Instructions 

#### Pre-Req: 
Linux (or WSL1/2)<br>
Cython<br> 
Python 2.7.18<br>
Git<br>
Clone this repo<br>
Grab `PSVita_Release.zip` and `vitasdk_stubs.zip` from [PVR_PSP2 releases](https://github.com/GrapheneCt/PVR_PSP2/releases)<br>
Clone [SDL](https://github.com/libsdl-org/SDL)<br>
Clone [Python Future](https://github.com/PythonCharmers/python-future)

#### Steps:
1. Copy the contents of `vitasdk_stubs.zip` into your VitaSDK lib directory
2. Copy all of the files in [include](https://github.com/GrapheneCt/PVR_PSP2/tree/main/include) into your VitaSDK include directory
3. Copy files WITHOUT underscores in `python-future/src` to the Python2.7 folder in your VitaSDK lib directory.<br>(This will be removed in the future. Heh)
4. Navigate to the SDL directory and run<br>`cmake -S. -Bbuild -DVIDEO_VITA_PVR=ON -DCMAKE_TOOLCHAIN_FILE=${VITASDK}/share/vita.toolchain.cmake -DCMAKE_BUILD_TYPE=Release`
5. Once complete, navigate to the newly create build folder and run `make -j4 install`
6. Navigate to the `renpy-vita` folder and run `git submodule init` and then `git submodule update`
6. Navigate to the `renpy-vita/renpy/module` folder and run `RENPY_DEPS_INSTALL=$VITASDK/arm-vita-eabi RENPY_STATIC=1 python2 setup.py`
7. Navigate to `renpy-vita/pygame-sdl2` and run `PYGAME_SDL2_STATIC=1 python setup.py`
8. Navigate to `renpy-vita/renpy-vita` and run `./link_compile_pylibs.bash` and then `./link_sources.bash` (may need to chmod)
9. Now run `cmake -Bbuild -DCMAKE_TOOLCHAIN_FILE=${VITASDK}/share/vita.toolchain.cmake -DCMAKE_BUILD_TYPE=Release`
10. Navigate to the build directory and run `make -j4`
11. And now you're all done

## Ren'Py Vita Ports
[Doki Doki Literature Club Vita](https://github.com/SonicMastr/Doki-Doki-Literature-Club-Vita)

