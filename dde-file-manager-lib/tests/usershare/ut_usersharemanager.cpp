#include <gtest/gtest.h>
#include <QDateTime>

#include "usershare/usersharemanager.h"
#include <QProcess>
#include <QDebug>

using namespace testing;
class UserShareManagerTest:public testing::Test{

public:
    UserShareManager sharemanager;
    virtual void SetUp() override{
        std::cout << "start UserShareManagerTest" << std::endl;
    }

    virtual void TearDown() override{
        std::cout << "end UserShareManagerTest" << std::endl;
    }
};


TEST_F(UserShareManagerTest,can_getCacehPath){
    EXPECT_FALSE(sharemanager.getCacehPath().isEmpty());
}

TEST_F(UserShareManagerTest,can_getOldShareInfoByNewInfo){
    EXPECT_FALSE(sharemanager.getOldShareInfoByNewInfo(ShareInfo()).isValid());
}

TEST_F(UserShareManagerTest,can_getShareInfoByPath){
    EXPECT_FALSE(sharemanager.getShareInfoByPath(QString()).isValid());
}

TEST_F(UserShareManagerTest,can_getShareNameByPath){
    EXPECT_TRUE(sharemanager.getShareNameByPath(QString()).isEmpty());
}

TEST_F(UserShareManagerTest,can_shareInfoList){
    EXPECT_TRUE(sharemanager.shareInfoList().isEmpty());
}

TEST_F(UserShareManagerTest,can_validShareInfoCount){
    EXPECT_TRUE(sharemanager.validShareInfoCount() == 0);
}

TEST_F(UserShareManagerTest,can_addUserShare){

    ShareInfo info;
    DUrl url;
    url.setPath("./share");
    url.setScheme(FILE_SCHEME);
    QProcess::execute("mkdir "+url.toLocalFile());
    info.setPath(url.path());

    info.setShareName("share");
    info.setIsGuestOk(true);
    info.setIsWritable(true);
    EXPECT_TRUE(sharemanager.addUserShare(info));
    EXPECT_FALSE(sharemanager.getOldShareInfoByNewInfo(info).isValid());
    sharemanager.testUpdateUserShareInfo();
    sharemanager.usershareCountchanged();
    sharemanager.initSamaServiceSettings();
}

TEST_F(UserShareManagerTest,can_addUserShare_false){

    ShareInfo info;
    info.setPath("~/share/share");

    info.setShareName("share");
    info.setIsGuestOk(true);
    info.setIsWritable(true);
    EXPECT_FALSE(sharemanager.addUserShare(info));
}

TEST_F(UserShareManagerTest,can_addUserShare_same){
    ShareInfo info;
    info.setPath("~/share");
    info.setShareName("share");
    info.setIsGuestOk(true);
    info.setIsWritable(true);
    EXPECT_FALSE(sharemanager.addUserShare(info));
}

TEST_F(UserShareManagerTest,can_addUserShare_changepath){
    ShareInfo info;
    DUrl url,urlone;
    url.setPath("./share_other");
    url.setScheme(FILE_SCHEME);
    urlone = url;
    QProcess::execute("mkdir "+url.toLocalFile());
    info.setPath(url.path());
    info.setShareName("share");
    info.setIsGuestOk(true);
    info.setIsWritable(true);
    EXPECT_TRUE(sharemanager.addUserShare(info));
    EXPECT_FALSE(sharemanager.getOldShareInfoByNewInfo(info).isValid());
    EXPECT_FALSE(sharemanager.getShareInfoByPath(url.toLocalFile()).isValid());
    EXPECT_TRUE(sharemanager.getShareNameByPath(url.toLocalFile()).isEmpty());
    urlone.setPath("./share");
    info.setPath(urlone.path());
    info.setShareName("share_other");
    EXPECT_TRUE(sharemanager.addUserShare(info));
    EXPECT_TRUE(sharemanager.getShareInfoByPath(urlone.toLocalFile()).isValid());
    EXPECT_FALSE(sharemanager.getShareNameByPath(urlone.toLocalFile()).isEmpty());
    EXPECT_FALSE(sharemanager.shareInfoList().isEmpty());
    EXPECT_FALSE(sharemanager.getCurrentUserName().isEmpty());
    EXPECT_FALSE(sharemanager.validShareInfoCount() == 0);
    sharemanager.testUpdateUserShareInfo();
    sharemanager.usershareCountchanged();
    sharemanager.initSamaServiceSettings();
    EXPECT_TRUE(sharemanager.isShareFile(urlone.path()));

}

TEST_F(UserShareManagerTest,can_deleteUserShareByPath){
    DUrl url;
    url.setPath("./share");
    url.setScheme(FILE_SCHEME);
    QProcess::execute("rm -rf " + url.toLocalFile());
    url.setPath("./share_other");
    sharemanager.deleteUserShareByPath(url.toLocalFile());
    QProcess::execute("rm -rf " + url.toLocalFile());
}

