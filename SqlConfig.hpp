#pragma once

#include "SimpleIni.h"
#include "log.hpp"
#include <string>

class SqlConfig
{
public:
    bool loadConfigFile(std::string filename = "../mysql.ini")
    {
        CSimpleIni ini;
        SI_Error rc;
        rc = ini.LoadFile(filename.c_str());
        if (0 > rc)
        {
            LOG_ERROR << "加载 {} ini 文件失败！" << filename;
            return false;
        }

        CSimpleIniA::TNamesDepend sections;
        ini.GetAllSections(sections);

        for (const auto &it : sections)
        {
            const CSimpleIniA::TKeyVal *pKeyVal = ini.GetSection(it.pItem);

            if (nullptr != pKeyVal)
                for (const auto &it : *pKeyVal)
                    m_.emplace(it.first.pItem, it.second);
        }

        return true;
    }

    // mysql的连接信息
    std::string ip() const { return m_.at("ip"); }
    unsigned int port() const { return stoul(m_.at("port")); }
    std::string username() const { return m_.at("username"); }
    std::string password() const { return m_.at("password"); }
    std::string dbname() const { return m_.at("dbname"); }

    // mysql的连接配置项
    int initSize() const { return stoi(m_.at("initSize")); }
    int maxSize() const { return stoi(m_.at("maxSize")); }
    int maxIdleTime() const { return stoi(m_.at("maxIdleTime")); }
    int connectionTimeOut() const { return stoi(m_.at("connectionTimeOut")); }

public:
    std::map<std::string, std::string> m_;
};