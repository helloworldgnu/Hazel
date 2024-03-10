//
// Created by baidu on 2023/9/2.
//

#ifndef SANBOX_CORE_H
#define SANBOX_CORE_H

#ifdef HZ_BUILD_DLL
    #define HAZEL_API __declspec(dllexport)
#else
    #define HAZEL_API __declspec(dllimport)
#endif

#endif //SANBOX_CORE_H