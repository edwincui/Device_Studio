#include "qonelanguageview.h"

#include "qlanguageitemdeletegate.h"

#include "../../../libs/kernel/language/qlanguagemanager.h"
#include "../../../libs/kernel/language/qlanguage.h"
#include "../../../libs/platform/qlanguageid.h"
#include "../../../libs/platform/qbaseitemdelegate.h"
#include "../../../libs/platform/qbuttonlineedit.h"


QOneLanguageView::QOneLanguageView(QLanguage * language,QWidget* parent):
    QBaseListView(parent),
    m_language(language)
{
    QLanguageInfo info = QLanguageID::getLanguageInfo(language->getID());
    setHeaderLabels(QStringList()<<tr("Default")<<info.m_name);
    setColumnWidth(0,300);

    connect(language,SIGNAL(itemAdded(QString)),this,SLOT(itemAdded(QString)));
    connect(language,SIGNAL(itemDeled(QString)),this,SLOT(itemDeled(QString)));

    QStringList list = m_language->getUuids();

    QLanguageManager *manager = m_language->getManager();
    foreach(QString str,list)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(this);
        QLanguageItem *li = manager->getItem(str);
        item->setText(0,li->m_name);
        item->setText(1,m_language->getValue(li->m_uuid));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
        m_keyToItem.insert(li->m_uuid,item);
        m_itemToKey.insert(item,li->m_uuid);
    }

    m_itemDeletegate = new QLanguageItemDeletegate;

    setItemDelegate(m_itemDeletegate);

    connect(m_itemDeletegate,SIGNAL(editValue(QString,QModelIndex)),
            this,SLOT(itemEdited(QString,QModelIndex)));
    connect(m_language,SIGNAL(itemUpdated(QString)),
            this,SLOT(itemUpdate(QString)));
}

QOneLanguageView::~QOneLanguageView()
{

}

void QOneLanguageView::itemAdded(const QString &key)
{
    if(m_keyToItem.keys().contains(key))
    {
        return;
    }

    QTreeWidgetItem *item = new QTreeWidgetItem(this);
    QLanguageItem *li = m_language->getManager()->getItem(key);
    item->setData(0,Qt::UserRole,li->m_uuid);
    item->setText(0,li->m_name);
    item->setText(1,m_language->getValue(li->m_uuid));
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
    m_keyToItem.insert(li->m_uuid,item);
    m_itemToKey.insert(item,li->m_uuid);
}

void QOneLanguageView::itemDeled(const QString &key)
{
    QTreeWidgetItem *item = m_keyToItem.value(key);

    m_keyToItem.remove(key);
    m_itemToKey.remove(item);
    delete item;
}

void QOneLanguageView::clickEditItem(QTreeWidgetItem * item,int index)
{
    if(index != 0)
    {
        editItem(item,index);
    }
}

void QOneLanguageView::itemEdited(const QString &value, const QModelIndex &index)
{
    QTreeWidgetItem *item = itemFromIndex(index);
    m_language->setValue(m_itemToKey.value(item),value);
}

void QOneLanguageView::itemUpdate(const QString &key)
{
    QTreeWidgetItem *item = m_keyToItem.value(key);
    item->setText(1,m_language->getValue(key));
}
