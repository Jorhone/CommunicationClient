#include "TestDemoForm.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CTestDemoForm form;
    form.show();

    return a.exec();
}
