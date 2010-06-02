#ifndef __UPDATER_H__
#define __UPDATER_H__
#include "def.hh"
#include "listAddr.hh"
#include "flowratehistory.hh"
class Updater : public QObject {
    Q_OBJECT
public:
   Updater(FlowrateDict *ratedict, FlowDict *flowdict, double interval, QObject *parent = 0)
       :QObject(parent), ratedict_(ratedict), flowdict_(flowdict), interval_(interval) {;}
public slots:
   void update() {
       foreach(Interface inf, flowdict_->keys()) {
           long *flow = flowdict_->value(inf);
           (*ratedict_)[inf]->update((*flow)  * 8 / interval_ / 1024);
           *flow = 0;
       }
   }
private:
   FlowrateDict *ratedict_;
   FlowDict     *flowdict_;
   double       interval_;
};
#endif
