#pragma once
#include "mprpcconfig.h"
// mprpc 框架的基础类，用于初始化框架
// 单例设计
class MprpcApplication
{
public:
    static void Init(int argc, char ** argv);
    static MprpcApplication& GetInstance();
    // 以便RpcProvider获得config对象
    static MprpcConfig& GetConfig();
private:
    static MprpcConfig m_config;
    MprpcApplication()
    {

    }
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;
};