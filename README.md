# Scion · a Maxis framework implementation

**Scion** is a reference implementation of the Gonzo/Rizzo framework used by Maxis. This framework was used by
games such as SimCity 3000, SimCity 4, The Sims 2, and Spore, but this project aims for functional partiy with
the version specifically used in SimCity 4 Deluxe Edition. As a stretch goal, it also tries (where practical)
to produce identical machine code, such that the assembly matches when compiled for Windows with the right
toolset and options.

This project is not affiliated with or endorsed by EA Games.

## Building

Scion currently only targets Windows. It should be buildable with any C++03-compliant compiler, but currently there
are only project files for Visual Studio .NET 2003. That appears to be the toolset used for building SimCity 4
Deluxe.

This project also uses [STLport](http://www.stlport.org/) instead of the compiler-provided STL. To build STLport,
open the Visual Studio command line and run:

```
cd src\stlport\src
nmake -f vc7.mak
nmake -f vc7.mak install
```

Once STLport is built, the framework solution can be built too.

## License

Scion is licensed under the [GNU Lesser General Public License, version 2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html)
or (at your option) any later version published by the Free Software Foundation.

The LGPL allows you to dynamically link this project with proprietary software, like retail Maxis games; in other words,
you can use it to write framework hooks for those non-free games. But if you make changes to Scion itself, those changes
must be shared under the LGPLv2.1 license or later.

## Progress

| **Component**                        | **Status**                           |
|--------------------------------------|--------------------------------------|
| Core framework                       | 🟨 Work in progress                  |
| Resource manager (GZResourceD)       | 🟨 Work in progress                  |
| Lua                                  | ❌ Not started                       |
| Graphics (GZGraphicD)                | ❌ Not started                       |
| UI primitives (GZWinD)               | ❌ Not started                       |
| Sound (GZSoundD)                     | ❌ Not started                       |
| Internet (GZWWWD)                    | ❌ Not started                       |
| Debug tools (GZToolsD)               | ❌ Not started                       |
| Gimex (GXCOM)                        | ❌ Not started                       |
