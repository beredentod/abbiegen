#include "gui.h"
#include "./ui_gui.h"
#include <bits/stdc++.h>
#include <string>
#include "../../setup.h"
#include "../../graph.h"
#include "../../dijkstra.h"
#include "../../yen.h"
#include "../../simple_svg.hpp"

gui::gui(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::gui)
{
    ui->setupUi(this);
    ui->defaultSave->setCheckState(Qt::Checked);
}

gui::~gui()
{
    delete ui;
}


//Erstellung der SVG-Datei
void MakeSVG (string $OUTPUT_FILE, dPair s, dPair e, dPair sp, dPair ep, set<dPair> pts,
                set<pair<dPair, dPair>> strL, set<pair<dPair, dPair>> path)
{
    using namespace std;
    using namespace svg;

    int WIN_WIDTH = 2000;
    int WIN_HEIGHT = 1000;
    int ZOOM = 120;
    int MOVE = 50;
    int CIRC_DIAM = 20;
    //Pivotkanten und -knoten werden entfernt
    pts.erase(sp);
    pts.erase(ep);
    strL.erase(mp(sp, s));
    strL.erase(mp(e, ep));
    path.erase(mp(sp, s));
    path.erase(mp(e, ep));

    //Groesse des Fenster wird bestimmt
    Dimensions dimensions(WIN_WIDTH, WIN_HEIGHT);
    Document doc($OUTPUT_FILE, Layout(dimensions, Layout::BottomLeft));

    //alle Strassen werden eingefuegt
    for (auto x: strL)
    {
        Polyline polyline (Stroke(3, Color::Black));
        polyline << Point(MOVE+x.first.first*ZOOM, MOVE+x.first.second*ZOOM)
            << Point(MOVE+x.second.first*ZOOM, MOVE+x.second.second*ZOOM);
        doc << polyline;
    }

    //der Pfad wird gekennzeichnet
    for (auto x: path)
    {
        if (x.first.first >= 0 && x.first.second >= 0)
        {
            Polyline polyline (Stroke(3, Color::Red));
            polyline << Point(MOVE+x.first.first*ZOOM, MOVE+x.first.second*ZOOM)
                << Point(MOVE+x.second.first*ZOOM, MOVE+x.second.second*ZOOM);
            doc << polyline;
        }
    }

    //alle Punkte werden eingefuegt
    for (auto x: pts)
        doc << Circle(Point(MOVE+x.first*ZOOM, MOVE+x.second*ZOOM), CIRC_DIAM,
            Fill(Color(0, 0, 0)), Stroke(1, Color(0, 0, 0)));

    //Start- und Zielpunkte werden gekennzeichnet
    doc << Circle(Point(MOVE+(s.first*ZOOM), MOVE+(s.second*ZOOM)), CIRC_DIAM,
        Fill(Color(0, 0, 256)), Stroke(1, Color(0, 0, 256)));
    doc << Circle(Point(MOVE+e.first*ZOOM, MOVE+e.second*ZOOM), CIRC_DIAM,
        Fill(Color(0, 256, 0)), Stroke(1, Color(0, 256, 0)));

    doc.save();
    cout << "SVG-Datei \"" << $OUTPUT_FILE << "\" wurde hergestellt.\n";
}


void gui::on_browse_pressed()
{
    QString inputPath("./input");
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file", inputPath);
    QFile file (fileName);
    if (!file.open(QIODevice::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return;
    }
    currFileInput = fileName;
    ui->textBrowser->setText(currFileInput);

    string strName = fileName.toUtf8().constData();
    size_t found = 0;

    for (string::iterator it = strName.end(); it != strName.begin(); it--)
    {
        if (*it == '/')
            break;
        found ++;
    }
    found--;
    std::string proposedName = strName.substr(strName.size()-found);

    proposedName.resize(proposedName.size()-4);

    string proposedNameMin = proposedName + "_least.svg";
    string proposedNameShort = proposedName + "_shortest.svg";

    proposedName += ".svg";

    file.close();

    if (defaultSave)
    {
        ui->inputSaveResult->setText(QString::fromStdString(proposedName));
        ui->inputSaveMinimal->setText(QString::fromStdString(proposedNameMin));
        ui->inputSaveShort->setText(QString::fromStdString(proposedNameShort));
    }
}

void gui::on_ok_pressed()
{
    QString rawInput = ui->inputMax->text();
    int intNum = rawInput.toInt();
    if (k == -1 || intNum != k)
    {
        k = rawInput.toInt();
        maxProcent = k + 100;

        if (defaultSave)
        {
            QString rawName = ui->inputSaveResult->text();
            string strName1 = rawName.toUtf8().constData();
            if (strName1.find("_") < strName1.size())
            {
                string strName = strName1.substr(0, strName1.find("_"));
                strName += "_" + to_string(int(k));
                strName += ".svg";
                ui->inputSaveResult->setText(QString::fromStdString(strName));
            }
            else
            {
                strName1.resize(strName1.size()-4);
                strName1 += "_" + to_string(int(k));
                strName1 += ".svg";
                ui->inputSaveResult->setText(QString::fromStdString(strName1));
            }
        }
    }
    else
    {
        k = rawInput.toInt();
        maxProcent = k + 100;

        if (defaultSave)
        {
            QString rawName = ui->inputSaveResult->text();
            string strName1 = rawName.toUtf8().constData();
            string strName = strName1.substr(0, strName1.find("_"));
            strName += "_" + to_string(int(k));
            strName += ".svg";
            ui->inputSaveResult->setText(QString::fromStdString(strName));
        }
    }
}

void gui::on_inputMax_returnPressed()
{
    gui::on_ok_pressed();
}

void gui::on_approx_clicked(bool checked)
{
    if (!checked)
    {
        exact = 1;
        ui->textBrowser->setText("Genaue Suche");

        if (defaultSave)
        {
            QString rawName = ui->inputSaveResult->text();
            string strName = rawName.toUtf8().constData();
            if (strName.find("approx") < strName.size())
            {
                strName.erase(strName.find("_approx"), 7);
                ui->inputSaveResult->setText(QString::fromStdString(strName));
            }
        }
    }
    else
    {
        exact = 0;
        ui->textBrowser->setText("Approximierte Suche");

        if (defaultSave)
        {
            QString rawName = ui->inputSaveResult->text();
            string strName = rawName.toUtf8().constData();
            strName.resize(strName.size()-4);
            strName += "_approx.svg";
            ui->inputSaveResult->setText(QString::fromStdString(strName));
        }
    }
}

void gui::on_start_pressed()
{
    using namespace chrono;
    stringstream stream;
    stream << fixed << setprecision(2);

    std::string utf8_text = currFileInput.toUtf8().constData();

    Graph G1(utf8_text, 1);
    Dijkstra dijkstra(&G1);

    //der kuerzeste Weg von allen wird gefunden
    shortestPath = dijkstra.calculateShortestPath(G1.getNode(G1.retrieveStartPointID()), G1.getNode(G1.retrieveEndPointID()))->retrievePath();

    //die kuerzeste Entfernung vom Startknoten bis zum Zielknoten
    scale = shortestPath.first;

    //der zweite Graph -- mit Strassen als Knoten (Graph G')
    Graph G(utf8_text, 2);

    streetWeights = G.retrieveStreetWeights();
    IDToStreet = G.retrieveStreets();
    edgeNum = G.retrieveEdgeNum();

    maxDist = (double)(1 + ((double)k/100))*scale;

    //Der Yen-Algorithmus laeuft vom Startknoten bis zum Endknoten
    Yen yen(G, G.getNode(G.retrieveStartStreet()), G.getNode(G.retrieveEndStreet()));

    int i = 0;
    double currDist;
    double currDistProcent;
    tuple<double, double, vector<int>> currPath;

    //die Zeit wird gemessen
    steady_clock::time_point begin = steady_clock::now();

    //Generierung der Pfade
    generate:
    do
    {
        pair<double, vector<int>> p;
        map<dPair, int> vis;
        //der naechste beste Pfad wird gefunden
        p = yen.generatePath()->retrievePath();

        double dist = 0;
        for (auto x: p.second)
        {
            //Pfade, in denen dreimal ein Punkt besucht wird,
            //werden nicht betrachtet
            pair<dPair, dPair> str = IDToStreet.find(x)->second;

            if (vis.find(str.first) == vis.end())
                vis[str.first] = 1;
            else
            {
                if (vis.find(str.first)->second == 2)
                    goto generate;
                else
                    vis.find(str.first)->second++;
            }
            if (vis.find(str.second) == vis.end())
                vis[str.second] = 1;
            else
            {
                if (vis.find(str.second)->second == 2)
                    goto generate;
                else
                    vis.find(str.second)->second++;
            }

            //die Gewichte an den Knoten (Strassen)
            //werden zugeordnet
            dist += streetWeights.find(x)->second;
        }

        ++i;
        currDist = dist;
        currDistProcent = round(currDist/scale*100);

        //ein neur, fertige Pfad wird in der Menge allPaths gespeichert
        currPath = make_tuple(p.first, dist, p.second);
        allPaths.insert(currPath);

    } while ((exact && currDist > maxDist) || (!exact && currDistProcent > maxProcent));

    steady_clock::time_point end = steady_clock::now();

    bestPath = currPath;

    //das Format der Zeit wird angepasst
    auto ms = duration_cast<milliseconds>(end - begin);
    auto secs = duration_cast<seconds>(ms);
    ms -= duration_cast<milliseconds>(secs);
    auto mins = duration_cast<minutes>(secs);
    secs -= duration_cast<seconds>(mins);
    leastTurns = *(allPaths.begin());

    stream << "\nFolgender Pfad wurde gefunden.\nDie Anzahl von Abbiegungen:\t" << int(get<0>(bestPath)) << "\n";

    if (exact)
        stream << "Die ganze Entfernung:\t\t" <<get<1>(bestPath)<< " (" << setprecision(3) << get<1> (bestPath)/scale*100 << "%)\n";
    else
        stream << "Die ganze Entfernung:\t\t" <<get<1>(bestPath)<< " (" <<  setprecision(3) << round(get<1> (bestPath)/scale*100) << "%)\n";

    stream << "Alle Punkte: " << fixed << setprecision(2);

    /*output += "\nFolgender Pfad wurde gefunden.\nDie Anzahl von Abbiegungen:\t" + to_string(get<0>(bestPath)) + "\n";

    if (exact)
        output += "Die ganze Entfernung:\t\t" + to_string(get<1>(bestPath)) + " (" + to_string(get<1> (bestPath)/scale*100) + "%)\n";
    else
        output += "Die ganze Entfernung:\t\t" + to_string(get<1>(bestPath)) + " (" + to_string(round(get<1> (bestPath)/scale*100)) + "%)\n";

    output += "Alle Punkte: ";*/

    //Formatierung der Punkte
    //Strassen besitzen je zwei Punkte
    //Wiederholungen muessen entfernt werden
    dPair prevA = mp(-1,-1), prevB = mp(-1,-1);
    for (auto y: get<2>(bestPath))
    {
        pair<dPair, dPair> p = IDToStreet.find(y)->second;

        if (prevA.second == -1 && prevB.second == -1)
        {
            bestPoints.pb(p.first);
            bestPoints.pb(p.second);
        }
        else
        {
            if (prevA != p.first && prevB != p.first)
                bestPoints.pb(p.first);
            if (prevA != p.second && prevB != p.second)
                bestPoints.pb(p.second);
        }
        prevA = p.first;
        prevB = p.second;
    }
    bestPoints.erase(bestPoints.begin());
    bestPoints.erase(bestPoints.end());

    //Ausgabe aller Punkte der besten Strasse
    //mit der niedrigsten Anzahl an Abbiegungen
    for (auto x: bestPoints)
        stream << "(" << x.first << ", " << x.second << ") ";
    stream << "\n\n";

  /*      output += "(" + to_string(x.first) + ", " +  to_string(x.second) + ") ";
    output += "\n\n";*/

    //Vorbereitung auf die graphische Ausgabe
    for (auto x: get<2>(bestPath))
        vis_path.insert(IDToStreet.find(x)->second);

    streetToID = G.retrieveStreetsToID();
    IDToPoint = G1.retrieveIDToPoint();
    streetIDToNeighbors = G.retrieveTurns();

    //Umwandlung Indizes -> Punkte
    for (auto x: shortestPath.second)
    {
        dPair a = IDToPoint.find(x)->second;
        shortestPoints.pb(a);
    }

    //Umwandlung Punkte -> Strassen
    for (unsigned long i=0; i < shortestPoints.size()-1; ++i)
    {
        dPair a = shortestPoints[i];
        dPair b = shortestPoints[i+1];

        int id = streetToID.find(mp(a,b))->second;
        shortestStreets.pb(id);
    }

    //Umwandlung Strassen -> Abbiegungen
    for (unsigned long i=0; i < shortestStreets.size()-1; ++i)
    {
        int strA = shortestStreets[i];
        int strB = shortestStreets[i+1];

        bool found = 0;

        for (auto x: streetIDToNeighbors.find(strA)->second)
        {
            if (x.first == strB)
            {
                shortestTurnsNum += x.second;
                found = 1;
                break;
            }
        }
        if (!found)
        {
            for (auto x: streetIDToNeighbors.find(strB)->second)
            {
                if (x.first == strA)
                {
                    shortestTurnsNum += x.second;
                    break;
                }
            }
        }
    }

    //der kuerzeste Pfad
    shortestTurns = make_tuple(shortestTurnsNum, scale, shortestStreets);

    stream << "\nZum Vergleich: der Pfad mit der niedrigsten anzahl von Abbiegungen.\n"<<
    "Die Anzahl von Abbiegungen:\t" << int(get<0>(leastTurns)) << "\n";

    if (exact)
        stream << "Die ganze Entfernung:\t\t" <<get<1>(leastTurns)<< " (" << setprecision(3) << get<1> (leastTurns)/scale*100 << "%)\n\n";
    else
        stream << fixed <<setprecision(2) << "Die ganze Entfernung:\t\t" <<get<1>(leastTurns) <<setprecision(3) << " (" << round(get<1> (leastTurns)/scale*100) << "%)\n\n";

    stream << "\nZum Vergleich: der kürzeste Pfad.\nDie Anzahl von Abbiegungen:\t" << int(get<0>(shortestTurns)) << "\n";

    if (exact)
        stream <<setprecision(2) << "Die ganze Entfernung:\t\t" <<get<1>(shortestTurns)<< " (" <<setprecision(3) << get<1> (shortestTurns)/scale*100 << "%)\n\n\n";

    /*output += "\nZum Vergleich: der Pfad mit der niedrigsten anzahl von Abbiegungen.\n";
    output += "Die Anzahl von Abbiegungen:\t" + to_string(get<0>(leastTurns)) + "\n";

    if (exact)
        output += "Die ganze Entfernung:\t\t" + to_string(get<1>(leastTurns)) + " (" + to_string(get<1> (leastTurns)/scale*100) + "%)\n\n";
    else
        output += "Die ganze Entfernung:\t\t" + to_string(get<1>(leastTurns)) + " (" + to_string(round(get<1> (leastTurns)/scale*100)) + "%)\n\n";

    output += "\nZum Vergleich: der kürzeste Pfad.\nDie Anzahl von Abbiegungen:\t" + to_string(get<0>(shortestTurns)) + "\n";

    if (exact)
        output += "Die ganze Entfernung:\t\t" + to_string(get<1>(shortestTurns)) + " (" + to_string(get<1> (shortestTurns)/scale*100)+ "%)\n\n\n";*/

    output += stream.str();

    //Ausgabe der Zeit
    time += "\n\nZeit: " + to_string(mins.count()) + " min " + to_string(secs.count()) + " s\n";
    output += time;

    retStart = G.retrieveStartPoint();
    retEnd = G.retrieveEndPoint();
    retStartP = G.retrieveStartPointPivot();
    retEndP = G.retrieveEndPointPivot();
    retPoints = G.retrieveAllPoints();
    retStreets = G.retrieveAllStreets();

    for (auto x: get<2>(leastTurns))
        vis_path_minimal.insert(IDToStreet.find(x)->second);

    for (auto x: get<2>(shortestTurns))
        vis_path_shortest.insert(IDToStreet.find(x)->second);

    ui->textBrowser->setText(QString::fromStdString(output));
}


void gui::on_saveAsResult_pressed()
{
    QString *outputPath = new QString("./output/");
    QString *rawInput = new QString(ui->inputSaveResult->text());
    QString name = *outputPath + *rawInput;
    QString fileName = QFileDialog::getSaveFileName(this, "Save as", name);

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }

    std::string utf8_text = fileName.toUtf8().constData();

    MakeSVG(utf8_text, retStart, retEnd, retStartP, retEndP, retPoints, retStreets, vis_path);
    ui->inputSaveResult->setText("");
}

void gui::on_saveAsShort_pressed()
{
    QString *outputPath = new QString("./output/");
    QString *rawInput = new QString(ui->inputSaveShort->text());
    QString name = *outputPath + *rawInput;
    QString fileName = QFileDialog::getSaveFileName(this, "Save as", name);

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }

    std::string utf8_text = fileName.toUtf8().constData();

    MakeSVG(utf8_text, retStart, retEnd, retStartP, retEndP, retPoints, retStreets, vis_path_shortest);
    ui->inputSaveShort->setText("");
}

void gui::on_saveAsMinimal_pressed()
{
    QString *outputPath = new QString("./output/");
    QString *rawInput = new QString(ui->inputSaveMinimal->text());
    QString name = *outputPath + *rawInput;
    QString fileName = QFileDialog::getSaveFileName(this, "Save as", name);

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }

    std::string utf8_text = fileName.toUtf8().constData();

    MakeSVG(utf8_text, retStart, retEnd, retStartP, retEndP, retPoints, retStreets, vis_path_minimal);
    ui->inputSaveMinimal->setText("");
}

void gui::on_saveAll_pressed()
{
    string outputPath = "./output/";

    QString res = ui->inputSaveResult->text();
    QString resMin = ui->inputSaveMinimal->text();
    QString resShort = ui->inputSaveShort->text();

    string resPath = outputPath + res.toUtf8().constData();
    string resMinPath = outputPath + resMin.toUtf8().constData();
    string resShortPath = outputPath + resShort.toUtf8().constData();

    MakeSVG(resMinPath, retStart, retEnd, retStartP, retEndP, retPoints, retStreets, vis_path_minimal);
    MakeSVG(resShortPath, retStart, retEnd, retStartP, retEndP, retPoints, retStreets, vis_path_shortest);
    MakeSVG(resPath, retStart, retEnd, retStartP, retEndP, retPoints, retStreets, vis_path);
    ui->inputSaveResult->setText("");
    ui->inputSaveShort->setText("");
    ui->inputSaveMinimal->setText("");
}

void gui::on_defaultSave_clicked(bool checked)
{
    if (!checked)
    {
        defaultSave = 0;
        ui->inputSaveResult->setText("");
        ui->inputSaveMinimal->setText("");
        ui->inputSaveShort->setText("");
    }
    else
    {
        defaultSave = 1;
        if (k != -1)
        {
            string strName = currFileInput.toUtf8().constData();
            size_t found = 0;

            for (string::iterator it = strName.end(); it != strName.begin(); it--)
            {
                if (*it == '/')
                    break;
                found ++;
            }
            found--;
            std::string proposedName = strName.substr(strName.size()-found);

            proposedName.resize(proposedName.size()-4);

            string proposedNameMin = proposedName + "_least.svg";
            string proposedNameShort = proposedName + "_shortest.svg";

            proposedName += "_" + to_string(int(k));
            proposedName += ".svg";
            ui->inputSaveResult->setText(QString::fromStdString(proposedName));

            ui->inputSaveResult->setText(QString::fromStdString(proposedName));
            ui->inputSaveMinimal->setText(QString::fromStdString(proposedNameMin));
            ui->inputSaveShort->setText(QString::fromStdString(proposedNameShort));

            if (!exact)
            {
                QString rawName = ui->inputSaveResult->text();
                string strName = rawName.toUtf8().constData();
                strName.resize(strName.size()-4);
                strName += "_approx.svg";
                    ui->inputSaveResult->setText(QString::fromStdString(strName));
            }
        }
    }
}
