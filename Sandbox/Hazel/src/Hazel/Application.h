#ifndef SANBOX_APPLICATION_H
#define SANBOX_APPLICATION_H

namespace Hazel{
class Application {
public:
    Application();
    virtual ~Application();
    void Run();
};
    Application* CreateApplication();
}



#endif //SANBOX_APPLICATION_H