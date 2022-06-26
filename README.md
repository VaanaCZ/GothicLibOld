# GothicLib
**Libraries for all things Piranha Bytes...**

A set of libraries intended for reading and writing data in the native formats of Piranha Bytes games.

**Table of contents** 
1. [ZenGin](#zengine)  
1.1. [Description](#zen_desc)  
2. [Genome Engine](#genome)  
3. [CodeCreatures](#codecreatures)  
4. [Space&Time](#space)  

<a name="zengine"></a>
## 1.ZenGin (Gothic + Gothic II) | 1996-2003

<a name="zen_desc"></a>
### 1.1. Description

### 1.1. File structure
```
zen_vdfs.h - Full implementation of VDFS.
zen_base.h - Base of the object persistence system.
  \ zen_world.h - All classes required for reading world and mesh formats.
```

<a name="genome"></a>
## 2. Genome Engine (Gothic 3 + Risen) | 2003-present


### 1.1. Description

### 1.1. File structure
```
ge_fs.h - Implentation of the Genome virtual filesystem. 
  \ ge_g3_base.h - Base of the Gothic 3 object persistence system.
  \ ge_r1_base.h - Base of the Risen 1 object persistence system.
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