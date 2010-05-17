#include <QApplication>
#include <QCheckBox>
#include <QVBoxLayout>
#include "flowrategraph.hh"
#include "def.hh"

int main(int argc, char **argv) {
    QApplication a (argc, argv);

    QWidget *window = new QWidget;
    window->setWindowTitle("Multiple Channels Scheduler");

    QList<Interface> infs = ListAddr::validIPv4Infs();
    FlowrateDict dict;
    foreach(Interface inf, infs) {
        dict[inf] = new FlowrateHistory(120);
    }

    FlowrateGraph *graph = new FlowrateGraph(&dict, 1);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(graph);
    
    foreach (Interface inf, infs) {
        QCheckBox *checkbox = new QCheckBox(inf.name);
        layout->addWidget(checkbox);
    }
    
    window->setLayout(layout);
    window->show();
    return a.exec();
}

    
    

