#include "rpcprovider.h"
#include "mprpcapplication.h"
#include <functional>
#include <google/protobuf/descriptor.h>
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
void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr& conn)
{

}
void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp timestamp)
{

}