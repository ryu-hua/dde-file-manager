#include "backgroundmanager.h"
#include "screen/screenhelper.h"
#include "util/xcb/xcb.h"

#include <qpa/qplatformwindow.h>
#include <QImageReader>

BackgroundManager::BackgroundManager(bool preview, QObject *parent)
    : QObject(parent)
    ,windowManagerHelper(DWindowManagerHelper::instance())
    ,m_preview(preview)
{
    init();
}

BackgroundManager::~BackgroundManager()
{
    if (gsettings){
        gsettings->deleteLater();
        gsettings = nullptr;
    }

    if (wmInter){
        wmInter->deleteLater();
        wmInter = nullptr;
    }

    windowManagerHelper = nullptr;

    m_backgroundMap.clear();
}

void BackgroundManager::onRestBackgroundManager()
{
    //判断是否需要绘制背景，如果是窗口绘制背景则桌面isEnable返回false，即桌面不绘制背景
    if (m_preview || isEnabled()) {
        if (wmInter) {
            return;
        }

        wmInter = new WMInter("com.deepin.wm", "/com/deepin/wm", QDBusConnection::sessionBus(), this);
        gsettings = new DGioSettings("com.deepin.dde.appearance", "", this);

        if (!m_preview) {
            connect(wmInter, &WMInter::WorkspaceSwitched, this, [this] (int, int to) {
                currentWorkspaceIndex = to;
                onResetBackgroundImage(); //todo
            });

            connect(gsettings, &DGioSettings::valueChanged, this, [this] (const QString & key, const QVariant & value) {
                Q_UNUSED(value);
                if (key == "background-uris") {
                    onResetBackgroundImage(); //todo
                }
            });
        }

        //屏幕改变
        connect(ScreenHelper::screenManager(), &AbstractScreenManager::sigScreenChanged,
                this, &BackgroundManager::onBackgroundBuild);
        disconnect(ScreenHelper::screenManager(), &AbstractScreenManager::sigScreenChanged,
                this, &BackgroundManager::onSkipBackgroundBuild);

        //显示模式改变
        connect(ScreenHelper::screenManager(), &AbstractScreenManager::sigDisplayModeChanged,
                this, &BackgroundManager::onBackgroundBuild);
        disconnect(ScreenHelper::screenManager(), &AbstractScreenManager::sigDisplayModeChanged,
                this, &BackgroundManager::onSkipBackgroundBuild);

        //屏幕大小改变
        connect(ScreenHelper::screenManager(), &AbstractScreenManager::sigScreenGeometryChanged,
                this, &BackgroundManager::onScreenGeometryChanged);

        //可用区改变
        connect(ScreenHelper::screenManager(), &AbstractScreenManager::sigScreenAvailableGeometryChanged,
                this, &BackgroundManager::onScreenGeometryChanged);

        //创建背景
        onBackgroundBuild();
    } else {

        // 清理数据
        if (gsettings){
            gsettings->deleteLater();
            gsettings = nullptr;
        }

        if (wmInter){
            wmInter->deleteLater();
            wmInter = nullptr;
        }

        currentWorkspaceIndex = 0;

        //屏幕改变
        connect(ScreenHelper::screenManager(), &AbstractScreenManager::sigScreenChanged,
                this, &BackgroundManager::onSkipBackgroundBuild);
        disconnect(ScreenHelper::screenManager(), &AbstractScreenManager::sigScreenChanged,
                this, &BackgroundManager::onBackgroundBuild);

        //显示模式改变
        connect(ScreenHelper::screenManager(), &AbstractScreenManager::sigDisplayModeChanged,
                this, &BackgroundManager::onSkipBackgroundBuild);
        disconnect(ScreenHelper::screenManager(), &AbstractScreenManager::sigDisplayModeChanged,
                this, &BackgroundManager::onBackgroundBuild);

        //没有背景所以不用关心
        disconnect(ScreenHelper::screenManager(), &AbstractScreenManager::sigScreenGeometryChanged,
                this, &BackgroundManager::onScreenGeometryChanged);
        disconnect(ScreenHelper::screenManager(), &AbstractScreenManager::sigScreenAvailableGeometryChanged,
                this, &BackgroundManager::onScreenGeometryChanged);

        //销毁窗口
        m_backgroundMap.clear();

        //不创建背景
        onSkipBackgroundBuild();
    }
}

void BackgroundManager::onScreenGeometryChanged(ScreenPointer sp)
{
    BackgroundWidgetPointer bw = m_backgroundMap.value(sp);
    qInfo() << "screen geometry changed" << sp.get() << bw.get();
    if (bw.get() != nullptr){
        qInfo() << "background geometry change from" << bw->geometry() << "to" << sp->geometry()
                << "screen name" << sp->name();
        //bw->windowHandle()->handle()->setGeometry(sp->handleGeometry()); //不能设置，设置了widget的geometry会被乱改
        bw->setGeometry(sp->geometry());

        //todo 背景处理
        onResetBackgroundImage();
    }
}

void BackgroundManager::init()
{
    if (!m_preview) {
        connect(windowManagerHelper, &DWindowManagerHelper::windowManagerChanged,
                this, &BackgroundManager::onRestBackgroundManager);
        connect(windowManagerHelper, &DWindowManagerHelper::hasCompositeChanged,
                this, &BackgroundManager::onRestBackgroundManager);
    }

    onRestBackgroundManager();
}

BackgroundWidgetPointer BackgroundManager::createBackgroundWidget(ScreenPointer screen)
{
    BackgroundWidgetPointer bwp(new BackgroundWidget);
    bwp->setProperty("isPreview", m_preview);
    bwp->setProperty("myScreen", screen->name()); // assert screen->name is unique
    //bwp->createWinId();   //不创建，4k下有bug
    //bwp->windowHandle()->handle()->setGeometry(screen->handleGeometry()); //不能设置，设置了widget的geometry会被乱改//分辨率原始大小
    bwp->setGeometry(screen->geometry()); //经过缩放的区域
    qInfo() << "screen name" << screen->name() << "geometry" << screen->geometry() << bwp.get();

    if (m_preview) {
        bwp->setWindowFlags(bwp->windowFlags() | Qt::BypassWindowManagerHint | Qt::WindowDoesNotAcceptFocus);
    } else {
        Xcb::XcbMisc::instance().set_window_type(bwp->winId(), Xcb::XcbMisc::Desktop);
    }

    return bwp;
}

bool BackgroundManager::isEnabled() const
{
    // 只支持kwin，或未开启混成的桌面环境
    return windowManagerHelper->windowManagerName() == DWindowManagerHelper::KWinWM || !windowManagerHelper->hasComposite();
}

void BackgroundManager::setVisible(bool visible)
{
    m_visible = visible;
    for (BackgroundWidgetPointer w : m_backgroundMap.values()) {
        w->setVisible(visible);
    }
}

bool BackgroundManager::isVisible() const
{
    return m_visible;
}

BackgroundWidgetPointer BackgroundManager::backgroundWidget(ScreenPointer sp) const
{
    return m_backgroundMap.value(sp);
}

void BackgroundManager::setBackgroundImage(const QString &screen, const QString &path)
{
    if (screen.isEmpty() || path.isEmpty())
        return;

    m_backgroundImagePath[screen] = path;
    onResetBackgroundImage();
}

void BackgroundManager::onBackgroundBuild()
{
    //屏幕模式判断
    AbstractScreenManager::DisplayMode mode = ScreenMrg->displayMode();
    qDebug() << "screen mode" << mode << "screen count" << ScreenMrg->screens().size();

    //删除不存在的屏
    m_backgroundMap.clear();

    //实际是单屏
    if ((AbstractScreenManager::Showonly == mode) || (AbstractScreenManager::Duplicate == mode) //仅显示和复制
            || (ScreenMrg->screens().count() == 1)){   //单屏模式

        ScreenPointer primary = ScreenMrg->primaryScreen();
        if (primary == nullptr){
            qCritical() << "get screen failed return";
            return;
        }

        BackgroundWidgetPointer bwp = createBackgroundWidget(primary);
        m_backgroundMap.insert(primary,bwp);

        //todo 设置壁纸
        onResetBackgroundImage();

        if (m_visible)
            bwp->show();
        else
            qDebug() << "Disable show the background widget, of screen:" << primary->name() << primary->geometry();
    }
    else {  //多屏
        for (ScreenPointer sc : ScreenMrg->logicScreens()){
            BackgroundWidgetPointer bwp = createBackgroundWidget(sc);
            m_backgroundMap.insert(sc,bwp);

            //todo 设置壁纸

            if (m_visible)
                bwp->show();
            else
                qDebug() << "Disable show the background widget, of screen:" << sc->name() << sc->geometry();
        }

        onResetBackgroundImage();
    }

    //通知view重建
    emit sigBackgroundBuilded(mode);
}

void BackgroundManager::onSkipBackgroundBuild()
{
    //通知view重建
    emit sigBackgroundBuilded(ScreenMrg->displayMode());
}

//临时使用
void BackgroundManager::onResetBackgroundImage()
{
    auto getPix = [](const QString &path,const QPixmap &defalutPixmap)->QPixmap{
        if (path.isEmpty())
            return defalutPixmap;

        auto currentWallpaper = path.startsWith("file:") ? QUrl(path).toLocalFile() : path;
        QPixmap backgroundPixmap = QPixmap(currentWallpaper);
        // fix whiteboard shows when a jpeg file with filename xxx.png
        // content formart not epual to extension
        if (backgroundPixmap.isNull()) {
            QImageReader reader(currentWallpaper);
            reader.setDecideFormatFromContent(true);
            backgroundPixmap = QPixmap::fromImage(reader.read());
        }
        return backgroundPixmap.isNull() ? defalutPixmap : backgroundPixmap;
    };

    QPixmap defaultImage;
    //todo 获取每个屏幕的壁纸
    {
        QString path = QDBusConnection::sessionBus().interface()->isServiceRegistered("com.deepin.wm")
                ? wmInter->GetCurrentWorkspaceBackground() : QString();

        if (path.isEmpty() || !QFile::exists(QUrl(path).toLocalFile())
                // 调用失败时会返回 "The name com.deepin.wm was not provided by any .service files"
                // 此时 wmInter->isValid() = true, 且 dubs last error type 为 NoError
                || (!path.startsWith("/") && !path.startsWith("file:"))) {
            path = gsettings->value("background-uris").toStringList().value(currentWorkspaceIndex);

            if (path.isEmpty()) {
                qWarning() << "invalid path, will not setbackground";
                //return;
            }
        }

        defaultImage = getPix(path, defaultImage);
    }

    for (ScreenPointer sp : m_backgroundMap.keys()){
        QString userPath = m_backgroundImagePath.value(sp->name());
        QPixmap backgroundPixmap = getPix(userPath, defaultImage);
        if (backgroundPixmap.isNull())
            continue;

        BackgroundWidgetPointer bw = m_backgroundMap.value(sp);
        QSize trueSize = sp->handleGeometry().size(); //使用屏幕缩放前的分辨率
        auto pix = backgroundPixmap.scaled(trueSize,
                         Qt::KeepAspectRatioByExpanding,
                         Qt::SmoothTransformation);

        if (pix.width() > trueSize.width() || pix.height() > trueSize.height()) {
            pix = pix.copy(QRect((pix.width() - trueSize.width()) / 2.0,
                                 (pix.height() - trueSize.height()) / 2.0,
                                 trueSize.width(),
                                 trueSize.height()));
        }

        qDebug() << "background truesize" << trueSize <<"devicePixelRatio"
                 << bw->devicePixelRatioF() << pix << "widget" << bw.get();
        pix.setDevicePixelRatio(bw->devicePixelRatioF());
        bw->setPixmap(pix);
    }
}
