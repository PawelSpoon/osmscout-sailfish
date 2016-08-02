#ifndef SETTINGS_H
#define SETTINGS_H

/*
  OSMScout - a Qt backend for libosmscout and libosmscout-map
  Copyright (C) 2013  Tim Teulings

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

#include <memory>

#include <QSettings>

#include <osmscout/RoutingProfile.h>
#include "InputHandler.h"

/**
 * Settings provide global instance that extends Qt's QSettings
 * by properties with signals
 */
class Settings: public QObject
{
  Q_OBJECT
  Q_PROPERTY(double   mapDPI    READ GetMapDPI  WRITE SetMapDPI   NOTIFY MapDPIChange)
  Q_PROPERTY(MapView  *mapView  READ GetMapView WRITE SetMapView  NOTIFY MapViewChanged)

signals:
  void MapDPIChange(double dpi);
  void MapViewChanged(MapView *view);
  
private:
  QSettings settings;
  double    physicalDpi;
  MapView   *view;

public:
  Settings();
  ~Settings();

  void SetMapDPI(double dpi);
  double GetMapDPI() const;

  MapView *GetMapView();
  void SetMapView(MapView *view);
  
  osmscout::Vehicle GetRoutingVehicle() const;
  void SetRoutingVehicle(const osmscout::Vehicle& vehicle);
  
  static Settings* GetInstance();
  static void FreeInstance();
};

class QmlSettings: public QObject{
  Q_OBJECT
  Q_PROPERTY(double   mapDPI    READ GetMapDPI  WRITE SetMapDPI   NOTIFY MapDPIChange)
  Q_PROPERTY(QObject  *mapView  READ GetMapView WRITE SetMapView  NOTIFY MapViewChanged)

signals:
  void MapDPIChange(double dpi);
  void MapViewChanged(MapView *view);

public:
  QmlSettings();
  
  inline ~QmlSettings(){};

  void SetMapDPI(double dpi);
  double GetMapDPI() const;  
  
  MapView *GetMapView() const;
  void SetMapView(QObject *view);  
};

#endif
