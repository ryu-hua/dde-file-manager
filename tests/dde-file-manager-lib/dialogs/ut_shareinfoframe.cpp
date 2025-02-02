// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "models/dfmrootfileinfo.h"
#include "stub.h"
#include "app/define.h"
#include "dfileservices.h"
#include "testhelper.h"

#include <gtest/gtest.h>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

#define private public
#include "dialogs/shareinfoframe.h"

namespace  {
    class TestShareInfoFrame : public testing::Test
    {
    public:
        void SetUp() override
        {
            QString strPath = QDir::currentPath() + QDir::separator() + "TestPropertyDialog.txt";
            QFile file(strPath);
            if(!file.exists()){
                if(file.open(QIODevice::ReadWrite | QIODevice::Text))
                    file.close();
            }
            DAbstractFileInfoPointer fi(new DFMRootFileInfo(DUrl("file:///test1")));;
            m_pTester = new ShareInfoFrame(fi);
            std::cout << "start TestShareInfoFrame";
        }
        void TearDown() override
        {
            QString strPath = QDir::currentPath() + QDir::separator() + "TestPropertyDialog.txt";
            QFile file(strPath);
            if(file.exists()){
                file.remove();
            }
            QString strPath2 = QDir::currentPath() + QDir::separator() + "TestPropertyDialog.txt.txt";
            QFile file2(strPath2);
            if(file2.exists()){
                file2.remove();
            }
            delete m_pTester;
            m_pTester = nullptr;
            std::cout << "end TestShareInfoFrame";
        }
    public:
        ShareInfoFrame  *m_pTester;
    };
}

TEST_F(TestShareInfoFrame, testInit)
{
    EXPECT_NE(m_pTester, nullptr);
}

TEST_F(TestShareInfoFrame, testSetFileinfo)
{
    DAbstractFileInfoPointer fileinfo(new DFMRootFileInfo(DUrl("file:///test1")));
    m_pTester->setFileinfo(fileinfo);
    QString str = m_pTester->m_fileinfo->path();
    EXPECT_TRUE(str == "");
}

TEST_F(TestShareInfoFrame, testCheckShareName)
{
    QString(*stub_fileName)() = []()->QString{
        return QString("sharenameedit");
    };
    Stub stu;
    stu.set(ADDR(QFileInfo, fileName), stub_fileName);

    EXPECT_TRUE(m_pTester->checkShareName());
}

TEST_F(TestShareInfoFrame, testHandleCheckBoxChanged)
{
    m_pTester->handleCheckBoxChanged(false);
    bool b = m_pTester->m_shareCheckBox->isEnabled();
    EXPECT_EQ(b, false);
}

TEST_F(TestShareInfoFrame, testHandleCheckBoxChanged2)
{
    m_pTester->handleCheckBoxChanged(true);
    bool b = m_pTester->m_shareCheckBox->isEnabled();
    EXPECT_EQ(b, false);
}

TEST_F(TestShareInfoFrame, testHandleCheckBoxChanged3)
{
    bool(*stub_doShareInfoSetting)() = []()->bool{
        return false;
    };
    Stub stu;
    stu.set(ADDR(ShareInfoFrame, doShareInfoSetting), stub_doShareInfoSetting);
    m_pTester->handleCheckBoxChanged(true);
    bool b = m_pTester->m_shareCheckBox->isEnabled();
    EXPECT_EQ(b, false);
}

TEST_F(TestShareInfoFrame, testHandleShareNameFinished)
{
    EXPECT_NO_FATAL_FAILURE(m_pTester->handleShareNameFinished());
}

TEST_F(TestShareInfoFrame, testHandleShareNameFinished2)
{
    QLineEdit *pShareNamelineEdit = new QLineEdit();
    m_pTester->m_shareNamelineEdit = pShareNamelineEdit;
    pShareNamelineEdit->setFocus();
    QComboBox *pPermissoComBox = new QComboBox();
    m_pTester->m_permissoComBox = pPermissoComBox;

    bool(*stub_hasFocus)() = []()->bool{
            return  true;
    };
    Stub stu;
    stu.set(ADDR(QWidget, hasFocus), stub_hasFocus);

    EXPECT_NO_FATAL_FAILURE(m_pTester->handleShareNameFinished());

    if (pShareNamelineEdit)
        pShareNamelineEdit->deleteLater();
    if (pPermissoComBox)
        pPermissoComBox->deleteLater();

}

TEST_F(TestShareInfoFrame, testHandleShareNameChinged)
{
    QLineEdit shareNamelineEdit;
    m_pTester->m_shareNamelineEdit = &shareNamelineEdit;
    EXPECT_NO_FATAL_FAILURE(m_pTester->handleShareNameChanged(" 123"));
}

TEST_F(TestShareInfoFrame, testHandlePermissionComboxChanged)
{
    EXPECT_NO_FATAL_FAILURE(m_pTester->handlePermissionComboxChanged(1));
}

TEST_F(TestShareInfoFrame, testHandleAnonymityComboxChanged)
{
    EXPECT_NO_FATAL_FAILURE(m_pTester->handleAnonymityComboxChanged(1));
}

TEST_F(TestShareInfoFrame, testHandShareInfoChanged)
{

}

TEST_F(TestShareInfoFrame, testDisactivateWidgets)
{
    m_pTester->disactivateWidgets();
    bool b = m_pTester->m_permissoComBox->isEditable();
    EXPECT_EQ(b, false);
}
#ifndef __arm__
TEST_F(TestShareInfoFrame, testDoShareInfoSetting)
{
    m_pTester->m_shareCheckBox->setChecked(true);
    m_pTester->m_permissoComBox->setCurrentIndex(0);
    m_pTester->m_anonymityCombox->setCurrentIndex(1);
    bool b = m_pTester->doShareInfoSetting();
    EXPECT_EQ(b, false);
}
#endif
#ifndef __arm__
#if 0 // 环境问题，暂时屏蔽
TEST_F(TestShareInfoFrame, testUpdateShareInfo)
{
    TestHelper::runInLoop([](){});

    DAbstractFileInfoPointer fileinfo = fileService->createFileInfo(nullptr, DUrl("file:///home"));
    m_pTester->m_fileinfo = fileinfo;

    bool(*stub_isEmpty)() = []()->bool{
        return false;
    };
    Stub stu;
    stu.set(ADDR(QString, isEmpty), stub_isEmpty);

    m_pTester->updateShareInfo("/home");
    bool b = m_pTester->m_shareCheckBox->isEnabled();
    EXPECT_EQ(b, false);
}
#endif

TEST_F(TestShareInfoFrame, testActivateWidgets)
{
    QComboBox box;
    m_pTester->m_permissoComBox = &box;
    QComboBox box2;
    m_pTester->m_anonymityCombox = &box2;
    m_pTester->activateWidgets();
    bool b = m_pTester->m_permissoComBox->isEnabled();
    EXPECT_EQ(b, true);
}
#endif
























