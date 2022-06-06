#include "testplugin.h"
#include "interface/moduleobject.h"

#include <QLabel>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qwidget.h>

QString Test2Plugin::name() const
{
    return QStringLiteral("plugin-test2");
}

ModuleObject* Test2Plugin::module()
{
    //-----------创建根节点----------
    ModuleObject *moduleRoot = new ModuleObject("menu3", tr("菜单3"), tr("我是菜单3"), QIcon::fromTheme("preferences-system"), this);
    moduleRoot->setChildType(ModuleObject::Page);

    for (int j = 0; j < 4; j++) {
        LabelModule *labelModule = new LabelModule(QString("main%1menu%2").arg(3).arg(j), QString("具体页面%1的第%2个page的标题").arg(3).arg(j), moduleRoot);
        labelModule->setText(QString("二级插件页面%1的第%2个page").arg(3).arg(j));
        moduleRoot->appendChild(labelModule);
    }

    return moduleRoot;
}

QString Test2Plugin::follow() const
{
    // 注意这里返回的是上级的url
    return QStringLiteral("firstmenu");
}

int Test2Plugin::location() const
{
    return 2;
}

QWidget *LabelModule::page()
{
    return new QLabel(text());
}

void LabelModule::setText(const QString &text)
{
    m_text = text;
}

