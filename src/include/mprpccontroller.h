#pragma once
#include <google/protobuf/service.h>
class MprpcController : public google::protobuf::RpcController
{
public:
    MprpcController();
    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string& reason);

    // 暂时空实现
    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure *callback);
private:
    // RPC方法执行过程中的状态
    bool m_failed;
    // RPC方法执行过程中的错误信息
    std::string m_errText;
};