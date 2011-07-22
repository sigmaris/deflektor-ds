Building from source
--------------------
The game requires the [devkitPRO] libraries to compile - it will build under devkitARM r34, libnds 1.5.2, libfilesystem 0.9.9, libfat 1.0.10 and maxmod 1.0.6. You'll need to set the DEVKITPRO and DEVKITARM environment variables to the location of your devkitPRO install directory as detailed here: <http://devkitpro.org/wiki/Getting_Started/devkitARM>

 [devkitPRO]: http://www.devkitpro.org

The default target in the Makefile will build a self-contained .nds file which contains all the level data, using NitroFS. For this to work, it requires that your flash card's launcher supports the [argv protocol]. Most modern card firmware does. If you can't use NitroFS, you can build a version that uses plain FAT access to files on your flash card, by using the 'nonitro' target: `make nonitro` - but note that this requires copying the `data` folder out of `fsroot` onto your flash card's filesystem.

Playing
-------
If your flash card's launcher doesn't do this already, you need to DLDI patch the .nds file with the correct module for your type of flash card (GBAMP, Supercard, etc). 
If using the FAT version, the `data` folder from `fsroot` must be placed in the root directory of your flash card filesystem, while the .nds file can be run from anywhere.
If using the NitroFS version, you don't need to put the `data` folder on your flash card, but you need to use a loader which supports the argv protocol.

Credits
-------
Original game by Vortex Software, published by Gremlin Graphics.

Music by Ben Daglish, converted to module format by wire.

Graphics were drawn based on the Amiga version, gremlin sprites are from Lufia II
