#ifndef __FLOWRATE_GRAPH_HH__
#define __FLOWRATE_GRAPH_HH__
#include <QHash>
#include <qwt_plot.h>
#include <qwt_legend.h>
#include <qwt_plot_curve.h>
#include <QColor>
#include "flowratehistory.hh"
#include "def.hh"
#include "listAddr.hh"

class FlowrateGraph : public QwtPlot
{
    Q_OBJECT
    private:
        FlowrateDict *dict_;
        double        interval_;
        bool          overlap_;
        QList<QColor> colorList_;
    public:
        FlowrateGraph( FlowrateDict *dict, double interval, QWidget *parent = 0, char *name = 0 ) 
            : QwtPlot (QwtText(name), parent), dict_(dict), interval_(interval), \
              overlap_(true) {
            setTitle( "Flowrate");

            QwtLegend *legend = new QwtLegend(this);
            insertLegend(legend);

            setAxisTitle(xBottom, "Time  (second)");
            setAxisTitle(yLeft,   "Flowrate (Kbps)");

            colorList_.append(Qt::red);
            colorList_.append(Qt::blue);
            colorList_.append(Qt::green);
            colorList_.append(Qt::cyan);
            colorList_.append(Qt::magenta);
            colorList_.append(Qt::yellow);

            replot();
        }
public slots:
        void plot() {
            clear();
            QVector<double> totalFlow;
            QList< QVector<double> > flowList;
            QList< QString >         nameList;
            foreach(Interface inf, dict_->keys()) {
                QVector<double> values = dict_->value(inf)->toArray();
                if (totalFlow.isEmpty()) {
                    totalFlow = values;
                } else {
                    totalFlow += values;
                }
                if (overlap_) {
                    flowList.append(values);
                } else {
                    flowList.append(totalFlow);
                }
                nameList.append(inf.name);
            }
            
            QVector<double> xvec;
            for (int i=totalFlow.size() - 1; i >= 0 ; i--) {
                xvec.append(-i*interval_);
            }

            for(int i = 0; i < flowList.size(); i++) {
                QwtPlotCurve *curve = new QwtPlotCurve(nameList[i]);
                curve->attach(this);
                curve->setData(xvec.constData(), flowList[i].constData(), xvec.size());
                curve->setPen(QPen(colorList_[i]));
            }

            if (overlap_) {
                QwtPlotCurve *curve = new QwtPlotCurve("total");
                curve->attach(this);
                curve->setData(xvec.constData(), totalFlow.constData(), xvec.size());
                curve->setPen(QPen(Qt::black));
            }
            replot();
        }
};
#endif

