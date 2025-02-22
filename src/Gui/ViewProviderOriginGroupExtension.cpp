/***************************************************************************
 *   Copyright (c) 2015 Alexander Golubev (Fat-Zer) <fatzer2@gmail.com>    *
 *   Copyright (c) 2016 Stefan Tröger <stefantroeger@gmx.net>              *
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


#include "PreCompiled.h"

#ifndef _PreComp_
#include <boost_bind_bind.hpp>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/nodes/SoSeparator.h>
#endif

#include "ViewProviderOriginGroupExtension.h"
#include "Application.h"
#include "Document.h"
#include "ViewProviderOriginFeature.h"
#include "ViewProviderOrigin.h"
#include "View3DInventorViewer.h"
#include "View3DInventor.h"
#include "Command.h"
#include <App/OriginGroupExtension.h>
#include <App/Document.h>
#include <App/Origin.h>
#include <Base/Console.h>

using namespace Gui;
namespace bp = boost::placeholders;


EXTENSION_PROPERTY_SOURCE(Gui::ViewProviderOriginGroupExtension, Gui::ViewProviderGeoFeatureGroupExtension)

ViewProviderOriginGroupExtension::ViewProviderOriginGroupExtension()
{
    initExtensionType(ViewProviderOriginGroupExtension::getExtensionClassTypeId());
}

ViewProviderOriginGroupExtension::~ViewProviderOriginGroupExtension()
{
}

void ViewProviderOriginGroupExtension::constructChildren (
        std::vector<App::DocumentObject*> &children ) const
{
    auto* group = getExtendedViewProvider()->getObject()->getExtensionByType<App::OriginGroupExtension>();
    if(!group)
        return;

    App::DocumentObject *originObj = group->Origin.getValue();

    // Origin must be first
    if (originObj) {
        children.insert(children.begin(),originObj);
    }
}

void ViewProviderOriginGroupExtension::extensionClaimChildren (std::vector<App::DocumentObject *> &children) const {
    ViewProviderGeoFeatureGroupExtension::extensionClaimChildren (children);
    constructChildren ( children );
}

void ViewProviderOriginGroupExtension::extensionClaimChildren3D (std::vector<App::DocumentObject *> &children) const {
    ViewProviderGeoFeatureGroupExtension::extensionClaimChildren3D (children);
    constructChildren ( children );
}

void ViewProviderOriginGroupExtension::extensionAttach(App::DocumentObject *pcObject) {
    ViewProviderGeoFeatureGroupExtension::extensionAttach ( pcObject );
}

void ViewProviderOriginGroupExtension::extensionUpdateData( const App::Property* prop ) {
    auto propName = prop->getName();
    if(propName && (strcmp(propName,"_GroupTouched")==0
                || strcmp(propName,"Group")==0
                || strcmp(propName,"Shape")==0))
        updateOriginSize();

    ViewProviderGeoFeatureGroupExtension::extensionUpdateData ( prop );
}

void ViewProviderOriginGroupExtension::updateOriginSize () {
    auto owner = getExtendedViewProvider()->getObject();

    if(!owner->getNameInDocument() ||
       owner->isRemoving() ||
       owner->getDocument()->testStatus(App::Document::Restoring))
        return;

    auto* group = owner->getExtensionByType<App::OriginGroupExtension>();
    if(!group)
        return;

    // obtain an Origin and it's ViewProvider
    App::Origin* origin = 0;
    Gui::ViewProviderOrigin* vpOrigin = 0;
    try {
        origin = group->getOrigin ();
        assert (origin);
        if (origin->OriginFeatures.getSize() == 0)
            return;

        Gui::ViewProvider *vp = Gui::Application::Instance->getViewProvider(origin);
        if (!vp) {
            Base::Console().Error ("No view provider linked to the Origin\n");
            return;
        }
        assert ( vp->isDerivedFrom ( Gui::ViewProviderOrigin::getClassTypeId () ) );
        vpOrigin = static_cast <Gui::ViewProviderOrigin *> ( vp );
    } catch (const Base::Exception &ex) {
        Base::Console().Error ("%s\n", ex.what() );
        return;
    }

    // calculate the bounding box for out content
    Base::BoundBox3d bbox(0,0,0,0,0,0);
    for(App::DocumentObject* obj : group->Group.getValues()) {
        ViewProvider *vp = Gui::Application::Instance->getViewProvider(obj);
        if (!vp || !vp->isVisible()) {
            continue;
        }
        bbox.Add ( vp->getBoundingBox() );
    };

    Base::Vector3d size(std::max(std::abs(bbox.MinX),std::abs(bbox.MaxX)),
                        std::max(std::abs(bbox.MinY),std::abs(bbox.MaxY)),
                        std::max(std::abs(bbox.MinZ),std::abs(bbox.MaxZ)));

    for (uint_fast8_t i=0; i<3; i++) {
        if (size[i] < 1e-7) { // TODO replace the magic values (2015-08-31, Fat-Zer)
            size[i] = ViewProviderOrigin::defaultSize();
        }
    }

    vpOrigin->Size.setValue ( size );
}

bool ViewProviderOriginGroupExtension::extensionCanDragObject(App::DocumentObject *obj) const
{
    if(obj->isDerivedFrom(App::Origin::getClassTypeId()))
        return false;
    return ViewProviderGeoFeatureGroupExtension::extensionCanDragObject(obj);
}

namespace Gui {
EXTENSION_PROPERTY_SOURCE_TEMPLATE(Gui::ViewProviderOriginGroupExtensionPython, Gui::ViewProviderOriginGroupExtension)

// explicit template instantiation
template class GuiExport ViewProviderExtensionPythonT<ViewProviderOriginGroupExtension>;
}
