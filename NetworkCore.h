#pragma once

// How to setup dll: https://learn.microsoft.com/en-us/cpp/build/walkthrough-creating-and-using-a-dynamic-link-library-cpp?view=msvc-170

#ifdef NETWORKCORE_EXPORTS
#define NETWORKCORE_API __declspec(dllexport)
#else // NETWORKCORE_EXPORTS
#define NETWORKCORE_API __declspec(dllimport)
#endif // NETWORKCORE_EXPORTS
