/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     Gary Wang <wzc782970009@gmail.com>
 *
 * Maintainer: Gary Wang <wangzichong@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "dfmfilecrumbcontroller.h"
#include "dfmcrumbitem.h"

#include "dfileinfo.h"

#include "singleton.h"

#include <QStandardPaths>
#include <QStorageInfo>

#include <QDebug>

DFM_BEGIN_NAMESPACE

DFMFileCrumbController::DFMFileCrumbController(QObject *parent)
    : DFMCrumbInterface(parent)
{
    homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).last();
}

DFMFileCrumbController::~DFMFileCrumbController()
{

}

bool DFMFileCrumbController::supportedUrl(DUrl url)
{
    Q_UNUSED(url);
    qWarning("DFMFileCrumbController::supportedUrl() should be implemented!!!");  
    return true;
}

QList<CrumbData> DFMFileCrumbController::seprateUrl(const DUrl &url)
{
    QList<CrumbData> list;
    QString prefixPath = "/";
    const QString &path = url.isLocalFile() ? url.toLocalFile() : QString();

    if (path.isEmpty()) {
        return list;
    }

    if (path.startsWith(homePath)) {
        prefixPath = homePath;
        CrumbData data(DUrl::fromLocalFile(homePath), "Home", "CrumbIconButton.Home");
        list.append(data);
    } else {
        QStorageInfo storageInfo(path);
        prefixPath = storageInfo.rootPath();

        if (prefixPath == "/") {
            CrumbData data(DUrl(FILE_ROOT), "Root", "CrumbIconButton.Disk");
            list.append(data);
        } else {
            CrumbData data(DUrl::fromLocalFile(prefixPath), QString(), "CrumbIconButton.Disk");
            list.append(data);
        }
    }

    DFileInfo info(url);
    DUrlList urlList = info.parentUrlList();

    // Push parent urls into crumb list (without prefix url)
    for (const DUrl & oneUrl : urlList) {
        if (oneUrl.path().startsWith(prefixPath) && oneUrl.path() != prefixPath) {
            CrumbData data(oneUrl, oneUrl.fileName());
            list.append(data);
        }
    }
    // Then itself..
    CrumbData data(url, url.fileName());
    list.append(data);

    return list;
}

DFMCrumbItem *DFMFileCrumbController::createCrumbItem(const CrumbData &data)
{
    return new DFMCrumbItem(data);
}

DFMCrumbItem *DFMFileCrumbController::createCrumbItem(const DUrl &url)
{

}

QStringList DFMFileCrumbController::getSuggestList(const QString &text)
{

}

DFM_END_NAMESPACE
