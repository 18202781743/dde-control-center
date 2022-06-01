/*
* Copyright (C) 2019 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     guoyao <guoyao@uniontech.com>

* Maintainer: guoyao <guoyao@uniontech.com>

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "namespace.h"
#include <QObject>

namespace DCC_NAMESPACE {

class ModuleObject;
class LayoutFactoryBase;

class PluginInterface : public QObject
{
    Q_OBJECT
public:
    PluginInterface(QObject *parent = nullptr)
        : QObject(parent) {}
    virtual ~PluginInterface() {}

    /**
     * @brief 插件最基本的元素由 ModuleObject 组成，所以最少有一个 ModuleObject
     * @brief 插件子项列表
     */
    virtual ModuleObject *module() = 0;

    /**
     * @brief 标识插件信息
     * @return 插件名称
     */
    virtual QString name() const
    {
        return QString();
    }

    /**
     * @brief 插件必须知道其需要跟随的父ModuleObject的url ,默认为空则为一级插件
     * @return 跟随的父ModuleObject的url
     */
    virtual QString follow() const
    {
        return QString();
    }

    /**
     * @brief 插件位置索引，相同索引则按加载顺序进行排序，先加载的往后顺延，默认追加到最后
     * @return 位置索引
     */
    virtual int location() const
    {
        return -1;
    }

    /**
     * @brief layoutFactory 自定义布局工厂类，实现自定义布局
     * @return
     */
    virtual QList<LayoutFactoryBase *> layoutFactory() { return {}; }
};

} // namespace DCC_NAMESPACE

Q_DECLARE_INTERFACE(DCC_NAMESPACE::PluginInterface, "com.deepin.dde.ControlCenter.Plugin/1.1")
