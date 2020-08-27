#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QWindow>
#include <QTest>
#include <QPlainTextEdit>
#include <QPushButton>

#define private public
#include "views/dfmvaultrecoverykeypages.h"


namespace  {
    class TestDFMVaultRecoveryKeyPages : public testing::Test
    {
    public:
        QSharedPointer<DFMVaultRecoveryKeyPages> m_view;

        virtual void SetUp() override
        {
            m_view = QSharedPointer<DFMVaultRecoveryKeyPages>(new DFMVaultRecoveryKeyPages());
            m_view->show();
            std::cout << "start TestDFMVaultRecoveryKeyPages" << std::endl;
        }

        virtual void TearDown() override
        {
            m_view->close();
            std::cout << "end TestDFMVaultRecoveryKeyPages" << std::endl;
        }
    };
}


TEST_F(TestDFMVaultRecoveryKeyPages, tst_buttonClicked)
{
    m_view->onButtonClicked(1);
    m_view->onButtonClicked(0);
}

TEST_F(TestDFMVaultRecoveryKeyPages, tst_recoveryKeyChanged)
{
    QTest::keyClicks(m_view->m_recoveryKeyEdit, "");
    EXPECT_FALSE(m_view->getButton(1)->isEnabled());

    EXPECT_NO_FATAL_FAILURE(QTest::keyClicks(m_view->m_recoveryKeyEdit, "123465678aA!"));
    EXPECT_NO_FATAL_FAILURE(QTest::keyClicks(m_view->m_recoveryKeyEdit,
                     "12346567891011121314151611234656789101112131415161aA!"));
}

TEST_F(TestDFMVaultRecoveryKeyPages, tst_onUnlockVault)
{
//    m_view->m_bUnlockByKey = true;
//    m_view->onUnlockVault(0);
//    EXPECT_TRUE(m_view->isHidden());
}

TEST_F(TestDFMVaultRecoveryKeyPages, tst_eventFilter)
{
    QTest::keyClick(m_view->m_recoveryKeyEdit, Qt::Key_Enter);
}



