#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"
#include "string.h"
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
    MprpcController controller;
    stub.Login(&controller, &request, &response, nullptr);
    
    if (controller.Failed()) // rpc调用过程中有异常，response没有填入数据
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    else // 一次rpc调用结束，返回了，读返回的response结果
    {
        if (response.result().errcode() == 0)
        {
            // 成功
            std::cout << "rpc login response success: " << response.success() << std::endl;
        }
        else
        {
            std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
        }
    }
    return 0;
}