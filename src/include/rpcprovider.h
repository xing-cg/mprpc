#pragma once
#include "google/protobuf/service.h"
// 框架提供的专门发布RPC服务的网络对象类
class RpcProvider
{
public:
    // 这里是框架提供给外部使用的，可以发布Service
    void NotifyService(::google::protobuf::Service * service);
    // 启动RPC服务节点
    void Run();
};