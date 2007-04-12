/* $Id: Light.hh,v 1.1 2006/09/04 19:22:52 stuart Exp $ */

#ifndef _LIGHT_HH_

#define _LIGHT_HH_

// Abstract base class for a generic Light source
// Each light has a warm color, cool color and intensity. The intensity need not
// be used at all. And for lights which have only one color, there is a reference
// variable 'color' which points to the warm color. This can be used without having
// to worry about warm and cool colors
// Every light also has a position (although it might be meaningless for directional lights)
// and a boolean variable which indicates active/inactive state of light

#include <BaseObject.hh>
#include <Color.hh>
#include <Vector3d.hh>

enum LightType { Ambient=0, Directional=1, Point=2, Spot=3 };

class Light;
typedef Light * LightPtr;

class Light : public BaseObject
{
  public :

     Vector3d  position;                               // Position

        /* NOTE: ORDER DEPENDENCY HERE! 'color' has to come *AFTER* warmcolor */
     RGBColor  warmcolor;                              // Warm color - RGB
     RGBColor  coolcolor;                              // Cool color - RGB
     RGBColor& color;                                  // Reference to warm color

     double    intensity;                              // Intensity of color

     bool      state;                                  // true/1 = on, false/0 = off
     
  public :

        // Default constructor
     Light()
       : BaseObject(), position(0,0,0),
         warmcolor(1,1,1), coolcolor(1,1,1), color(warmcolor),
         intensity(1), state(true)
       {}

        // 1-arg constructor
     Light(const Vector3d& pos)
       : BaseObject(), position(pos),
         warmcolor(1,1,1), coolcolor(1,1,1), color(warmcolor),
         intensity(1), state(true)
       {}

        // 1-arg constructor. specifies color
     Light(const RGBColor& col)
       : BaseObject(), position(0,0,0),
         warmcolor(col), coolcolor(col), color(warmcolor),
         intensity(1), state(true)
       {}

        // Copy constructor
     Light(const Light& light)
       : BaseObject(light), position(light.position),
         warmcolor(light.warmcolor), coolcolor(light.coolcolor), color(warmcolor),
         intensity(light.intensity), state(light.state)
       {}

        // Destructor
     virtual ~Light()
       {}

        // Assignment operator
     Light& operator = (const Light& light)
       {
         BaseObject :: operator = (light);
         position = light.position;
         warmcolor = light.warmcolor; coolcolor = light.coolcolor;
         intensity = light.intensity; state = light.state;
         return (*this);
       }

        // Return type of light.
     virtual LightType type(void) const = 0;

        // Does this light illuminate given point?
     virtual bool illuminates(const Vector3d& p) const = 0;

        // Compute the cosine factor for given point/normal. Meaningful only for some lights
     virtual double cosfactor(const Vector3d& p, const Vector3d& n) const = 0;
     
        // Illuminate a given point with given normal using this light and return the color
     virtual RGBColor illuminate(const Vector3d& p, const Vector3d& n) const = 0;

        // Illuminate a given point with given normal using this light and return the color
        // The eye position is also given to allow specular computations
     virtual RGBColor illuminate(const Vector3d& p, const Vector3d& n, const Vector3d& e) const = 0;
};
     
#endif // #ifndef _LIGHT_HH_

/*
  $Log: Light.hh,v $
  Revision 1.1  2006/09/04 19:22:52  stuart
  Added includes and libs.

  Revision 2.4  2002/10/17 06:43:58  vinod
  Removed ID field and reference to PETComponent

  Revision 2.2  2000/03/02 07:43:07  vinod
  Added illuminates and cosfactor functions

  Revision 2.1  2000/02/29 06:26:34  vinod
  Added intensity member and a color reference member for single color mode

  Revision 2.0  2000/02/18 01:43:46  vinod
  Release 2.0

  Revision 1.3  2000/02/14 21:09:13  vinod
  Changed color model to use warm and cool colors

  Revision 1.2  2000/02/08 10:24:31  vinod
  Changed enum for LightType to avoid conflicts with class names

  Revision 1.1.1.1  2000/02/08 02:55:22  vinod
  Light classes

*/
