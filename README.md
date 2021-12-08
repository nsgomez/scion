# Scion · a Maxis framework implementation

**Scion** is an implementation of the GZCOM/RZCOM framework used by SimCity 4 and The Sims 2. Its goal is to produce
binary output identical to the framework used in SimCity 4 Deluxe Edition: when compiled for Windows with the right
toolset and options, the assembly should match.

After the framework is finished, I'd like to use Scion to reverse-engineer SimCity 4 itself in the same vein as
other open-source game projects (e.g. OpenTTD, OpenRCT2).

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

Scion is licensed under the [GNU Lesser General Public License, version 2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html).

The LGPL allows you to dynamically link this project with proprietary software like the Maxis games that use this
framework; in other words, you can use it to write framework hooks for those non-free games. But if you make changes to
Scion itself, those changes must be shared under the LGPLv2.1 license.