Wii U
=======
SDL port for the Nintendo Wii U

Credit to
* @rw-r-r-0644 and @QuarkTheAwesome for the initial Wii U port
* @GaryOderNichts

Building
--------
To build for the Wii U, make sure you have wut and wiiu-cmake installed and run:
```
   /opt/devkitpro/portlibs/wiiu/bin/powerpc-eabi-cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$DEVKITPRO/portlibs/wiiu
   cmake --build build
   cmake --install build
```


Notes
-----
* TODO
