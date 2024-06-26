#pragma once

#ifndef SANBOX_ENTRYPOINT_H
#define SANBOX_ENTRYPOINT_H

#include "Debugger/Instrumentor.h"

extern Hazel::Application* Hazel::CreateApplication();

int main(int argc, char** argv) {
    Hazel::Log::Init();

    HZ_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Startup.json");
    auto app = Hazel::CreateApplication();
    HZ_PROFILE_END_SESSION();

    HZ_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");
    app->Run();
    HZ_PROFILE_END_SESSION();

    HZ_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Shutdown.json");
    delete app;
    HZ_PROFILE_END_SESSION();
}

#endif //SANBOX_ENTRYPOINT_H
