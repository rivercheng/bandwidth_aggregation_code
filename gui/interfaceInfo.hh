#ifndef __INTERFACE_INFO_HH__
#define __INTERFACE_INFO_HH__
#include <QtGui>
#include "def.hh"

class FlowrateGraph;
class InterfaceInfo : public QVBoxLayout
{
public:
    InterfaceInfo(FlowrateDict *ratedict, double interval, QWidget* parent = 0);
    void refresh();
    void redraw();
private:
    void         init();
private:
    FlowrateDict *ratedict;
    double       interval;
    FlowrateGraph *graph;
    QVBoxLayout *infLayout;
};

#endif
