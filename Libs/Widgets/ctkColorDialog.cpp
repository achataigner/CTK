/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// QT includes
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QVariant>

// CTK includes
#include "ctkColorDialog.h"

QList<QWidget*> ctkColorDialog::DefaultTabs;
int ctkColorDialog::DefaultTab = -1;

//------------------------------------------------------------------------------
class ctkColorDialogPrivate
{
  Q_DECLARE_PUBLIC(ctkColorDialog);
protected:
  ctkColorDialog* const q_ptr;
public:
  ctkColorDialogPrivate(ctkColorDialog& object);
  void init();
  QTabWidget* LeftTabWidget;
  QWidget*    BasicTab;
};

//------------------------------------------------------------------------------
ctkColorDialogPrivate::ctkColorDialogPrivate(ctkColorDialog& object)
  :q_ptr(&object)
{
  this->LeftTabWidget = 0;
}

//------------------------------------------------------------------------------
void ctkColorDialogPrivate::init()
{
  Q_Q(ctkColorDialog);
  QVBoxLayout* mainLay = qobject_cast<QVBoxLayout*>(q->layout());
  QHBoxLayout* topLay = qobject_cast<QHBoxLayout*>(mainLay->itemAt(0)->layout());
  QVBoxLayout* leftLay = qobject_cast<QVBoxLayout*>(topLay->takeAt(0)->layout());
  
  leftLay->setParent(0);
  this->BasicTab = new QWidget(q);
  this->BasicTab->setLayout(leftLay);

  this->LeftTabWidget = new QTabWidget(q);
  topLay->insertWidget(0, this->LeftTabWidget);
  this->LeftTabWidget->addTab(this->BasicTab, QObject::tr("Basic"));
}

//------------------------------------------------------------------------------
ctkColorDialog::ctkColorDialog(QWidget* parent)
  : QColorDialog(parent)
  , d_ptr(new ctkColorDialogPrivate(*this))
{
  Q_D(ctkColorDialog);
  d->init();
}

//------------------------------------------------------------------------------
ctkColorDialog::ctkColorDialog(const QColor& initial, QWidget* parent)
  : QColorDialog(initial, parent)
  , d_ptr(new ctkColorDialogPrivate(*this))
{
  Q_D(ctkColorDialog);
  d->init();
}

//------------------------------------------------------------------------------
ctkColorDialog::~ctkColorDialog()
{
}

//------------------------------------------------------------------------------
void ctkColorDialog::insertTab(int tabIndex, QWidget* widget, const QString& label)
{
  Q_D(ctkColorDialog);
  d->LeftTabWidget->insertTab(tabIndex, widget, label);
}

//------------------------------------------------------------------------------
void ctkColorDialog::setCurrentTab(int index)
{
  Q_D(ctkColorDialog);
  d->LeftTabWidget->setCurrentIndex(index);
}

//------------------------------------------------------------------------------
void ctkColorDialog::removeTab(int index)
{
  Q_D(ctkColorDialog);
  d->LeftTabWidget->removeTab(index);
}

//------------------------------------------------------------------------------
int ctkColorDialog::indexOf(QWidget* widget)const
{
  Q_D(const ctkColorDialog);
  return d->LeftTabWidget->indexOf(widget);
}

//------------------------------------------------------------------------------
QWidget* ctkColorDialog::widget(int index)const
{
  Q_D(const ctkColorDialog);
  return d->LeftTabWidget->widget(index);
}

//------------------------------------------------------------------------------
QColor ctkColorDialog::getColor(const QColor &initial, QWidget *parent, const QString &title,
                              ColorDialogOptions options)
{
  ctkColorDialog dlg(parent);
  if (!title.isEmpty())
    {
    dlg.setWindowTitle(title);
    }
  dlg.setOptions(options | QColorDialog::DontUseNativeDialog);
  dlg.setCurrentColor(initial);
  foreach(QWidget* tab, ctkColorDialog::DefaultTabs)
    {
    dlg.insertTab(tab->property("tabIndex").toInt(), tab, tab->windowTitle());
    if (!tab->property("signal").isValid())
      {
      QObject::connect(tab, tab->property("signal").toString().toLatin1(),
                       &dlg, SLOT(setColor(QColor)));
      }
    }
  dlg.setCurrentTab(ctkColorDialog::DefaultTab);
  dlg.exec();
  foreach(QWidget* tab, ctkColorDialog::DefaultTabs)
    {
    dlg.removeTab(dlg.indexOf(tab));
    if (tab->property("signal").isValid())
      {
      QObject::disconnect(tab, tab->property("signal").toString().toLatin1(),
                          &dlg, SLOT(setColor(QColor)));
      }
    tab->setParent(0);
    tab->hide();
    }
  
  return dlg.selectedColor();
}

//------------------------------------------------------------------------------
void ctkColorDialog::insertDefaultTab(int tabIndex, QWidget* widget, const QString& label, const char* signal)
{
  widget->setWindowTitle(label);
  widget->setProperty("signal", signal);
  widget->setProperty("tabIndex", tabIndex);

  ctkColorDialog::DefaultTabs << widget;
  widget->setParent(0);
}

//------------------------------------------------------------------------------
void ctkColorDialog::setDefaultTab(int index)
{
  ctkColorDialog::DefaultTab = index;
}

//------------------------------------------------------------------------------
void ctkColorDialog::setColor(const QColor& color)
{
  this->QColorDialog::setCurrentColor(color);
}


