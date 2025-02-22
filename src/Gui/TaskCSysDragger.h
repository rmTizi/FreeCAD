/***************************************************************************
 *   Copyright (c) 2015 Thomas Anderson <blobfish[at]gmx.com>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef TASKCSYSDRAGGER_H
#define TASKCSYSDRAGGER_H

#include <Gui/TaskView/TaskDialog.h>
#include <App/DocumentObserver.h>

namespace Gui
{
  class QuantitySpinBox;
  class SoFCCSysDragger;
  class ViewProviderDragger;

  class TaskCSysDragger : public Gui::TaskView::TaskDialog
  {
      Q_OBJECT
    public:
      TaskCSysDragger(ViewProviderDocumentObject *vpObjectIn, SoFCCSysDragger *draggerIn);
      virtual ~TaskCSysDragger() override;
      virtual QDialogButtonBox::StandardButtons getStandardButtons() const override
        { return QDialogButtonBox::Ok;}
      virtual void open() override;
      virtual bool accept() override;
    private Q_SLOTS:
      void onTIncrementSlot(double freshValue);
      void onRIncrementSlot(double freshValue);
      void onToggleShowOnTop(bool);
    private:
      void setupGui();
      App::DocumentObjectT vpObject;
      SoFCCSysDragger *dragger;
      QuantitySpinBox *tSpinBox;
      QuantitySpinBox *rSpinBox;
      App::SubObjectT editObj;
  };
}

#endif // TASKCSYSDRAGGER_H
