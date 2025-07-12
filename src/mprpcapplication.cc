#include "mprpcapplication.h"
// 类外实现静态方法，不用前面加static
void MprpcApplication::Init(int argc, char ** argv)
{

}
MprpcApplication& MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
}