# GothicLib
**Library for all things Piranha Bytes...**

A set of libraries intended for reading and writing data in the native formats of Piranha Bytes games.

**Table of contents** 
1. [ZenGin](#zengin)
1.1. [Description](#zen_desc)
1.2. [Status](#zen_desc)
1.3. [Usage](#zen_usage)
1.4. [File structure](#zen_structure)
2. [Genome Engine](#genome)
3. [CodeCreatures](#codecreatures)
4. [Space&Time](#space)

<a name="zengin"></a>
## 1.ZenGin (Gothic + Gothic II) | 1996-2003

<a name="zen_desc"></a>
### 1.1. Description

<a name="zen_desc"></a>
### 1.2. Status

**ZenGin Archive**

|       | ASCII | ASCII_PROPS | BINARY | BIN_SAFE |
|-------|-------|-------------|--------|----------|
| Read  | ✔     | ✔           | ✔      | ✖         |
| Write | ✖     | ✖           | ✖      | ✖         |

**World archivation**

<table>
	<tr>
		<th></th>
		<th colspan=2>Loading</th>
		<th colspan=2>Saving</th>
	</tr>
	<tr>
		<th></th>
		<th>Uncompiled</th>
		<th>Compiled</th>
		<th>Uncompiled</th>
		<th>Compiled</th>
	</tr>
	<tr>
		<th>Gothic II: NotR</th>
		<th></th>
		<th></th>
		<th></th>
		<th></th>
	</tr>
	<tr>
		<td>2.6</td>
		<td>✖</td>
		<td>✖</td>
		<td>✖</td>
		<td>✖</td>
	</tr>
	<tr>
		<th>Gothic II</th>
		<th></th>
		<th></th>
		<th></th>
		<th></th>
	</tr>
	<tr>
		<td>1.30</td>
		<td>✖</td>
		<td>✖</td>
		<td>✖</td>
		<td>✖</td>
	</tr>
	<tr>
		<th>Gothic Sequel</th>
		<th></th>
		<th></th>
		<th></th>
		<th></th>
	</tr>
	<tr>
		<td>1.12f</td>
		<td>✖</td>
		<td>✖</td>
		<td>✖</td>
		<td>✖</td>
	</tr>
	<tr>
		<th>Gothic</th>
		<th></th>
		<th></th>
		<th></th>
		<th></th>
	</tr>
	<tr>
		<td>1.08k_mod</td>
		<td>✔</td>
		<td>✖</td>
		<td>✖</td>
		<td>✖</td>
	</tr>
	<tr>
		<td>1.01e_christmas_edition</td>
		<td>✔</td>
		<td>✖</td>
		<td>✖</td>
		<td>✖</td>
	</tr>
	<tr>
		<td>1.00b</td>
		<td>✖</td>
		<td>✖</td>
		<td>✖</td>
		<td>✖</td>
	</tr>
	<tr>
		<td>0.94k</td>
		<td>✔</td>
		<td>✖</td>
		<td>✖</td>
		<td>✖</td>
	</tr>
	<tr>
		<td>0.64b</td>
		<td>✔</td>
		<td>✖</td>
		<td>✖</td>
		<td>✖</td>
	</tr>
	<tr>
		<td>0.56c</td>
		<td>✔</td>
		<td>✖</td>
		<td>✖</td>
		<td>✖</td>
	</tr>
</table>

<a name="zen_usage"></a>
### 1.3. Usage


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


### 1.1. Description

### 1.1. File structure
```
ge_base - Base of the object persistence system.
  \ ge_world.h - Clasess required for reading Gothic 3 and Risen 1 world formats.
```

<a name="codecreatures"></a>
## 3. CodeCreatures (Zerberus + Unplugged) | 1999-2002

```
cc_base.h - Base of the object persistence system.
```

<a name="space"></a>
## 3. Space&Time (Finster) | 1995-1996
As a fun bonus, GothicLib also includes a small header-only library which allows the user to load data from the 1996 demo named Finster.
```
space.h - 
```