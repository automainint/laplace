/*  laplace/platform/win32/wgl.h
 *
 *      Windows OpenGL declarations.
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_platform_win32_wgl_h
#define laplace_platform_win32_wgl_h

#include "../opengl.h"
#include "win.predef.h"

namespace laplace::win32 {
  auto gl_init() -> bool;
  auto gl_preload() -> bool;
  void gl_cleanup();
  auto get_proc_address(const char *name) -> gl::ptr_function;

  enum _hwgl_enum : uint32_t {
    WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB = 0x0002,
    WGL_CONTEXT_MAJOR_VERSION_ARB          = 0x2091,
    WGL_CONTEXT_MINOR_VERSION_ARB          = 0x2092,
    WGL_CONTEXT_FLAGS_ARB                  = 0x2094
  };

  static constexpr auto opengl_path = L"opengl32.dll";

  extern HMODULE opengl_dll;

  using pfn_wglCopyContext   = BOOL(__stdcall *)(HGLRC    hSrcRC,
                                               HGLRC    hDstRC,
                                               uint32_t mask);
  using pfn_wglCreateContext = HGLRC(__stdcall *)(HDC hDC);
  using pfn_wglCreateLayerContext =
      HGLRC(__stdcall *)(HDC hDC, int32_t iLayerPlane);
  using pfn_wglDeleteContext     = BOOL(__stdcall *)(HGLRC hRC);
  using pfn_wglGetCurrentContext = HGLRC(__stdcall *)();
  using pfn_wglGetCurrentDC      = HDC(__stdcall *)();
  using pfn_wglGetProcAddress =
      laplace::gl::ptr_function(__stdcall *)(const char *);
  using pfn_wglMakeCurrent = BOOL(__stdcall *)(HDC hDC, HGLRC hRC);
  using pfn_wglShareLists = BOOL(__stdcall *)(HGLRC hRC1, HGLRC hRC2);
  using pfn_wglUseFontBitmapsA  = BOOL(__stdcall *)(HDC      hDC,
                                                   uint32_t first,
                                                   uint32_t count,
                                                   uint32_t listBase);
  using pfn_wglUseFontBitmapsW  = BOOL(__stdcall *)(HDC      hDC,
                                                   uint32_t first,
                                                   uint32_t count,
                                                   uint32_t listBase);
  using pfn_wglUseFontOutlinesA = BOOL(__stdcall *)(
      HDC hDC, uint32_t first, uint32_t count, uint32_t listBase,
      float deviation, float extrusion, int32_t format,
      LPGLYPHMETRICSFLOAT lpgmf);
  using pfn_wglUseFontOutlinesW = BOOL(__stdcall *)(
      HDC hDC, uint32_t first, uint32_t count, uint32_t listBase,
      float deviation, float extrusion, int32_t format,
      LPGLYPHMETRICSFLOAT lpgmf);
  using pfn_wglDescribeLayerPlane = BOOL(__stdcall *)(
      HDC hDC, int32_t iPixelFormat, int32_t iLayerPlane,
      uint32_t nBytes, LPLAYERPLANEDESCRIPTOR plpd);
  using pfn_wglSetLayerPaletteEntries = int32_t(__stdcall *)(
      HDC hDC, int32_t iLayerPlane, int32_t iStart, int32_t cEntries,
      const uint32_t *pcr);
  using pfn_wglGetLayerPaletteEntries = int32_t(__stdcall *)(
      HDC hDC, int32_t iLayerPlane, int32_t iStart, int32_t cEntries,
      uint32_t *pcr);
  using pfn_wglRealizeLayerPalette =
      BOOL(__stdcall *)(HDC hDC, int32_t iLayerPlane, bool bRealize);
  using pfn_wglSwapLayerBuffers = BOOL(__stdcall *)(HDC      hDC,
                                                    uint32_t fuPlanes);
  using pfn_wglCreateContextAttribsARB = HGLRC(__stdcall *)(
      HDC hDC, HGLRC hShareContext, const int32_t *attribList);

  extern pfn_wglCopyContext             wglCopyContext;
  extern pfn_wglCreateContext           wglCreateContext;
  extern pfn_wglCreateLayerContext      wglCreateLayerContext;
  extern pfn_wglDeleteContext           wglDeleteContext;
  extern pfn_wglGetCurrentContext       wglGetCurrentContext;
  extern pfn_wglGetCurrentDC            wglGetCurrentDC;
  extern pfn_wglGetProcAddress          wglGetProcAddress;
  extern pfn_wglMakeCurrent             wglMakeCurrent;
  extern pfn_wglShareLists              wglShareLists;
  extern pfn_wglUseFontBitmapsA         wglUseFontBitmapsA;
  extern pfn_wglUseFontBitmapsW         wglUseFontBitmapsW;
  extern pfn_wglUseFontOutlinesA        wglUseFontOutlinesA;
  extern pfn_wglUseFontOutlinesW        wglUseFontOutlinesW;
  extern pfn_wglDescribeLayerPlane      wglDescribeLayerPlane;
  extern pfn_wglSetLayerPaletteEntries  wglSetLayerPaletteEntries;
  extern pfn_wglGetLayerPaletteEntries  wglGetLayerPaletteEntries;
  extern pfn_wglRealizeLayerPalette     wglRealizeLayerPalette;
  extern pfn_wglSwapLayerBuffers        wglSwapLayerBuffers;
  extern pfn_wglCreateContextAttribsARB wglCreateContextAttribsARB;
}

#endif
