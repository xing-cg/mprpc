#pragma once
// mprpc 框架的基础类，用于初始化框架
// 单例设计
class MprpcApplication
{
public:
    static void Init(int argc, char ** argv);
    static MprpcApplication& GetInstance();
private:
    MprpcApplication()
    {

    }
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;
};