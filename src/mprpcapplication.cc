#include "mprpcapplication.h"
#include <iostream>
#include <unistd.h>
MprpcConfig MprpcApplication::m_config;
void ShowArgsHelp()
{
    std::cout << "format: commad -i <configfile>" << std::endl;
}
MprpcApplication& MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
}
MprpcConfig& MprpcApplication::GetConfig()
{
    return m_config;
}
// 类外实现静态方法，不用前面加static
void MprpcApplication::Init(int argc, char ** argv)
{
    if (argc < 2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }
    int c = 0;
    std::string config_file;
    while((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c)
        {
        case 'i':
            // 获取到配置文件的名字
            config_file = optarg;
            break;
        case '?':
            // std::cout << "invalid args!" << std::endl;
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            // std::cout << "need <configfile>!" << std::endl;
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }
    // 开始加载配置文件 rpcserver_ip rpcserver_port zookeeper_ip zookeeper_port
    m_config.LoadConfigFile(config_file.c_str());

    std::cout << "rpcserver_ip: " << m_config.Load("rpcserver_ip") << std::endl;
    std::cout << "rpcserver_port: " << m_config.Load("rpcserver_port") << std::endl;
    std::cout << "zookeeper_ip: " << m_config.Load("zookeeper_ip") << std::endl;
    std::cout << "zookeeper_port: " << m_config.Load("zookeeper_port") << std::endl;

}
