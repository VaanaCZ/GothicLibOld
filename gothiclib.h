#pragma once

//
// Main macros:
// 
// GOTHICLIB_DISABLE
//     Disables the whole library
// 
// GOTHICLIB_ALL
//     Loads everything
// 
// GOTHICLIB_ZENGIN
//     Loads ZenGin libs
// 
// GOTHICLIB_GENOME
//     Loads Genome libs
// 
// Special macros:
// 
//

#ifndef GOTHICLIB_DISABLE

#include "gothiclib_base.h"

// Load everything
#ifdef GOTHICLIB_ALL

#undef GOTHICLIB_ZENGIN
#define GOTHICLIB_ZENGIN

#undef GOTHICLIB_GENOME
#define GOTHICLIB_GENOME

#undef GOTHICLIB_GENOME2
#define GOTHICLIB_GENOME2

#endif

// Include ZenGin
#ifdef GOTHICLIB_ZENGIN

#include "zen_base.h"
#include "zen_vdfs.h"
#include "zen_world.h"
#include "zen_script.h"

#endif // GOTHICLIB_ZENGIN

// Include Genome
#ifdef GOTHICLIB_GENOME

#include "ge_base.h"
#include "ge_world.h"

#endif // GOTHICLIB_GENOME

#ifdef GOTHICLIB_GENOME2

#include "ge2_base.h"
#include "ge2_world.h"

#endif // GOTHICLIB_GENOME

#endif // !GOTHICLIB_DISABLE
