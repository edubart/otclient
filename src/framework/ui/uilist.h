#ifndef UITEXTLIST_H
#define UITEXTLIST_H

#include "uiwidget.h"

class UIListItem;

typedef std::shared_ptr<UIListItem> UIListItemPtr;
typedef std::list<UIListItemPtr> UIListItemList;

class UIListItem
{
public:
    void setText(const std::string& text) { m_text = text; }
    void setData(const boost::any& data) { m_data = data; }

    boost::any getData() { return m_data; }
    std::string getText() { return m_text; }

private:
    std::string m_text;
    boost::any m_data;
    int height;
    /*
    Image m_image;
    Color m_backgroundColor;
    Color m_foregroundColor;
    */
};

class UIList : public UIWidget
{
public:
    UIList();

    virtual void render();

    void insertItem(int row, const UIListItemPtr& item);
    //void insertItems(int row, const UIListItemList& items);
    int addItem(const UIListItemPtr& item);
    //int addItems(const UIListItemList& items);
    void removeItem(const UIListItemPtr& item);
    void removeRow(int row);
    UIListItemPtr takeRow(int row);

    void setCurrentItem(const UIListItemPtr& item);
    void setCurrentRow(int row);

    UIListItemPtr getItem(int row);
    UIListItemPtr getItemAt(const Point& point);
    UIListItemList getItems();
    int getItemRow(const UIListItemPtr& item);
    int getItemsCount();
    UIListItemPtr getCurrentItem();
    int getCurrentRow();

protected:
    virtual void onStyleApply(const OTMLNodePtr& styleNode);
    virtual bool onKeyPress(uchar keyCode, char keyChar, int keyboardModifiers);
    virtual bool onMousePress(const Point& mousePos, UI::MouseButton button);
    virtual bool onMouseMove(const Point& mousePos, const Point& mouseMoved);

    // possible signals
    //onItemActivated
    //onItemChanged
    //onCurrentItemChange
    //onCurrenteRowChange
    //onCurrentTextChange
    //onItemClicked

    std::deque<UIListItem> m_items;
};

#endif
