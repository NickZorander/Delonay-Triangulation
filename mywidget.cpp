#include <mywidget.h>
#include <QApplication>
#include<QPainter>
#include<QPaintEvent>
#include<QtWidgets>
#include<cstdlib>
#include <QtDebug>


QVector <QPoint> Points; //массив с точками
QLinkedList<int> MBO; // минимальная внешняя оболочка
QLinkedList<Quadragon> L; // по сути текущая триангуляция
int iter=0; //текущая итерация
QLinkedList<int>::iterator MBO_prev_index; //место точки из предыдущей итерации в массиве MBO
int N;//количество точек в массиве







Edge::Edge(const int& point1, const int& point2)
{
    p1=point1;
    p2=point2;
}

Edge::Edge(){}


bool Edge::operator==(const Edge& E2)
{
    return ( (p1==E2.p1) && (p2==E2.p2) ) || (p1==E2.p2 && p2==E2.p1);
}


Quadragon::Quadragon(const int& E_point1, const int& E_point2, const int& point1, const int& point2)
{
    spine=Edge(E_point1, E_point2);
    p1=point1;
    p2=point2;
}



Edge* Quadragon::get_ext_edges()
{
    Edge* result = new Edge [4];

    result[0]=Edge(spine.p1, p1);
    result[1]=Edge(spine.p1, p2);
    result[2]=Edge(spine.p2, p1);
    result[3]=Edge(spine.p2, p2);

//    qDebug()<< "external edges:";
//    result[0].printEdge();
//    result[1].printEdge();
//    result[2].printEdge();
//    result[3].printEdge();

    return result;
}

void Quadragon::printQuadragon()
{
    qDebug() << "Q( " << Points[p1] << " , [" << Points[spine.p1] << " ," << Points[spine.p2] << "] ," << Points[p2] << " )";
}

void printQuadragonList()
{
    for (QLinkedList<Quadragon>::iterator it=L.begin(); it!=L.end(); ++it)
        {
            it->printQuadragon();
        }
    qDebug() << "__________________"<<endl;
}

void printMBO()
{
    qDebug() << "current MBO:";
    for (QLinkedList<int>::iterator it=MBO.begin(); it!=MBO.end(); ++it)
    {
        qDebug() << Points[*it];
    }

    qDebug()<<"_______________________________";
    qDebug() << "Last point:" << Points[*MBO_prev_index];
}

void Edge::printEdge() const
{
    qDebug() << "E ["<<Points[p1]<<" ,"<<Points[p2]<<"]";
}






//эта функция выполняет две роли:
// 1) вносит изменения в четырехугольник (добивается того, чтобы при перестройке одного четырехугольника это изменение отражалось в списке)
// 2) проверяет этот четырехугольник на условие делоне, и в с лучае чего запускает рекурсию
void Quadragon::rebuild_one_neigbour_Quadragon ( const Edge& neighbour_spine) //ВАЖНО!!! предполагаем, что исходный четырехугольник уже перестроен
{
    qDebug() << "rebuild_one_neigbour_Quadragon";
    qDebug() << "list before rebuild_one_neigbour_Quadragon:";
    printQuadragonList();
    neighbour_spine.printEdge();

    QLinkedList<Quadragon>::iterator it;

    for (it=L.begin(); it!=L.end(); ++it)
        {
            if (it->spine==neighbour_spine) //нашли четырехугольник, хребтом которго является переданное ребро, далее переопределяем у него вершину
                {
                    qDebug() << "neighbour found!:";
                    it->printQuadragon();

                    if ( (it->spine.p1==spine.p1) || (it->spine.p2==spine.p1) ) //если хребты четырехугольников совпадают в точке p1
                        {
                            if ((it->p1==p1)||(it->p1==p2)) //если общей для четырехугольников раньше была вершина p1 перестраиваемого четурехугольника
                                {
                                    it->p1=spine.p2;
                                }

                            if ((it->p2==p1)||(it->p2==p2)) //если общей для четырехугольников раньше была вершина p2 перестраиваемого четурехугольника
                                {
                                    it->p2=spine.p2;
                                }
                        }

                    if ( (it->spine.p1==spine.p2) || (it->spine.p2==spine.p2) )
                        {
                            if ((it->p1==p1)||(it->p1==p2))
                                {
                                    it->p1=spine.p1;
                                }

                            if ((it->p2==p1)||(it->p2==p2))
                                {
                                    it->p2=spine.p1;
                                }
                        }

                    if (!(it->check_for_deloney())) //если после перестройки в соседе нарушилось условие делоне, то сначала исправляем соседа, а потом проверяем его соседей
                        {
                            it->rebuild_Quadragon();
                        }
                    break;
                }

        }

    qDebug() << "list after rebuild_one_neigbour_Quadragon:";
    printQuadragonList();
}



void Quadragon::rebuild_neighbour_Quadragons ()
{
    qDebug() << "rebuild_neigbour_Quadragons";
    qDebug() << "list before rebuild_neigbour_Quadragons:";
    printQuadragonList();

    Edge* ext_edges =get_ext_edges(); //получаем массив внешних ребер

    for (int i=0; i<=3; ++i)
        {
            rebuild_one_neigbour_Quadragon(ext_edges[i]); //для каждого четырехугольника, хребтом которого является данное ребро выполняем перестройку
        }

    qDebug() << "list after rebuild_neigbour_Quadragons:";
    printQuadragonList();
}



void Quadragon::rebuild_Quadragon() //перестраиваем четырехугольник - отрезок, соединяющий вершины, становится хребтом, а точки образовавшие хребет становятся новыми вершинами
{
    qDebug() << "rebuild_Quadragon";
    printQuadragon();

    int t1=p1, t2=p2;

    p1=spine.p1;
    p2=spine.p2;
    spine=Edge(t1,t2);

    qDebug() << "rebuilded Quadragon:";
    printQuadragon();

    rebuild_neighbour_Quadragons();

    qDebug() << "list after rebuild_Quadragon:";
    printQuadragonList();
}




qreal determinant3 (qreal a1, qreal a2,qreal a3,qreal a4,qreal a5,qreal a6,qreal a7,qreal a8,qreal a9)
{
    return (a1*a5*a9 - a1*a6*a8 - a2*a4*a9 + a2*a6*a7 - a3*a5*a7 + a3*a4*a8);
}

bool checkleft (const QPointF& p1, const QPointF& p2, const QPointF& p3)
{

    bool res= ( ( (p2.x()-p1.x())*(p3.y()-p1.y()) - (p2.y()-p1.y())*(p3.x()-p1.x()) ) <0);
//    if (res)
//        qDebug()<< p3<< "is left to [" << p1 << ","<< p2;
    return res;
}

bool checkright (const QPointF& p1, const QPointF& p2, const QPointF& p3)
{
   bool res= ( ( (p2.x()-p1.x())*(p3.y()-p1.y()) - (p2.y()-p1.y())*(p3.x()-p1.x()) ) >0);
//   if (res)
//       qDebug()<< p3<< "is right to [" << p1 << ","<< p2;
   return res;
}


bool check_point_and_triangle_for_deloney (QPoint& p1, QPoint& p2, QPoint& p3, QPoint& p0)
{

    int x1=p1.x(), x2=p2.x(), x3=p3.x(), y1=p1.y(), y2=p2.y(), y3=p3.y(), x0=p0.x(), y0=p0.y();
    qreal a= determinant3(x1, y1,1 , x2, y2, 1, x3, y3, 1);
    qreal b= determinant3(x1*x1+y1*y1, y1, 1, x2*x2+y2*y2, y2, 1, x3*x3+y3*y3, y3, 1);
    qreal c= determinant3(x1*x1+y1*y1, x1, 1, x2*x2+y2*y2, x2, 1, x3*x3+y3*y3, x3,1);
    qreal d= determinant3(x1*x1+y1*y1, x1, y1, x2*x2+y2*y2, x2, y2, x3*x3+y3*y3, x3, y3);
    int sign;

    if (checkleft(p2,p3,p1))
        sign=-1;
    else
        sign=1;

    return (a*(x0*x0+y0*y0)-b*x0+c*y0-d)*sign >=0;
}

bool Quadragon::check_for_deloney() //проверка четырехугольника на выполнение условий Делоне
{
    return check_point_and_triangle_for_deloney(Points[spine.p1], Points[spine.p2], Points[p1],Points[p2]) && check_point_and_triangle_for_deloney(Points[spine.p1], Points[spine.p2], Points[p2],Points[p1]);
}





bool if_cross (const QPointF& a1, const QPointF& a2, const QPointF& b1, const QPointF& b2)
{
    //qDebug() <<"checking ["<< a1 << ", "<< a2 <<"] and [" << b1 << ", "<< b2 <<"]" ;
    bool res= (( (checkleft(b1,b2,a1)&&checkright(b1,b2,a2)) || (checkleft(b1,b2,a2)&&checkright(b1,b2,a1)) ) &&
            ( (checkleft(a1,a2,b1)&&checkright(a1,a2,b2)) || (checkleft(a1,a2,b2)&&checkright(a1,a2,b1)) ));

//    if (res)
//        qDebug()<< "they cross";
//    else
//        qDebug()<< "they does not cross";

    return res;
}

qreal sqr_length (QPoint& A, QPoint& B){
    return ((A.x()-B.x())*(A.x()-B.x())+(A.y()-B.y())*(A.y()-B.y()));
}



bool checkbelong (const QPointF& p1, const QPointF& p2,  const QPointF& p3) //проверка того, лежить ли точка p3 ВНУТРИ отрезка p1p2 (если точка совпадает с одним из концов будет возвращено false)
{
    if ( ( (p2.x()-p1.x())*(p3.y()-p1.y()) - (p2.y()-p1.y())*(p3.x()-p1.x()) ) !=0)
        return false; //если точка даже не лежит на прямой отрезка


    if (p1.x()>p2.x())
        {
            //если p1 правее p2
            if (p1.y()>p2.y())
                {
                    //если p1 выше р2
                    if ( (p3.x()<p1.x() && p3.x()>p2.x()) || (p3.y()<p1.y() && p3.y()>p2.y()) )
                        return true;
                }
            else
                {
                    //если p1 ниже р2
                    if ( (p3.x()<p1.x() && p3.x()>p2.x()) || (p3.y()<p2.y() && p3.y()>p1.y()) )
                        return true;
                }
        }
    else
        {
            //если p1 левее p2
            if (p1.y()>p2.y())
                {
                    //если p1 выше р2
                    if ( (p3.x()<p2.x() && p3.x()>p1.x()) || (p3.y()<p1.y() && p3.y()>p2.y()) )
                        return true;

                }
            else
                {
                    //если p1 ниже р2
                    if ( (p3.x()<p2.x() && p3.x()>p1.x()) || (p3.y()<p2.y() && p3.y()>p1.y()) )
                        return true;
                }
        }
    return false;
}






//ВАЖНО! считаем, что первый аргумент - точка из МВО, а второй - вновь добавляемая точка
bool if_point_is_visible(const QPointF& p1,  const QPointF& p2) //считаем, что все соседние точки в МВО расположены по соседству в списке МВО
{
    QLinkedList<int>::iterator it, jt;
    qDebug() << "checking if point" << p1 << "is visible from" <<p2;
    for (it=MBO.begin(), jt=it, ++it; it!=MBO.end();jt=it, ++it)
        {
            if (if_cross(Points[*it], Points[*jt], p1, p2)) //если отрезки явно пересекаются
                   return false;

            //если явного пересечения нет
            if (checkbelong(p1, p2, Points[*it]) ||  checkbelong(p1, p2, Points[*jt]) )
                   return false; //если точка из МВО лежит на отрезке и закрывает собой видимомть
        }
    if (if_cross(Points[*MBO.begin()], Points[*(MBO.end()-1)], p1, p2))
           return false;


    qDebug() << "point " << p1 << " of MBO is visible from point"<< p2 << endl;
    return true;
}





void add_new_Quadragon (int point1, int point2, int new_point) //первые две точки это концы видимого ребра, третья - вновь добавляемая точка
{
    QLinkedList<Quadragon>::iterator it;
    qDebug() << "add_new_Quadragon starts";

    //ищем в массиве четырехугольник, у которого
    //  1)в хребте содержится одна из точек нашего видимого ребра МВО
    //  2)одна из вершин принадлежит видимому ребру
    // Тогда другая точка хребта такого четырехугольника станет второй вершиной нашего построимого четырехугольника (первой вершиной будет новая точка, а видимое ребро станет хребтом)
    for (it=L.begin(); it!=L.end(); ++it)
        {
            if ( (it->spine.p1==point1 || it->spine.p1==point2) && (it->p1==point1 || it->p2==point1 || it->p1==point2 || it->p2==point2 ) )
                {
                    L.push_back(Quadragon(point1,point2,it->spine.p2, new_point));
                    qDebug() << "added quadragon:";
                    L.last().printQuadragon();
                    break;
                }

            if ( (it->spine.p2==point1 || it->spine.p2==point2) && (it->p1==point1 || it->p2==point1 || it->p1==point2 || it->p2==point2 ) )
                {
                    L.push_back(Quadragon(point1,point2,it->spine.p1, new_point));
                    qDebug() << "added quadragon:";
                    L.last().printQuadragon();
                    break;
                }
        }

}





int next_point (int point) //обработка очередной точки
{
    qDebug()<<endl << "NEXT POINT: "<< Points[point]<<endl;
    qDebug()<< "MBO before next_point";
    printMBO();

    QLinkedList<int>::iterator right_it, left_it; //очевидно, что точка с предыдущей итерации (в силу сортировки) видимая, поэтому идем в две стороны
    int  count_left=0, count_right=0;

    if (MBO_prev_index==MBO.end()-1)
        {
            MBO.push_back(*MBO.begin());
            MBO.erase(MBO.begin());
        }
    else
    {
        if (MBO_prev_index==MBO.begin())
        {
            MBO.push_front(*(MBO.end()-1));
            MBO.erase(MBO.end());
        }
    }

    right_it=MBO_prev_index+1;
    left_it=MBO_prev_index-1;
    printMBO();

//    qDebug()<< "MBO before right:";
//    printMBO();
    bool right_overcome_flag=false;

    while (if_point_is_visible(Points[*right_it], Points[point])) //идем вправо (против часовой)
        {
            qDebug()<< "RIGHT ITER STARTS";
            if (!right_overcome_flag)
                {
                    if ( !if_point_is_visible( QPointF ( (qreal)((Points[*right_it].x()+Points[*(right_it-1)].x()))/2, (qreal)((Points[*right_it].y()+Points[*(right_it-1)].y()))/2) , Points[point]) )
                        break;//проверили середину ребра на видимость
                }
            else
                {
                    if (right_it!=MBO.begin())
                        {
                            if ( !if_point_is_visible( QPointF ( (qreal)((Points[*right_it].x()+Points[*(right_it-1)].x()))/2, (qreal)((Points[*right_it].y()+Points[*(right_it-1)].y()))/2) , Points[point]) )
                                break;//проверили середину ребра на видимость
                        }
                    else
                        {
                            if ( !if_point_is_visible( QPointF ( (qreal)((Points[*right_it].x()+Points[*(MBO.end()-1)].x()))/2, (qreal)((Points[*right_it].y()+Points[*(MBO.end()-1)].y()))/2) , Points[point]) )
                                break;//проверили середину ребра на видимость
                        }
                }


            add_new_Quadragon(*(right_it-1), *right_it, point); //добавляем новый четырехугольник
            ++count_right; //увеличиваем счетчик добавленных четырехугольников
            if (right_it+1==MBO.end())
                {
                    right_it=MBO.begin();
                    right_overcome_flag=true;
                }
            else
                ++right_it;
            printMBO();
            qDebug() << "right_it points to " << Points[*right_it];
            qDebug()<< "RIGHT ITER FINISHED";
        }

//    qDebug() << "right_it points to " << Points[*right_it]<<  endl;
//    qDebug() << "right_OVERCOME_FLaG"<< right_overcome_flag;


    if (count_right>1)
        {
            if (!right_overcome_flag)
                MBO.erase(MBO_prev_index+1, right_it-1); //если перехода не было, то просто стираем все от (не включая) метки до (не включая) предыдущего элемента
            else
                {   //ели переход таки был
                    if (right_it!=MBO.begin()) //если он стоит в начале
                        {
                             MBO.erase(MBO_prev_index+1, MBO.end()); //стереть все до конца
                             if (right_it!=MBO.begin()+1)
                                MBO.erase(MBO.begin(), right_it-1); //стерееть все лишнее с начала
                        }
                    else
                        {   //если он все же первый символ, то надо оставить еще маленький хвостик в конце
                            MBO.erase(MBO_prev_index+1, MBO.end()-1);
                        }
                }
        }



//    qDebug()<< "MBO before left ";
//    printMBO();

    bool left_overcome_flag=false;
    while (if_point_is_visible(Points[*left_it], Points[point])) //идем влево (по часовой)
        {
            qDebug()<< "LEFT ITER STARTS";
            if (!left_overcome_flag)
                {
                    if ( !if_point_is_visible( QPointF ( (qreal)((Points[*left_it].x()+Points[*(left_it+1)].x()))/2, (qreal)((Points[*left_it].y()+Points[*(left_it+1)].y()))/2) , Points[point]) )
                        break;//проверили середину ребра на видимость
                }
            else
                {
                    if (left_it!=MBO.end()-1)
                        {
                            if ( !if_point_is_visible( QPointF ( (qreal)((Points[*left_it].x()+Points[*(left_it+1)].x()))/2, (qreal)((Points[*left_it].y()+Points[*(left_it+1)].y()))/2) , Points[point]) )
                                break;//проверили середину ребра на видимость
                        }
                    else
                        {
                            if ( !if_point_is_visible( QPointF ( (qreal)((Points[*left_it].x()+Points[*(MBO.begin())].x()))/2, (qreal)((Points[*left_it].y()+Points[*(MBO.begin())].y()))/2) , Points[point]) )
                                break;//проверили середину ребра на видимость
                        }
                }

            add_new_Quadragon(*(left_it+1), *left_it, point);
            ++count_left;
            if (left_it==MBO.begin())
                {
                    left_it=MBO.end()-1;
                    left_overcome_flag=true;
                }
            else
                --left_it;
           printMBO();
            qDebug() << "left_it points to " << Points[*left_it] << " and left_it+1 points to " << Points[*left_it+1];
            qDebug()<< "LEFT ITER FINISHED";
        }

//    qDebug() << "left_it points to " << Points[*left_it]<<endl;
//    qDebug() << "left_OVERCOME_FLaG"<< left_overcome_flag;
//    printMBO();

    if (count_left>0)
        {
            //qDebug()<<"count left>1";
            if (!left_overcome_flag)
                {
                    //qDebug()<<"!left_overcome_flag";
                    if (right_overcome_flag)
                        {
                             //qDebug()<<"right_overcome flag";
                            if (right_it==MBO.begin())
                                {
                                     //qDebug()<<"right_it==MBO.begin()";
                                    if (MBO.end()-1!=MBO_prev_index)
                                        {
                                            MBO.erase(left_it+2, MBO_prev_index+1);
                                            //qDebug()<<"must be here";
                                        }
                                    else
                                        MBO.erase(left_it+2, MBO_prev_index);
                                }
                            else
                              MBO.erase(left_it+2, MBO_prev_index+1);
                        }
                    else
                        MBO.erase(left_it+2, MBO_prev_index+1);
                }
            else
                {   //если был левый переход
                     //qDebug()<<"left_overcome flag";
                    if (left_it!=MBO.end()-1)
                        {
                            if (right_it-1!=MBO_prev_index)
                                MBO.erase(MBO.begin(), MBO_prev_index+1);
                            else
                                MBO.erase(MBO.begin(), MBO_prev_index);

                            if (left_it!= MBO.end()-2)
                                MBO.erase(left_it+2, MBO.end());
                        }
                    else
                        {
                                    if (right_it-1!=MBO_prev_index)
                                        MBO.erase(MBO.begin()+1, MBO_prev_index+1);
                                    else
                                        MBO.erase(MBO.begin()+1, MBO_prev_index);
                        }

                }
        }



//    qDebug() << "MBO after deleting:";
//    printMBO();

    if (!left_overcome_flag)
        MBO_prev_index=MBO.insert(left_it+2, point);
    else
        MBO_prev_index=MBO.insert(right_it-1, point);



    qDebug()<< "MBO after next_point";
    printMBO();

    return count_left+count_right;
}







void build_Triangulation ()
{
   MBO.push_back(0);
   MBO.push_back(1);
   MBO.push_back(2);
   MBO_prev_index=MBO.begin()+2;

   printMBO();

    bool vertex_0=if_point_is_visible(Points[0], Points[3]),
         vertex_1=if_point_is_visible(Points[1], Points[3]),
         edge_0_1=if_point_is_visible( QPointF ( ((qreal)(Points[0].x()+Points[1].x()))/2, (qreal)((Points[0].y()+Points[1].y()))/2) , (QPointF)Points[3]),
         edge_1_2=if_point_is_visible( QPointF ( ((qreal)(Points[1].x()+Points[2].x()))/2, (qreal)((Points[1].y()+Points[2].y()))/2) , (QPointF)Points[3]),
         edge_0_2=if_point_is_visible( QPointF ( ((qreal)(Points[0].x()+Points[2].x()))/2, (qreal)((Points[0].y()+Points[2].y()))/2) , (QPointF)Points[3]);

    qDebug()<< "vertex_0: "<< vertex_0;
    qDebug()<< "vertex_1: "<< vertex_1;
    qDebug()<< "edge_0_1: "<< edge_0_1;
    qDebug()<< "edge_1_2: "<< edge_1_2;
    qDebug()<< "edge_0_2: "<< edge_0_2;
    QLinkedList<Quadragon>::iterator jt=L.begin();


    MBO.erase(MBO.begin(), MBO.end());

    if (vertex_0 && vertex_1 && edge_0_2 && edge_1_2 && !edge_0_1) //  когда добавляем два ребра
        {
            L.push_back(Quadragon(0,2, 1,3));
            L.push_back(Quadragon(1,2, 0,3));
            MBO.push_back(0);
            MBO.push_back(1);
            MBO.push_back(3);
            MBO_prev_index=MBO.begin()+2;

            if(!L.begin()->check_for_deloney())
                L.begin()->rebuild_Quadragon();
            if (! (L.begin()+1)->check_for_deloney())
                (L.begin()+1)->rebuild_Quadragon();
        }

    else if (vertex_0 && vertex_1 && edge_0_2 && !edge_1_2 && edge_0_1) //  когда добавляем два ребра
        {
            L.push_back(Quadragon(0,2, 1,3));
            L.push_back(Quadragon(0,1, 2,3));
            MBO.push_back(1);
            MBO.push_back(2);
            MBO.push_back(3);
            MBO_prev_index=MBO.begin()+2;

            if(!L.begin()->check_for_deloney())
                L.begin()->rebuild_Quadragon();
            if (! (L.begin()+1)->check_for_deloney())
                (L.begin()+1)->rebuild_Quadragon();
        }

    else if (vertex_0 && vertex_1 && !edge_0_2 && edge_1_2 && edge_0_1) //  когда добавляем два ребра
        {
            L.push_back(Quadragon(0,1, 2,3));
            L.push_back(Quadragon(1,2, 0,3));
            MBO.push_back(0);
            MBO.push_back(2);
            MBO.push_back(3);
            MBO_prev_index=MBO.begin()+2;;

            if(!L.begin()->check_for_deloney())
                L.begin()->rebuild_Quadragon();
            if (! (L.begin()+1)->check_for_deloney())
                (L.begin()+1)->rebuild_Quadragon();
        }



    else if (!vertex_0 && vertex_1) //когда добавляем 1 ребро
        {
            L.push_back(Quadragon(1,2, 0,3));
            MBO.push_back(1);
            MBO.push_back(0);
            MBO.push_back(2);
            MBO.push_back(3);
            MBO_prev_index=MBO.begin()+3;
            if(!L.begin()->check_for_deloney())
                L.begin()->rebuild_Quadragon();
        }

    else if (vertex_0 && !vertex_1) //когда добавляем 1 ребро
        {
            L.push_back(Quadragon(0,2, 1,3));
            MBO.push_back(0);
            MBO.push_back(1);
            MBO.push_back(2);
            MBO.push_back(3);
            MBO_prev_index=MBO.begin()+3;
            if(!L.begin()->check_for_deloney())
                L.begin()->rebuild_Quadragon();
        }


    qDebug() << "list after init";
    printQuadragonList();

    for (iter=4;iter<=N-1;++iter)
    {
        int quadr_count=next_point(iter); //взяли новую точку из массива, добавили в триангуляцию quadr_count новых четырехугольников
        QLinkedList<Quadragon>::iterator it=L.end()-1;
        qDebug()<< "count:";
        printQuadragonList();
        for (int i=1; i<=quadr_count;++i ) //для каждого из этих четырехкгольников выполняем:
            {
                if (!it->check_for_deloney()) //проверяем на условие делоне
                    {
                        it->rebuild_Quadragon(); //если не выполнилось, то перестраиваем четырехугольник
                    }
                --it; //уменьшаем итератор, тк идем от конца списка
            }
    }

    qDebug()<< "builded triangulation";
    printQuadragonList();

}

//_______________________________________________________________________________________










Triangle::Triangle(QPoint& a, QPoint& b, QPoint& c){
    p1=a;
    p2=b;
    p3=c;
}



bool checkinside (QPoint& p1, QPoint& p2, QPoint& p3, QPoint& cpoint){
    if ( checkleft(p1,p2,cpoint)&&checkleft(p2,p3,cpoint)&&checkleft(p3,p1,cpoint) )
        return true;
    else{
        if ( checkright(p1,p2,cpoint)&&checkright(p2,p3,cpoint)&&checkright(p3,p1,cpoint) )
            return true;
    }
    return false;
}



void MyWidget::paintEvent(QPaintEvent* e){

    QPainter painter(this);
    int i;

    for (i=0; i<=num-1; i++){

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(QPen(Qt::black));

        if (i==paintindex){
            painter.setBrush(QBrush(Qt::yellow));
        }
        else{
            painter.setBrush(QBrush(Qt::lightGray));
        }


        QPolygon t1;
        t1 << TrArray[i].p1 << TrArray[i].p2 << TrArray[i].p3;

        painter.drawPolygon(t1);


    }

}


void MyWidget::mousePressEvent (QMouseEvent* pe){
    QPoint M(pe->x(), pe->y());
    int i;
    for (i=0;i<=num-1;i++){
        if (checkinside(TrArray[i].p1,TrArray[i].p2,TrArray[i].p3,M))
                {
                    paintindex=i;
                    QString S=TrArray[i].printTriangle();
                    QMessageBox::information(this,"Error", S);
                }
    }

    repaint();
}


bool operator==(Triangle& A, Triangle& B)
{
    return  (A.p1==B.p1 && A.p2==B.p2 && A.p3==B.p3) ||
            (A.p1==B.p1 && A.p2==B.p3 && A.p3==B.p2) ||
            (A.p1==B.p2 && A.p2==B.p1 && A.p3==B.p3) ||
            (A.p1==B.p2 && A.p2==B.p3 && A.p3==B.p1) ||
            (A.p1==B.p3 && A.p2==B.p1 && A.p3==B.p2) ||
            (A.p1==B.p3 && A.p2==B.p2 && A.p3==B.p1) ;
}

bool belongs_to_array (Triangle& tr, Triangle* Arr, int size)
{
    for (int i=0; i<=size-1; ++i)
    {
        if (tr==Arr[i])
            return true;
    }
    return false;
}

bool operator<(const QPoint& A, const QPoint& B)
{
    return A.x()<B.x();
}

QString Triangle::printTriangle(){
    QString S;
    S+="T( (";
    S+=QString::number(p1.x());
    S+=",";
    S+=QString::number(p1.y());
    S+=") , (";
    S+=QString::number(p2.x());
    S+=",";
    S+=QString::number(p2.y());
    S+=") , (";
    S+=QString::number(p3.x());
    S+=",";
    S+=QString::number(p3.y());
    S+=") )";
    return S;
}

void MyWidget::printTriangleArray(){


    for (int i=0; i<=num-1; ++i){
        qDebug() << TrArray[i].printTriangle() <<endl;
    }

}


void MyWidget::MakeTriangleArray ()
{
    //здесь происходит ввод данных извне и генерация представления__________________________________________________________________________________________________________

    QFile inputfile("D:\\QT\\New_Triangulation\\points.txt"); //входной файл (сделать нормальный ввод!)

    if (!inputfile.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::information(this,"Error", "path not found!");
        return;
    } //проверили на открытие

    QTextStream stream(&inputfile);

    QString A =stream.readAll(); //сделали содержимое файла строчкой


    QStringList list = A.split(' ', QString::SkipEmptyParts); //распилили строку в список где каждый элемент - одна координата точки
    bool ok;


    N=list[0].toInt(&ok); //первым числом передается число точек


    Points.resize(N);
    for (int i=0; i<=N-1; ++i)
        {
            QPoint p(list[i*2+1].toInt(&ok), list[i*2+2].toInt(&ok));
            Points[i]=p;
        } //создали массив точек
    qSort(Points);
    //________________________________________________________________________________________________________________________________________________________________________

    for (int i=0; i<=N-1; ++i)
        {
           qDebug()<< Points[i];
        }




    build_Triangulation();

    TrArray=new Triangle [2*L.size()]; //сделали память под массив

    num=0;
    for (QLinkedList<Quadragon>::iterator it=L.begin(); it!=L.end(); ++it)
    {       
        Triangle Tr1(Points[it->spine.p1], Points[it->spine.p2], Points[it->p1]);
        Triangle Tr2(Points[it->spine.p1], Points[it->spine.p2], Points[it->p2]);

        if (!belongs_to_array(Tr1, TrArray, num))
            {
                TrArray[num]=Tr1;
                ++num;
            }

        if (!belongs_to_array(Tr2, TrArray, num))
            {
                TrArray[num]=Tr2;
                ++num;
            }
    }

    printTriangleArray();
}



















