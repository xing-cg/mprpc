#include "rpcprovider.h"
#include "mprpcapplication.h"
#include <functional>
#include <google/protobuf/descriptor.h>
#include "rpcheader.pb.h"
void RpcProvider::Run()
{
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserver_ip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserver_port").c_str());
    muduo::net::InetAddress address(ip, port);
    // 创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");
    // 绑定连接回调和消息读写回调方法 - 分离网络代码和业务代码
    // 需要给TcpServer提供一个返回值为void，参数有TcpConnectionPtr的函数
    // 这个函数我们在RpcProvider的成员方法提供
    // 实际上，onConnection到时候由muduo库进行调用
    // 调用的就是this对象，然后需要一个_1以预留TcpConnectionPtr& conn这个参数的位置
    // std::function<void (const TcpConnectionPtr&)>
    server.setConnectionCallback(std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));
    // std::function<void (const TcpConnectionPtr&, Buffer*, Timestamp)>
    server.setMessageCallback(std::bind(&RpcProvider::onMessage, this, 
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // 设置muduo库的线程数量
    server.setThreadNum(4);

    std::cout << "RpcProvider Start Service at IP: " << ip << ", port: " << port << std::endl;

    // 启动网络服务
    server.start();
    // 启动epoll_wait，以阻塞的方式等待远程的连接。
    // 如果有连接请求，则muduo库会回调onConnection
    // 如果有收发数据请求，则muduo库会回调onMessage
    m_eventLoop.loop();
}
void RpcProvider::NotifyService(::google::protobuf::Service * service)
{
    const google::protobuf::ServiceDescriptor * pserviceDesc = service->GetDescriptor();

    ServiceInfo service_info;
    service_info.m_service = service;
    int methodCnt = pserviceDesc->method_count();
    for(int i = 0; i < methodCnt; ++i)
    {
        // 该方法定义于ServiceDescriptor
        // const MethodDescriptor* method(int index) const; 
        // 返回的是一个MethodDescriptor*
        // 获取了服务对象指定下标的rpc服务方法的描述
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        std::cout << i << ". method_name: " << method_name << std::endl;
        service_info.m_methodMap.insert({method_name, pmethodDesc});
    }
    
    std::string service_name = pserviceDesc->name();
    std::cout << "service_name: " << service_name << std::endl;
    m_serviceMap.insert({service_name, service_info});
}
void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if (!conn->connected())
    {
        conn->shutdown();
    }
}
void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp timestamp)
{
    // 接收到的rpc调用请求的字符流
    // 包含了方法名、参数
    std::string recv_buf = buffer->retrieveAllAsString();
    // 从字符流中读取前4个字节的header_size（即按内存中原来的2进制读取）
    // 有相应的string方法：insert、copy，是按2进制写入、读取固定字节的数据，而不是直接把数字存为长度不一的字符串。
    uint32_t header_size = 0;
    // 从recv_buf的 0 位置开始，读取uint32_t 大小个字节，存入header_size。
    recv_buf.copy((char*)&header_size, sizeof(uint32_t), 0);
    // 根据header_size读取数据头的原始字符流
    // substr表示，略过recv_buf的前uint32_t 大小个字节，返回之后的header_size长度的字符串
    // rpc_header_str 目前保存了service_name method_name args_size
    std::string rpc_header_str = recv_buf.substr(sizeof(uint32_t), header_size);
    // 用rpcheader.pb.h生成的rpcHeader中的ParseFromString反序列化rpc_header_str
    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if (rpcHeader.ParseFromString(rpc_header_str))
    {
        // header反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else
    {
        // header反序列化失败
        std::cout << "rpc_header_str: " << rpc_header_str << "parse error!" << std::endl;
        return;
    }

    // 获取rpc方法参数的字符流数据
    std::string args_str = recv_buf.substr(sizeof(uint32_t) + header_size, args_size);

    // 打印调试信息
    std::cout << "==============================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "==============================" << std::endl;

    auto service_it = m_serviceMap.find(service_name);
    if (service_it == m_serviceMap.end())
    {
        std::cout << service_name << " not exist!" << std::endl;
        return;
    }
    auto method_it = service_it->second.m_methodMap.find(method_name);
    if (method_it == service_it->second.m_methodMap.end())
    {
        std::cout << method_name << " not exist in " << service_name << std::endl;
        return;
    }

    google::protobuf::Service *service = service_it->second.m_service;
    const google::protobuf::MethodDescriptor *method = method_it->second;

    // 生成rpc方法调用的请求request和响应response
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str))
    {
        std::cout << "request parse args_str error! args_str: " << args_str << std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    // 给下面的method方法的调用，绑定一个Closure的回调函数
    google::protobuf::Closure* done = google::protobuf::NewCallback
        <RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>
            (this, &RpcProvider::SendRpcResponse, conn, response);

    // 在框架上根据远端rpc请求，调用当前rpc节点上发布的方法
    service->CallMethod(method, nullptr, request, response, done);
}
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response)
{
    std::string response_str;
    // 将response序列化为字符流
    if (response->SerializeToString(&response_str))
    {
        // 序列化成功后，通过网络把rpc方法执行的结果发送回rpc的调用方
        conn->send(response_str);
    }
    else
    {
        std::cout << "Serialize response_str error!" << std::endl;
    }
    conn->shutdown(); // 短连接效果，由rpcprovider主动断开连接，让出服务器资源。
}
