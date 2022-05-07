#include "pluginmanager.h"
#include "interface/moduleobject.h"
#include "interface/plugininterface.h"

#include <QDir>
#include <QDebug>
#include <QElapsedTimer>
#include <QPluginLoader>
#include <QCoreApplication>
#include <queue>

DCC_USE_NAMESPACE

const QString &PluginDirectory = QStringLiteral("/usr/lib/dde-control-center/modules");

bool comparePluginLocation(const QPair<PluginInterface*, ModuleObject *> target1, const QPair<PluginInterface*, ModuleObject *> target2)
{
    return target1.first->location() < target2.first->location();
}

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
    , m_rootModule(new ModuleObject(this))
{

}

void PluginManager::loadModules()
{
    QDir pluginDir(PluginDirectory);
#ifdef QT_DEBUG
    pluginDir.setPath(qApp->applicationDirPath());
#endif
    if (!pluginDir.exists()) {
        qWarning() << "plugin directory not exists";
        return;
    }

    const auto &pluginList = pluginDir.entryInfoList();
    for (auto &&pluginName : pluginList) {
        QString path = pluginName.absoluteFilePath();

        if (!QLibrary::isLibrary(path))
            continue;

        qInfo() << "loading plugin: " << path;

        QElapsedTimer et;
        et.start();
        QPluginLoader *loader = new QPluginLoader(path, this);
        if (!loader->load()) {
            qWarning() << QString("The plugin: %1 load failed! error message: %2").arg(path, loader->errorString());
        }
        const QJsonObject &meta = loader->metaData().value("MetaData").toObject();
        if (!compareVersion(meta.value("api").toString(), "1.0.0")) {
            qWarning() << QString("The version of plugin: %1 is too low!").arg(path);
            continue;
        }

        PluginInterface * plugin = qobject_cast<PluginInterface *>(loader->instance());
        if (!plugin) {
            qWarning() << QString("Can't read plugin: %1").arg(path);
            loader->unload();
            continue;
        }
        qInfo() << QString("load plugin: %1 end, using time: %2 ms").arg(path).arg(et.elapsed());
        // 防止内存泄露
        plugin->setParent(this);
        m_loaders.append(loader);
    }
    initModules();
}

void PluginManager::initModules()
{
    initRootModule();
    initOtherModule();
    for (auto &&loader : m_loaders) {
        auto *plugin = qobject_cast<PluginInterface *>(loader->instance());
        qWarning() << QString("can't find follow:%1, the plugin:%2 will unload!").arg(plugin->follow(), plugin->name());
        loader->unload();
    }
}

void PluginManager::initRootModule()
{
    QList<QPair<PluginInterface*, ModuleObject *>> plugins;
    for (auto &&loader : m_loaders) {
        auto *plugin = qobject_cast<PluginInterface *>(loader->instance());
        if (plugin->follow().isEmpty()) {
            QElapsedTimer et;
            et.start();
            auto child = plugin->module();
            plugins.append(QPair<PluginInterface*, ModuleObject *>(plugin, child));
            m_loaders.removeOne(loader);
            qInfo() << QString("init module:%1 using time: %2 ms").arg(plugin->name()).arg(et.elapsed());
        }
    }
    std::sort(plugins.begin(), plugins.end(), comparePluginLocation);
    for (auto plugin : plugins) {
        m_rootModule->appendChild(plugin.second);
    }
}

void PluginManager::initOtherModule()
{
    for (auto &&loader : m_loaders) {
        auto *plugin = qobject_cast<PluginInterface *>(loader->instance());
        auto *module = findModule(m_rootModule, plugin->follow());
        if (module) {
            QElapsedTimer et;
            et.start();
            module->insertChild(plugin->location(), plugin->module());
            m_loaders.removeOne(loader);
            qInfo() << QString("init module:%1 using time: %2 ms").arg(plugin->name()).arg(et.elapsed());
        }
    }
}

ModuleObject *PluginManager::findModule(ModuleObject *module, const QString &name)
{
    if (!module)
        return nullptr;

    std::queue<ModuleObject*> qbfs;
    qbfs.push(module);
    while (!qbfs.empty()) {
        if (qbfs.front()->name() == name)
            return qbfs.front();
        for (auto &&child : qbfs.front()->childrens()) {
            qbfs.push(child);
        }
        qbfs.pop();
    }

    return nullptr;
}

bool PluginManager::compareVersion(const QString &targetVersion, const QString &baseVersion)
{
    const QStringList &version1 = baseVersion.split(".");
    const QStringList &version2 = targetVersion.split(".");

    const auto size = qAbs<int>(version1.size() - version2.size());

    for (int i = 0; i < size; ++i) {
        // 相等判断下一个子版本号
        if (version1[i] == version2[i])
            continue;

        // 转成整形比较
        if (version1[i].toInt() > version2[i].toInt()) {
            return false;
        } else {
            return true;
        }
    }

    return true;
}
