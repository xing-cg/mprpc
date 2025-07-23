#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"
int main(int argc, char ** argv)
{
    // 整个程序启动以后，想使用mprpc框架来享受rpc服务调用，一定需要先调用框架的初始化函数（只初始化一次）
    MprpcApplication::Init(argc, argv);

    // 填写rpc方法的请求参数
    xcg::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    // 定义rpc方法的响应
    xcg::LoginResponse response;

    // 底层转到RpcChannel->CallMethod来做所有rpc方法调用的参数序列化和网络发送
    xcg::UserServiceRpc_Stub stub(new MprpcChannel());

    // 发起rpc方法的调用。
    stub.Login(nullptr, &request, &response, nullptr);
    
    // 一次rpc调用完成，读调用的结果
    if (response.result().errcode() == 0)
    {
        // 成功
        std::cout << "rpc login response success: " << response.success() << std::endl;
    }
    else
    {
        std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
    }
    return 0;
}