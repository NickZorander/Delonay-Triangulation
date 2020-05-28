#ifndef PAINTING_H
#define PAINTING_H
#include<QtWidgets>
#include<QPainter>
#include<QPaintEvent>

struct Triangle
{
    QPoint p1, p2, p3;

    Triangle (QPoint&,QPoint&,QPoint&);
    Triangle(){}
    QString printTriangle();
};

class MyWidget : public QWidget
{
    Q_OBJECT
    int num; //сколько треугольников в массиве
    Triangle* TrArray; //массив треугольников
    int paintindex; //индекс закрашиваемого треугольника в массиве треугольников
public:
    MyWidget(QWidget* p=0) {num=0;}
    void MakeTriangleArray ();
    virtual ~MyWidget() {}
    void printTriangleArray();
protected:
     void paintEvent(QPaintEvent*e);
     void mousePressEvent (QMouseEvent* pe);
};




//_______________________________________________________________________________________________
bool if_inside_round (QPoint& a, QPoint& circleCenter, qreal circleRadius_square);
QPointF circle_center (QPoint& p1, QPoint& p2, QPoint& p3);
void next_point ();

struct Edge
{
    int p1;
    int p2;
    Edge(const int&, const int&);
    Edge();
    bool operator==(const Edge&);
    void printEdge() const;
};

struct Quadragon
{
    Edge spine;
    int p1;
    int p2;
    Quadragon(const int& E_point1, const int& E_point2, const int& point1, const int& point2); //строит четырехугольник по четырем точкам (первые две еще образуют диагональ)
    Edge* get_ext_edges(); //возвращает массив из четырех внешних ребер
    void rebuild_neighbour_Quadragons (); //ищет все соседствующие четырехугольники, и перестраивает их
    bool check_for_deloney(); //проверка на условие делоне
    void rebuild_Quadragon();
    void rebuild_one_neigbour_Quadragon (const Edge& neighbour_spine); //перестраиваем один четырехугольник - сосед
    void printQuadragon();
};


#endif // PAINTING_H
