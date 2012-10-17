#ifndef OSMSCOUT_MAP_MAPPAINTERIOS_H
#define OSMSCOUT_MAP_MAPPAINTERIOS_H

/*
 This source is part of the libosmscout-map library
 Copyright (C) 2010  Tim Teulings, Vladimir Vyskocil
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */
#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#define Font UIFont
#define Image CGImageRef
#else
#import <AppKit/AppKit.h>
#define Font NSFont
#define Image CGImageRef
#endif

#include <osmscout/MapPainter.h>

namespace osmscout {
    typedef struct { double x; double y; } Pt;
    
    class MapPainterIOS : public MapPainter {
    private:
        CGContextRef                cg;
        std::vector<Image>          images;         // Cached CGImage for icons
        std::vector<CGPatternRef>   patterns;       // vector of Image for fill patterns
        std::map<size_t,Font *>     fonts;      // Cached fonts
        
    public:
        MapPainterIOS();
        virtual ~MapPainterIOS();
        
        bool DrawMap(const StyleConfig& styleConfig,
                     const Projection& projection,
                     const MapParameter& parameter,
                     const MapData& data,
                     CGContextRef paintCG);
    protected:
        bool HasIcon(const StyleConfig& styleConfig,
                     const MapParameter& parameter,
                     IconStyle& style);
        
        bool HasPattern(const MapParameter& parameter,
                        const FillStyle& style);
        
        void GetTextDimension(const MapParameter& parameter,
                              double fontSize,
                              const std::string& text,
                              double& xOff,
                              double& yOff,
                              double& width,
                              double& height);
        
        void DrawLabel(const Projection& projection,
                       const MapParameter& parameter,
                       const LabelData& label);
        
        void DrawPlateLabel(const Projection& projection,
                            const MapParameter& parameter,
                            const LabelData& label);
        
        void DrawContourLabel(const Projection& projection,
                              const MapParameter& parameter,
                              const LabelStyle& style,
                              const std::string& text,
                              size_t transStart, size_t transEnd);
        
        void DrawIcon(const IconStyle* style,
                      double x, double y);
        
        void DrawSymbol(const SymbolStyle* style,
                        double x, double y);
        
        void DrawPath(const Projection& projection,
                      const MapParameter& parameter,
                      const Color& color,
                      double width,
                      const std::vector<double>& dash,
                      LineStyle::CapStyle startCap,
                      LineStyle::CapStyle endCap,
                      size_t transStart, size_t transEnd);
        
        void DrawArea(const Projection& projection,
                      const MapParameter& parameter,
                      const AreaData& area);
        
        void DrawArea(const FillStyle& style,
                      const MapParameter& parameter,
                      double x,
                      double y,
                      double width,
                      double height);
        
        void SetBrush();
        void SetBrush(const Projection& projection,
                      const MapParameter& parameter,
                      const FillStyle& fillStyle);

        double textLength(const MapParameter& parameter, double fontSize, std::string text);
        
    private:
        Font *GetFont(const MapParameter& parameter, double fontSize);
        CGFloat pathLength(size_t transStart, size_t transEnd);
        Pt originForPositionAlongPath(CGFloat *l, CGFloat nextW, size_t transStart, size_t transEnd);
        CGFloat slopeForPositionAlongPath(CGFloat* l, CGFloat nextW, size_t transStart, size_t transEnd);
    };
}

#endif