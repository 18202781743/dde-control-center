//SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    name: "dock"
    parentName: "personalization"
    displayName: qsTr("Desktop and taskbar")
    description: qsTr("Desktop organization, taskbar mode, plugin area settings")
    icon: "dock"
    weight: 100
}
