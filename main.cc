#include <drogon/drogon.h>
#include <cstdlib>
int main()
{
    // Set HTTP listener address and port
    int port = 5555;
    const char *portEnv = std::getenv("PORT");
    if (portEnv)
    {
        port = std::atoi(portEnv);
    }
    drogon::app().addListener("0.0.0.0", port);
    // Load config file
    // drogon::app().loadConfigFile("../config.json");
    // drogon::app().loadConfigFile("../config.yaml");
    // Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();
    return 0;
}
