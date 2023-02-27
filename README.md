**PROJECT DEPRECATED**

Originally, it had been my intention to create a simple library which could load, save and modify data of various types found in Piranha Bytes games (both ZenGin and Genome). Eventually however, the project's complexity spiraled out of control and became more of an engine system re-implementation than an actual data format library.
Therefore, the code is provided only for educational purposes and is not meant to be used in any real capacity.
For example, the library contains a fully functional VDFS implementation, texture loader, as well loading and saving code for world files of all engine versions (listed below) including compiled worlds.
I might return to this project one day and rewrite it as a much smaller header-only library.

**PROCEED WITH CAUTION**

.
.
.
.
.

# GothicLib

**Library for all things Piranha Bytes...**

A set of libraries intended for reading and writing data in the native formats of Piranha Bytes games.

So far there is no build process.
In order to use the library, include all the source files in your project and then #include the main source file

```cpp
#include "src/gothiclib.h"
```

Afterwards, specify which functionality you need using one of several macros

```
GOTHICLIB_DISABLE   - Disables the whole library
GOTHICLIB_ZENGIN    - Enables ZenGin functionality
GOTHICLIB_GENOME    - Enables Genome functionality (Gothic3 & Risen1)
GOTHICLIB_GENOME2   - Enables Genome functionality (Risen2)
GOTHICLIB_ALL       - Enables everything
```

C++17 or newer is required!

**!!!THIS README IS INCOMPLETE!!!**

**Table of contents** 
1. [ZenGin](#zengin)  
1.1. [Description](#zen_desc)  
1.2. [Development status](#zen_status)  
1.3. [Usage](#zen_usage)  
1.4. [File structure](#zen_structure)  
2. [Genome Engine](#genome)  
2.1. [Description](#ge_desc)  
2.2. [Development status](#ge_status)  
2.3. [Usage](#ge_usage)  
2.4. [File structure](#ge_structure)  
3. [CodeCreatures](#codecreatures)  
3.1. [Description](#cc_desc)  
4. [Space&Time](#space)  

<a name="zengin"></a>
## 1.ZenGin (Gothic + Gothic II) | 1996-2003

<a name="zen_desc"></a>
### 1.1. Description

**ZenGin**, formerly known as **zEngine**, is a video game engine co-developed by Mad Scientists and Piranha Bytes during the production of Gothic [2001]. Initial work on the engine started in late 1996 and ceased in 2003, as Piranha Bytes decided to abandon it in favour of their new in-house engine called **Genome**.

In order to allow for as authentic as possible loading and saving of the engine's data, GothicLib mirrors the object persistence system and class structure of the original engine. As a result, the classes exported by GothicLib are quite similar to the ones you would see while for example making a plugin using Union.

<a name="zen_status"></a>
### 1.2. Development status

This section includes the current progress on loading and saving the engine's file formats.

**ZenGin Archive**

|       | ASCII | ASCII_PROPS | BINARY | BIN_SAFE |
|-------|-------|-------------|--------|----------|
| Read  | ✔     | ✔           | ✔      | ✔         |
| Write | ✔     | ✖           | ✔      | ✖         |

**World archivation**

<table>
	<tr>
		<th></th>
		<th colspan=3>Loading</th>
		<th colspan=3>Saving</th>
	</tr>
	<tr>
		<th></th>
		<th>Uncompiled</th>
		<th>Compiled</th>
		<th>Savegame</th>
		<th>Uncompiled</th>
		<th>Compiled</th>
		<th>Savegame</th>
	</tr>
	<tr>
		<th>Gothic II: NotR</th>
		<th></th>
		<th></th>
		<th></th>
		<th></th>
		<th></th>
		<th></th>
	</tr>
	<tr>
		<td>2.6</td>
		<td>✔</td>
		<td>✔</td>
		<td>✖</td>
		<td>✔</td>
		<td>✔</td>
		<td>✖</td>
	</tr>
	<tr>
		<th>Gothic II</th>
		<th></th>
		<th></th>
		<th></th>
		<th></th>
		<th></th>
		<th></th>
	</tr>
	<tr>
		<td>1.30</td>
		<td>✔</td>
		<td>✔</td>
		<td>✖</td>
		<td>✔</td>
		<td>✔</td>
		<td>✖</td>
	</tr>
	<tr>
		<th>Gothic Sequel</th>
		<th></th>
		<th></th>
		<th></th>
		<th></th>
		<th></th>
		<th></th>
	</tr>
	<tr>
		<td>1.12f</td>
		<td>✔</td>
		<td>✔</td>
		<td>✖</td>
		<td>✔</td>
		<td>✔</td>
		<td>✖</td>
	</tr>
	<tr>
		<th>Gothic</th>
		<th></th>
		<th></th>
		<th></th>
		<th></th>
		<th></th>
		<th></th>
	</tr>
	<tr>
		<td>1.08k_mod</td>
		<td>✔</td>
		<td>✔</td>
		<td>✖</td>
		<td>✔</td>
		<td>✔</td>
		<td>✖</td>
	</tr>
	<tr>
		<td>1.01e_christmas_edition</td>
		<td>✔</td>
		<td>✔</td>
		<td>✖</td>
		<td>✔</td>
		<td>✔</td>
		<td>✖</td>
	</tr>
	<tr>
		<td>1.00b</td>
		<td>✖</td>
		<td>✖</td>
		<td>✖</td>
		<td>✖</td>
		<td>✖</td>
		<td>✖</td>
	</tr>
	<tr>
		<td>0.94k</td>
		<td>✔</td>
		<td>✔</td>
		<td>✖</td>
		<td>✔</td>
		<td>✔</td>
		<td>✖</td>
	</tr>
	<tr>
		<td>0.64b</td>
		<td>✔</td>
		<td>✔</td>
		<td>-</td>
		<td>✔</td>
		<td>✔</td>
		<td>-</td>
	</tr>
	<tr>
		<td>0.56c</td>
		<td>✔</td>
		<td>✔</td>
		<td>-</td>
		<td>✔</td>
		<td>✔</td>
		<td>-</td>
	</tr>
</table>

<a name="zen_usage"></a>
### 1.3. Usage

This section includes several examples of how to properly utilise GothicLib.

**World loading**

```cpp
#define GOTHICLIB_ZENGIN
#include "src/gothiclib.h"

using namespace GothicLib::ZenGin;

oCWorld world;

if (!world.LoadWorld(file, GAME_GOTHIC1))
{
	// Error
	// Use Log::SetCallback
}
```

<a name="zen_structure"></a>
### 1.4. File structure
```
zen_vdfs.h - Full implementation of VDFS.
zen_base.h - Base of the object persistence system.
  \ zen_world.h - All classes required for reading world formats.
  \ zen_visual.h - All classes required for reading mesh formats.
```

<a name="genome"></a>
## 2. Genome Engine (Gothic 3 + Risen) | 2003-present

<a name="ge_desc"></a>
### 2.1. Description

**Genome** is a video game engine developed by Piranha Bytes for the game Gothic 3 [2006] and later used for their subsequent series Risen and ELEX. The engine started development during 2003 and a heavily revised version of it is still in use today.

Due to heavy changes between the engine's revisions, GothicLib only focuses on supporting formats from Gothic 3 [2006] and Risen [2009]. While newer versions of **Genome** still share most of the same overall concepts, they include major changes in the elemental systems of the engine, which require them to be handled separately in order to maintain clarity. 

In order to allow for as close loading and saving of data to the original engine as possible, the library mirrors the object persistence system and class structure of the engine. GothicLib is built according to the specification of Risen [2009], however, it is adjusted to allow for handling of Gothic 3 [2006] data as well.

<a name="ge_status"></a>
### 2.2. Development status

**TODO**

<a name="ge_usage"></a>
### 2.3. Usage

This section includes several examples of how to properly utilise GothicLib.

**World loading**

```cpp
#define GOTHICLIB_GENOME
#include "src/gothiclib.h"

using namespace GothicLib::Genome;

gCWorld world;

if (!world.Load(file, GAME_GOTHIC3))
{
	// Error
	// Use Log::SetCallback
}
```

<a name="ge_structure"></a>
### 2.4. File structure
```
ge_base - Base of the object persistence system.
  \ ge_world.h - Classes required for reading Gothic 3 and Risen 1 world formats.
```

<a name="codecreatures"></a>
## 3. CodeCreatures (Zerberus + Unplugged) | 1999-2002

<a name="cc_desc"></a>
### 3.1. Description

**CodeCreatures**, internally codenamed **Golem Engine**, is a video game engine developed by CodeCult, a former subsidiary of Piranha Bytes. The engine, which first started development around 1999, was targeted primarily at 3rd party developers, as its main goal was to offer a general purpose "Game Development System".

Piranha Bytes had plans to utilise **CodeCreatures** in its own titles, most notably in Project "Zerberus" [1999] and Project "Unplugged" [2001]. Sadly, the engine was never finished and the studio was officially dissolved around 2008 after its ownership had been transferred from Piranha Bytes to a new parent company around in early 2002, as a result of the Phenomedia financial scandal.

Despite, the fact the engine was never completed, bits and pieces of it can be found in both **ZenGin** and **Genome**. In fact, **CodeCreatures** and the **Genome Engine** share the same naming convention of source files, starting with the prefix "ge_" and the same integer types GEU8, GEU16, GEU32, etc.


**!!!TODO!!!**

```
cc_base.h - 
```

<a name="space"></a>
## 3. Space&Time (Finster) | 1995-1996
As a fun bonus, GothicLib also includes a small header-only library which allows the user to load data from the 1996 demo named Finster.
```
space.h - 
```