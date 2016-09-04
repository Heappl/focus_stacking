// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FOCUSSTACKIMPL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FOCUSSTACKIMPL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef FOCUSSTACKIMPL_EXPORTS
#define FOCUSSTACKIMPL_API  extern "C" __declspec(dllexport)
#else
#define FOCUSSTACKIMPL_API  extern "C" __declspec(dllimport)
#endif

#include <cstdint>

/*
[DllImport("FocusStackImpl.dll", CallingConvention = CallingConvention.Cdecl)]
public static extern IntPtr createFocusStack(int width, int height);

[DllImport("FocusStackImpl.dll", CallingConvention = CallingConvention.Cdecl)]
public static extern int addImage(IntPtr ctx, IntPtr img);

[DllImport("FocusStackImpl.dll", CallingConvention = CallingConvention.Cdecl)]
public static extern int createDepthOfField(IntPtr ctx, IntPtr dest);

[DllImport("FocusStackImpl.dll", CallingConvention = CallingConvention.Cdecl)]
public static extern int createInFocusImg(IntPtr ctx, IntPtr dest);

[DllImport("FocusStackImpl.dll", CallingConvention = CallingConvention.Cdecl)]
public static extern int releaseFocusStack(IntPtr ctx);
*/

FOCUSSTACKIMPL_API void* createFocusStack(int32_t width, int32_t height);
FOCUSSTACKIMPL_API int32_t addImage(void* ctx, int* img);
FOCUSSTACKIMPL_API int32_t createDepthOfField(void* focusStack, int32_t* dest);
FOCUSSTACKIMPL_API int32_t createInFocusImg(void* focusStack, int32_t* dest);
FOCUSSTACKIMPL_API int32_t releaseFocusStack(void* focusStack);
