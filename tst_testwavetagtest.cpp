#include <QString>
#include <QtTest>
#include "intervaltree.h"

class TestwavetagTest : public QObject
{
    Q_OBJECT

public:
    TestwavetagTest();

private Q_SLOTS:
    void testIntervalTree();
};

TestwavetagTest::TestwavetagTest()
{
}

void TestwavetagTest::testIntervalTree()
{
    IntervalTree<float> tree;
    tree.add(0,0);
    QVERIFY(tree.bounds() == 0);
    QVERIFY(!tree.inside(0));
    QVERIFY(!tree.inside(1));
    QVERIFY(tree.bounds() == 0);
    tree.add(0,1);
    QVERIFY(!tree.inside(-1));
    QVERIFY(tree.inside(0));
    QVERIFY(tree.inside(0.9));
    QVERIFY(!tree.inside(1));
    QVERIFY(!tree.inside(1.1));
    QVERIFY(tree.bounds() == 2);
    tree.add(2,3);
    QVERIFY(!tree.inside(-1));
    QVERIFY(tree.inside(0));
    QVERIFY(tree.inside(0.5));
    QVERIFY(!tree.inside(1));
    QVERIFY(!tree.inside(1.1));
    QVERIFY(tree.inside(2));
    QVERIFY(tree.inside(2.5));
    QVERIFY(tree.inside(2.99));
    QVERIFY(!tree.inside(3));
    QVERIFY(!tree.inside(3.1));
    QVERIFY(tree.bounds() == 4);
    tree.add(0.5,1.5);
    QVERIFY(!tree.inside(-1));
    QVERIFY(tree.inside(0));
    QVERIFY(tree.inside(0.5));
    QVERIFY(tree.inside(1));
    QVERIFY(tree.inside(1.4));
    QVERIFY(!tree.inside(1.5));
    QVERIFY(!tree.inside(1.9));
    QVERIFY(tree.inside(2));
    QVERIFY(tree.inside(2.5));
    QVERIFY(tree.inside(2.99));
    QVERIFY(!tree.inside(3));
    QVERIFY(!tree.inside(3.1));
    QVERIFY(tree.bounds() == 4);
    tree.add(1.5,1.6);
    QVERIFY(!tree.inside(-1));
    QVERIFY(tree.inside(0));
    QVERIFY(tree.inside(0.5));
    QVERIFY(tree.inside(1));
    QVERIFY(tree.inside(1.4));
    QVERIFY(tree.inside(1.5));
    QVERIFY(!tree.inside(1.6));
    QVERIFY(!tree.inside(1.9));
    QVERIFY(tree.inside(2));
    QVERIFY(tree.inside(2.5));
    QVERIFY(tree.inside(2.99));
    QVERIFY(!tree.inside(3));
    QVERIFY(!tree.inside(3.1));
    QVERIFY(tree.bounds() == 4);
    tree.add(1.6,2);
    QVERIFY(!tree.inside(-1));
    QVERIFY(tree.inside(0));
    QVERIFY(tree.inside(0.5));
    QVERIFY(tree.inside(1));
    QVERIFY(tree.inside(1.4));
    QVERIFY(tree.inside(1.5));
    QVERIFY(tree.inside(1.6));
    QVERIFY(tree.inside(1.9));
    QVERIFY(tree.inside(2));
    QVERIFY(tree.inside(2.5));
    QVERIFY(tree.inside(2.99));
    QVERIFY(!tree.inside(3));
    QVERIFY(!tree.inside(3.1));
    QVERIFY(tree.bounds() == 2);
    tree.subtract(1,2);
    QVERIFY(!tree.inside(-1));
    QVERIFY(tree.inside(0));
    QVERIFY(tree.inside(0.5));
    QVERIFY(!tree.inside(1));
    QVERIFY(!tree.inside(1.9));
    QVERIFY(tree.inside(2));
    QVERIFY(tree.inside(2.5));
    QVERIFY(tree.inside(2.99));
    QVERIFY(!tree.inside(3));
    QVERIFY(!tree.inside(3.1));
    QVERIFY(tree.bounds() == 4);
    tree.subtract(2.5,3.5);
    QVERIFY(!tree.inside(-1));
    QVERIFY(tree.inside(0));
    QVERIFY(tree.inside(0.5));
    QVERIFY(!tree.inside(1));
    QVERIFY(!tree.inside(1.9));
    QVERIFY(tree.inside(2));
    QVERIFY(tree.inside(2.4));
    QVERIFY(!tree.inside(2.5));
    QVERIFY(!tree.inside(3));
    QVERIFY(tree.bounds() == 4);
    tree.subtract(2.5,3);
    QVERIFY(!tree.inside(-1));
    QVERIFY(tree.inside(0));
    QVERIFY(tree.inside(0.5));
    QVERIFY(!tree.inside(1));
    QVERIFY(!tree.inside(1.9));
    QVERIFY(tree.inside(2));
    QVERIFY(tree.inside(2.4));
    QVERIFY(!tree.inside(2.5));
    QVERIFY(!tree.inside(3));
    QVERIFY(tree.bounds() == 4);
    tree.subtract(0,0.5);
    QVERIFY(!tree.inside(-1));
    QVERIFY(!tree.inside(0));
    QVERIFY(!tree.inside(0.4));
    QVERIFY(tree.inside(0.5));
    QVERIFY(tree.inside(0.6));
    QVERIFY(tree.inside(0.9));
    QVERIFY(!tree.inside(1));
    QVERIFY(!tree.inside(1.9));
    QVERIFY(tree.inside(2));
    QVERIFY(tree.inside(2.4));
    QVERIFY(!tree.inside(2.5));
    QVERIFY(!tree.inside(3));
    QVERIFY(tree.bounds() == 4);
    tree.subtract(0.6,0.6);
    QVERIFY(!tree.inside(-1));
    QVERIFY(!tree.inside(0));
    QVERIFY(!tree.inside(0.4));
    QVERIFY(tree.inside(0.5));
    QVERIFY(tree.inside(0.6));
    QVERIFY(tree.inside(0.9));
    QVERIFY(!tree.inside(1));
    QVERIFY(!tree.inside(1.9));
    QVERIFY(tree.inside(2));
    QVERIFY(tree.inside(2.4));
    QVERIFY(!tree.inside(2.5));
    QVERIFY(!tree.inside(3));
    QVERIFY(tree.bounds() == 4);
    // [(0.5,1.0),(2,2.5)]
    auto r = tree.overlapping(0.6,2.4);
    auto i = r.begin();
    auto j = r.end();
    QVERIFY(i->left == 0.5);
    QVERIFY(i->right == 1.0);
    QVERIFY(i != j);
    ++i;
    QVERIFY(i->left == 2);
    QVERIFY(i->right == 2.5);
    QVERIFY(i != j);
    ++i;
    QVERIFY(i == j);
    r = tree.overlapping(0,2.4);
    i = r.begin();
    j = r.end();
    QVERIFY(i->left == 0.5);
    QVERIFY(i->right == 1.0);
    QVERIFY(i != j);
    ++i;
    QVERIFY(i->left == 2);
    QVERIFY(i->right == 2.5);
    QVERIFY(i != j);
    ++i;
    QVERIFY(i == j);
    r = tree.overlapping(-1,2.4);
    i = r.begin();
    j = r.end();
    QVERIFY(i->left == 0.5);
    QVERIFY(i->right == 1.0);
    QVERIFY(i != j);
    ++i;
    QVERIFY(i->left == 2);
    QVERIFY(i->right == 2.5);
    QVERIFY(i != j);
    ++i;
    QVERIFY(i == j);
    r = tree.overlapping(1.0,2.4);
    i = r.begin();
    j = r.end();
    QVERIFY(i->left == 2);
    QVERIFY(i->right == 2.5);
    QVERIFY(i != j);
    ++i;
    QVERIFY(i == j);
    r = tree.overlapping(1.0,2.5);
    i = r.begin();
    j = r.end();
    QVERIFY(i->left == 2);
    QVERIFY(i->right == 2.5);
    QVERIFY(i != j);
    ++i;
    QVERIFY(i == j);
    r = tree.overlapping(1.0,3.0);
    i = r.begin();
    j = r.end();
    QVERIFY(i->left == 2);
    QVERIFY(i->right == 2.5);
    QVERIFY(i != j);
    ++i;
    QVERIFY(i == j);
    r = tree.overlapping(2.1,2.2);
    i = r.begin();
    j = r.end();
    QVERIFY(i->left == 2);
    QVERIFY(i->right == 2.5);
    QVERIFY(i != j);
    ++i;
    QVERIFY(i == j);
    r = tree.overlapping(2.2,2.2);
    i = r.begin();
    j = r.end();
    QVERIFY(i == j);
    i = tree.begin();
    j = tree.end();
    QVERIFY(i->left == 0.5);
    QVERIFY(i->right == 1.0);
    QVERIFY(i != j);
    ++i;
    QVERIFY(i->left == 2);
    QVERIFY(i->right == 2.5);
    QVERIFY(i != j);
    ++i;
    QVERIFY(i == j);
}

QTEST_APPLESS_MAIN(TestwavetagTest)

#include "tst_testwavetagtest.moc"
