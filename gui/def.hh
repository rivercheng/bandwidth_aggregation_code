#ifndef __GUI_DEF_H__
#define __GUI_DEF_H__
#include <QHash>
#include <QTextStream>
extern QTextStream cerr;
extern QTextStream cout;
class Interface;
class FlowrateHistory;
typedef QHash<Interface, FlowrateHistory*> FlowrateDict;
typedef QHash<Interface, long*>  FlowDict;
#endif
