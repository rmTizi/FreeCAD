/***************************************************************************
 *   Copyright (c) 2015 Stefan Tröger <stefeantroeger@gmx.net>             *
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


#ifndef GUI_TASKVIEW_TaskPrimitiveParameters_H
#define GUI_TASKVIEW_TaskPrimitiveParameters_H

#include <memory>
#include <Gui/TaskView/TaskView.h>
#include <Gui/Selection.h>
#include <Gui/DocumentObserver.h>
#include <Gui/TaskView/TaskDialog.h>

#include "TaskSketchBasedParameters.h"
#include "ViewProviderPrimitive.h"
#include "TaskDatumParameters.h"
#include <Mod/PartDesign/App/DatumCS.h>
#include <Mod/PartDesign/App/FeaturePrimitive.h>

class QCheckBox;

namespace App {
class Property;
}

namespace Gui {
class ViewProvider;
}

namespace PartDesignGui { 
class Ui_DlgPrimitives;
class TaskBoxPrimitives : public TaskFeatureParameters
{
    Q_OBJECT

public:
    TaskBoxPrimitives(ViewProviderPrimitive* vp, QWidget* parent = 0);
    ~TaskBoxPrimitives();

    bool setPrimitive(App::DocumentObject *);

public Q_SLOTS:
    void onBoxLengthChanged(double);
    void onBoxWidthChanged(double);
    void onBoxHeightChanged(double);
    void onCylinderRadiusChanged(double);
    void onCylinderHeightChanged(double);
    void onCylinderXSkewChanged(double);
    void onCylinderYSkewChanged(double);
    void onCylinderAngleChanged(double);
    void onSphereRadiusChanged(double);
    void onSphereAngle1Changed(double);
    void onSphereAngle2Changed(double);
    void onSphereAngle3Changed(double);
    void onConeRadius1Changed(double);
    void onConeRadius2Changed(double);
    void onConeAngleChanged(double);
    void onConeHeightChanged(double);
    void onEllipsoidRadius1Changed(double);
    void onEllipsoidRadius2Changed(double);
    void onEllipsoidRadius3Changed(double);
    void onEllipsoidAngle1Changed(double);
    void onEllipsoidAngle2Changed(double);
    void onEllipsoidAngle3Changed(double);
    void onTorusRadius1Changed(double);
    void onTorusRadius2Changed(double);
    void onTorusAngle1Changed(double);
    void onTorusAngle2Changed(double);
    void onTorusAngle3Changed(double);
    void onPrismCircumradiusChanged(double);
    void onPrismHeightChanged(double);
    void onPrismXSkewChanged(double);
    void onPrismYSkewChanged(double);
    void onPrismPolygonChanged(int);
    void onWedgeXmaxChanged(double);
    void onWedgeXminChanged(double);
    void onWedgeYmaxChanged(double);
    void onWedgeYminChanged(double);
    void onWedgeZmaxChanged(double);
    void onWedgeZminChanged(double);
    void onWedgeX2maxChanged(double);
    void onWedgeX2minChanged(double);
    void onWedgeZ2maxChanged(double);
    void onWedgeZ2minChanged(double);

private:
    virtual void refresh();

private:
    QWidget* proxy;
    std::unique_ptr<Ui_DlgPrimitives> ui;
};

class TaskPrimitiveParameters :  public TaskDlgFeatureParameters
{
    Q_OBJECT

public:
    TaskPrimitiveParameters(ViewProviderPrimitive *PrimitiveView);
    ~TaskPrimitiveParameters();

protected:
    virtual QDialogButtonBox::StandardButtons getStandardButtons(void) const;

private:
    TaskBoxPrimitives*     primitive;
    PartGui::TaskAttacher* parameter;
    ViewProviderPrimitive* vp_prm;
};

} //namespace PartDesignGui

#endif // GUI_TASKVIEW_TASKAPPERANCE_H
