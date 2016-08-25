/*
 OSMScout - a Qt backend for libosmscout and libosmscout-map
 Copyright (C) 2016  Lukas Karas

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

import QtQuick 2.0
import Sailfish.Silica 1.0
import QtPositioning 5.2

import harbour.osmscout.map 1.0

import "../custom"

Page {
    id: layersPage

    Settings {
        id: settings
    }

    Column {
        width: parent.width

        PageHeader { title: qsTr("Map settings") }

        SectionHeader{
            text: qsTr("Online Maps")
        }

        TextSwitch{
            id: onlineTiles
            width: parent.width

            checked: settings.onlineTiles
            text: qsTr("Enable online maps")
            //description: qsTr("Enables online maps")

            onCheckedChanged: {
                settings.onlineTiles = checked;
            }
        }

        ComboBox {
            width: parent.width

            property bool initialized: false

            OnlineTileProviderModel{
                id: providerModel
            }

            label: qsTr("Style")
            menu: ContextMenu {
                Repeater {
                    width: parent.width
                    model: providerModel
                    delegate: MenuItem {
                        text: name
                    }
                }
            }

            onCurrentItemChanged: {
                if (!initialized){
                    return;
                }

                settings.onlineTileProviderId = providerModel.getId(currentIndex)
            }
            Component.onCompleted: {
                for (var i = 0; i < providerModel.count(); i++) {
                    if (providerModel.getId(i) == settings.onlineTileProviderId) {
                        currentIndex = i
                        break
                    }
                }
                initialized = true;
            }
        }
    }
}