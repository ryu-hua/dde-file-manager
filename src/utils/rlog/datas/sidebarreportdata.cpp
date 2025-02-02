// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "sidebarreportdata.h"

QString SidebarReportData::type() const
{
    return "Sidebar";
}

QJsonObject SidebarReportData::prepareData(const QVariantMap &args) const
{
    QVariantMap temArgs = args;
    temArgs.insert("tid", 1000500003);
    temArgs.insert("sysTime", QDateTime::currentDateTime().toTime_t());
    return QJsonObject::fromVariantMap(temArgs);
}
