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
// GOTHICLIB_ZENGIN_PWF
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

#endif

// Include ZenGin
#ifdef GOTHICLIB_ZENGIN

#include "zen_base.h"
#include "zen_vdfs.h"
#include "zen_world.h"

#endif // GOTHICLIB_ZENGIN

#endif // !GOTHICLIB_DISABLE
