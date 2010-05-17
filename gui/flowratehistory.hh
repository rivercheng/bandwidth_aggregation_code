#ifndef __FLOWRATE_HISTORY_HH__
#define __FLOWRATE_HISTORY_HH__
#include <QObject>
#include <QQueue>
#include <QVector>
#include <QDebug>

class FlowrateHistory : public QObject {
    Q_OBJECT
public:
    FlowrateHistory(int size, double initialValue = 0) {
        for (int i = 0; i < size; i++) {
            history_.enqueue(initialValue);
        }
    }
    
    QVector<double> toArray() const{
        QVector<double> vec;
        foreach(double v, history_) {
            //qDebug() << v;
            vec.append(v);
        }
        //qDebug() << "end total" << vec.size();
        return vec;
    }
public slots:
    void update(double flowrate) {
        history_.dequeue();
        history_.enqueue(flowrate);
    }
private:
    QQueue<double> history_;
};
#endif
