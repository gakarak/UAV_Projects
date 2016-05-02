#include "model/main_model.h"
#include "controller/main_controller.h"
#include "view/main_view.h"
#include <QApplication>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    shared_ptr<modelpkg::MainModel> main_model = make_shared<modelpkg::MainModel>();
    shared_ptr<controllerpkg::MainController> main_controller = make_shared<controllerpkg::MainController>();
    shared_ptr<viewpkg::MainView> main_view = make_shared<viewpkg::MainView>();

    main_controller->setModel(main_model);
    main_controller->setView(main_view);

    main_view->setController(main_controller);

    main_view->show();

    return a.exec();
}
