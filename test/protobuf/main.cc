#include "test.pb.h"
#include <iostream>
using namespace xcg;
int main()
{
    LoginResponse rsp;
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);
    rc->set_errmsg("登陆处理失败");
    GetFriendListsResponse rsp2;
    ResultCode *rc2 = rsp2.mutable_result();
    rc2->set_errcode(0);
    rc2->set_errmsg("成功");

    User * user1 = rsp2.add_friend_list();
    user1->set_name("zhang san");
    user1->set_age(20);
    user1->set_sex(User::MAN);
    User * user2 = rsp2.add_friend_list();
    user2->set_name("li si");
    user2->set_age(22);
    user2->set_sex(User::WOMAN);

    std::cout << rsp2.friend_list_size() << std::endl;

}
int main1()
{
    // 封装了login请求对象的数据
    LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123456");
    // 对象数据序列化 为 字符串
    std::string send_str;
    if(req.SerializeToString(&send_str))
    {
        std::cout << send_str << std::endl;
    }
    // 从一个字符串反序列化到login请求对象
    LoginRequest req2;
    if (req2.ParseFromString(send_str))
    {
        std::cout << req2.name() << std::endl;
        std::cout << req2.pwd() << std::endl;
    }

    return 0;
}