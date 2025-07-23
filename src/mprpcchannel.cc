#include "mprpcchannel.h"
#include "rpcheader.pb.h"
#include "mprpcapplication.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <error.h>
// 约定好的包格式：（header_size)(service_name method_name args_size)(args)
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
    google::protobuf::RpcController *controller, const google::protobuf::Message *request,
    google::protobuf::Message *response, google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *sd = method->service();
    std::string service_name = sd->name();
    std::string method_name = method->name();
    
    // 序列化(args)，之后才能获取args的序列化字符串长度 args_size
    uint32_t args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        std::cout << "serialize request error!" << std::endl;
        return;
    }
    // 填写rpc的请求header
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);
    // 序列化（header_size)(service_name method_name args_size)
    uint32_t header_size = 0;
    std::string rpc_header_str;
    if (rpcHeader.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else
    {
        std::cout << "serialize request error!" << std::endl;
        return;
    }

    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char*)&header_size, 4));
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;

    // 打印调试信息
    std::cout << "==============================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "==============================" << std::endl;

    // 使用tcp编程，完成rpc方法的远程调用
    int cliendfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == cliendfd)
    {
        std::cout << "Create Socket Error: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    // 读取配置文件rpcserver的信息
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (-1 == inet_pton(AF_INET, ip.c_str(), &(server_addr.sin_addr.s_addr)))
    {
        std::cout << "inet_pton Error: " << errno << std::endl;
        close(cliendfd);
        exit(EXIT_FAILURE);
    }
    if (-1 == connect(cliendfd, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        std::cout << "connect Error: " << errno << std::endl;
        close(cliendfd);
        exit(EXIT_FAILURE);
    }
    if (-1 == send(cliendfd, send_rpc_str.c_str(), send_rpc_str.size(), 0))
    {
        std::cout << "send Error: " << errno << std::endl;
        close(cliendfd);
        exit(EXIT_FAILURE);
    }
    char recv_buff[1024] = {0};
    int recv_size = 0;
    if (-1 == (recv_size = recv(cliendfd, recv_buff, sizeof(recv_buff), 0)))
    {
        std::cout << "recv Error: " << errno << std::endl;
        close(cliendfd);
        exit(EXIT_FAILURE);
    }
    // 反序列化rpc调用的响应数据
    // std::string response_str(recv_buff, 0, recv_size); // 如此构造有问题，如果recv_buff中存在\0则断开。
    // if (!response->ParseFromString(response_str))
    if (!response->ParseFromArray(recv_buff, recv_size))
    {
        std::cout << "parse response_str error! response_str: " << recv_buff << std::endl;
        close(cliendfd);
        return;
    }
    close(cliendfd);
}