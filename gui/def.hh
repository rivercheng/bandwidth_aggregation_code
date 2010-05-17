#ifndef __GUI_DEF_H__
#define __GUI_DEF_H__
#include <QHash>
class Interface;
class FlowrateHistory;
typedef QHash<Interface, FlowrateHistory*> FlowrateDict;
typedef QHash<Interface, long*>  FlowDict;
#endif
