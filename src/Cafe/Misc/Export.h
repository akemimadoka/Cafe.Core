#ifndef CAFE_CORE_MISC_EXPORT_H
#define CAFE_CORE_MISC_EXPORT_H

// From https://gcc.gnu.org/wiki/Visibility
#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef CAFE_BUILDING_DLL
#ifdef CAFE_BUILDING
#ifdef __GNUC__
#define CAFE_PUBLIC [[gnu::dllexport]]
#else
#define CAFE_PUBLIC __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define CAFE_PUBLIC [[gnu::dllimport]]
#else
#define CAFE_PUBLIC __declspec(dllimport)
#endif
#endif
#define CAFE_LOCAL
#else
#define CAFE_PUBLIC
#define CAFE_LOCAL
#endif
#else
#if __GNUC__ >= 4
#define CAFE_PUBLIC [[gnu::visibility("default")]]
#define CAFE_LOCAL [[gnu::visibility("hidden")]]
#else
#define CAFE_PUBLIC
#define CAFE_LOCAL
#endif
#endif

#endif
