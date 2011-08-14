#ifndef OTMLNODEEXT_H
#define OTMLNODEEXT_H

#include <util/point.h>
#include <util/color.h>
#include <util/rect.h>
#include <util/size.h>

//inline bool from_otmlnode(const OTMLNodePtr& node, Color& color)
//{
    //int r, g, b, a;
    //r = node->readAt<int>(0);
    //g = node->readAt<int>(1);
    //b = node->readAt<int>(2);
    //a = 255;
    //if(node->hasChild(3))
        //a = node->readAt<int>(3);
    //return true;
//}

//template <class T>
//bool from_otmlnode(const OTMLNodePtr& node, TPoint<T>& point)
//{
    //point.x = node->readAt<T>(0);
    //point.y = node->readAt<T>(1);
    //return true;
//}

//template <class T>
//bool from_otmlnode(const OTMLNodePtr& node, TSize<T>& size)
//{
    //size.setSize(node->readAt<T>(0), node->readAt<T>(1));
    //return true;
//}

//template <class T>
//bool from_otmlnode(const OTMLNodePtr& node, TRect<T>& rect)
//{
    //rect.setRect(node->readAt<int>(0),
                 //node->readAt<int>(1),
                 //node->readAt<int>(2),
                 //node->readAt<int>(3));
    //return true;
//}

#endif
