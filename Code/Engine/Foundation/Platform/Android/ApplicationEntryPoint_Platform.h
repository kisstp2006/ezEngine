#pragma once

/// \file
#include <Foundation/Application/Application.h>
#include <Foundation/Platform/Android/Utils/AndroidUtils.h>

class ezApplication;

extern EZ_FOUNDATION_DLL void ezAndroidRun(struct android_app* pAndroidApp, ezApplication* pApp);

namespace ezApplicationDetails
{
  template <typename AppClass, typename... Args>
  void EntryFunc(struct android_app* pAndroidApp, Args&&... arguments)
  {
    alignas(alignof(AppClass)) static char appBuffer[sizeof(AppClass)]; // Not on the stack to cope with smaller stacks.
    ezAndroidUtils::SetAndroidApp(pAndroidApp);
    AppClass* pApp = new (appBuffer) AppClass(std::forward<Args>(arguments)...);

    ezAndroidRun(pAndroidApp, pApp);

    pApp->~AppClass();
    memset(pApp, 0, sizeof(AppClass));
  }
} // namespace ezApplicationDetails

/// \brief This macro allows for easy creation of application entry points (since they can't be placed in DLLs)
///
/// Just use the macro in a cpp file of your application and supply your app class (must be derived from ezApplication).
/// The additional (optional) parameters are passed to the constructor of your app class.
#define EZ_APPLICATION_ENTRY_POINT(AppClass, ...)                                                                        \
  alignas(alignof(AppClass)) static char appBuffer[sizeof(AppClass)]; /* Not on the stack to cope with smaller stacks */ \
  EZ_APPLICATION_ENTRY_POINT_CODE_INJECTION                                                                              \
  extern "C" void android_main(struct android_app* app)                                                                  \
  {                                                                                                                      \
    ::ezApplicationDetails::EntryFunc<AppClass>(app, ##__VA_ARGS__);                                                     \
  }