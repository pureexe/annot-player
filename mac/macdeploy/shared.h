#ifndef MAC_DEPLOMYMENT_SHARED_H
#define MAC_DEPLOMYMENT_SHARED_H

// shared.h
// 11/11/2011

#include <QString>
#include <QStringList>
#include <QDebug>

class FrameworkInfo
{
public:
    QString frameworkDirectory;
    QString frameworkName;
    QString frameworkPath;
    QString binaryDirectory;
    QString binaryName;
    QString binaryPath;
    QString version;
    QString installName;
    QString deployedInstallName;
    QString sourceFilePath;
    QString destinationDirectory;
};

bool operator==(const FrameworkInfo &a, const FrameworkInfo &b);
QDebug operator<<(QDebug debug, const FrameworkInfo &info);

class ApplicationBundleInfo
{
public:    
    QString path;
    QString binaryPath;
};

class DeploymentInfo
{
public:
    QString qtPath;
    QString pluginPath;
    QStringList deployedFrameworks;
};


inline QDebug operator<<(QDebug debug, const ApplicationBundleInfo &info);

void changeQtFrameworks(const QString appPath, const QString &qtPath);
void changeQtFrameworks(const QList<FrameworkInfo> frameworks, const QString &appBinaryPath, const QString &qtPath);

FrameworkInfo parseOtoolLibraryLine(const QString &line);
QString findAppBinary(const QString &appBundlePath);
QList<FrameworkInfo> getQtFrameworks(const QString &path);
QList<FrameworkInfo> getQtFrameworks(const QStringList &otoolLines);
QString copyFramework(const FrameworkInfo &framework, const QString path);
DeploymentInfo deployQtFrameworks(const QString &appBundlePath);
DeploymentInfo deployQtFrameworks(QList<FrameworkInfo> frameworks, const QString &bundlePath, const QString &binaryPath);
void createQtConf(const QString &appBundlePath);
void deployPlugins(const QString &appBundlePath, DeploymentInfo deploymentInfo);
void changeIdentification(const QString &id, const QString &binaryPath);
void changeInstallName(const QString &oldName, const QString &newName, const QString &binaryPath);
QString findAppBinary(const QString &appBundlePath);
void createDiskImage(const QString &appBundlePath);

#endif
