#include <iostream>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
// 服务提供者
class UserService : public xcg::UserServiceRpc
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name: " << name << ", pwd: " << pwd << std::endl;
        return true;
    }
    // 重写基类UserServiceRpc的虚函数。这些方法都是框架直接调用的
    void Login(::google::protobuf::RpcController* controller,
                       const ::xcg::LoginRequest* request,
                       ::xcg::LoginResponse* response,
                       ::google::protobuf::Closure* done)
    {
        // 拿到了框架给上报的请求参数LoginRequest
        // 需要取出相应数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();
        // 取出数据后，调用本身已有的方法
        bool login_result = Login(name, pwd);
        // 填写响应消息
        xcg::ResultCode * result_code = response->mutable_result();
        result_code->set_errcode(0);
        result_code->set_errmsg("ok!");
        response->set_success(login_result);
        // 执行回调操作：由框架done->Run()处理消息的序列化、返回传输
        done->Run();
    }
};
int main(int argc, char ** argv)
{
    // 框架的初始化操作
    MprpcApplication::Init(argc, argv);
        
    // provider是一个rpc网络服务对象，负责发布服务到rpc节点上
    RpcProvider provider; 
    // 把UserService发布到rpc节点上
    provider.NotifyService(new UserService());
    
    // 启动一个rpc服务发布节点
    provider.Run();
    // 之后，进程进入阻塞状态，等待远程的rpc调用请求
}