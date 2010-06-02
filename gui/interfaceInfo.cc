#include "interfaceInfo.hh"
#include "flowrategraph.hh"
#include "listAddr.hh"

InterfaceInfo::InterfaceInfo(FlowrateDict *ratedict, double interval, QWidget *parent)
    :QVBoxLayout(parent), ratedict(ratedict), interval(interval), graph(0), infLayout(0)
{
    qDebug() << graph;
    init();
}

void InterfaceInfo::init() {
    graph = new FlowrateGraph(ratedict, interval);
    addWidget(graph);
    
    infLayout = new QVBoxLayout();
    QList<Interface> infs = ListAddr::validIPv4Infs();
    foreach (Interface inf, infs) {
        QLabel *label = new QLabel(inf.name + ":  " + inf.addr.toString(), graph);
        infLayout->addWidget(label);
    }
    addLayout(infLayout);
}

void InterfaceInfo::refresh() {
    if (graph) {
        removeWidget(graph);
        delete graph;
        graph = 0;
    }
    if (infLayout) {
        removeItem(infLayout);
        delete infLayout;
        infLayout = 0;
    }
    init();
}
    

void InterfaceInfo::redraw() {
    if (graph) {
        graph->plot();
    }
}


    
