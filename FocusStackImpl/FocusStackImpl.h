// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FOCUSSTACKIMPL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FOCUSSTACKIMPL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef FOCUSSTACKIMPL_EXPORTS
#define FOCUSSTACKIMPL_API __declspec(dllexport)
#else
#define FOCUSSTACKIMPL_API __declspec(dllimport)
#endif

// This class is exported from the FocusStackImpl.dll
class FOCUSSTACKIMPL_API CFocusStackImpl {
public:
	CFocusStackImpl(void);
	// TODO: add your methods here.
};

extern FOCUSSTACKIMPL_API int nFocusStackImpl;

FOCUSSTACKIMPL_API int fnFocusStackImpl(void);
