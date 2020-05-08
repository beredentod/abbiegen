#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include "../../setup.h"
#include "../../graph.h"
#include "../../dijkstra.h"
#include "../../yen.h"

QT_BEGIN_NAMESPACE
namespace Ui { class gui; }
QT_END_NAMESPACE

class gui : public QMainWindow
{
    Q_OBJECT

public:
    gui(QWidget *parent = nullptr);
    ~gui();

private slots:
    void on_browse_pressed();

    void on_ok_pressed();

    void on_inputMax_returnPressed();

    void on_approx_clicked(bool checked);

    void on_start_pressed();

    void on_saveAsResult_pressed();

    void on_saveAsShort_pressed();

    void on_saveAsMinimal_pressed();

    void on_saveAll_pressed();

    void on_defaultSave_clicked(bool checked);

private:
    Ui::gui *ui;
    QString currFileInput = "";
    QString currFileRes = "";
    QString currFileShort = "";
    QString currFileMin = "";
    bool defaultSave = 1;

    double scale;

    pair<double, vector<int>> shortestPath;
    tuple<double, double, vector<int>> bestPath;
    tuple<double, double, vector<int>> leastTurns;
    tuple<double, double, vector<int>> shortestTurns;
    vector<dPair> bestPoints;

    set<tuple<double, double, vector<int>>> allPaths;
    set<pair<dPair, dPair>> vis_path;
    set<pair<dPair, dPair>> vis_path_shortest;
    set<pair<dPair, dPair>> vis_path_minimal;

    map<int, double> streetWeights;
    map<int, pair<dPair, dPair>> IDToStreet;
    int edgeNum;

    double k = -1;
    double maxProcent;
    double maxDist;

    bool exact = 1;

    string time;

    map<pair<dPair,dPair>, int> streetToID;
    map<int, dPair> IDToPoint;
    map<int, set<pair<int, int>>> streetIDToNeighbors;

    vector<int> shortestStreets;
    vector<dPair> shortestPoints;
    int shortestTurnsNum = 0;

    dPair retStart;
    dPair retEnd;
    dPair retStartP;
    dPair retEndP;
    set<dPair> retPoints;
    set<pair<dPair, dPair>> retStreets;

    string output;

};
#endif // GUI_H
