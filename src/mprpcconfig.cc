#include "mprpcconfig.h"
#include <iostream>
void MprpcConfig::Trim(std::string &str_buf)
{
    // 去掉字符串前面多余的空格
    int idx = str_buf.find_first_not_of(' ');
    if (idx > 0)
    {
        // 说明字符串前面有空格
        str_buf = str_buf.substr(idx, str_buf.size() - idx);
    }
    // 去掉字符串后面多余的空格
    idx = str_buf.find_last_not_of(' ');
    str_buf = str_buf.substr(0, idx + 1);
}
void MprpcConfig::LoadConfigFile(const char * config_file)
{
    FILE *pf = fopen(config_file, "r");
    if (nullptr == pf)
    {
        std::cout << config_file << " not exist!" << std::endl;
        exit(EXIT_FAILURE);
    }
    // 1. 注释
    // 2. 多余的空格
    // 3. 正确的配置项（以=分隔的）
    while (!feof(pf))
    {
        char buf[512] = {0};
        fgets(buf, 512, pf);
        std::string str_buf(buf);
        Trim(str_buf);

        // 判断 # 开头的注释 或者纯空行
        if (str_buf[0] == '#' || str_buf.empty())
        {
            continue;
        }
        int idx = str_buf.find('=');
        if (idx == -1 || idx == 0)
        {
            // 配置项不合法
            continue;
        }
        std::string key;
        std::string value;
        key = str_buf.substr(0, idx);
        // 去掉前后的空格
        Trim(key);


        // 去掉末尾的换行符
        int lineBreakIdx = str_buf.find('\n', idx + 1);
        value = str_buf.substr(idx + 1, lineBreakIdx - (idx + 1));
        // 去掉前后的空格
        Trim(value);

        m_configMap.insert({key, value});
    }
}
std::string MprpcConfig::Load(const std::string& key)
{
    auto it = m_configMap.find(key);
    if (it == m_configMap.end())
    {
        return "";
    }
    return it->second;
}